# -*- coding: utf-8 -*-
"""
/***************************************************************************
 globalfate
                                 A QGIS plugin
 Simulates contaminants fate and transport through the global river network
                              -------------------
        begin                : 2018-09-25
        git sha              : $Format:%H$
        copyright            : (C) 2018 by Carme Font
        email                : cfont@icra.cat
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
"""
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication, Qt
from PyQt4.QtGui import QAction, QIcon, QMessageBox, QPushButton, QLineEdit, QFileDialog, QApplication
# Initialize Qt resources from file resources.py
import resources
# Import the code for the dialog
from globalfate_dialog import globalfateDialog
from globalfate_dialog_geo1 import globalfateDialog_geo1
from globalfate_dialog_geo0 import globalfateDialog_geo0
import os.path
from aux_functions import check_filename
#from geo_windows import *
import platform
from qgis.core import *
import os
import time

class globalfate:
    """QGIS Plugin Implementation."""
    
    def __init__(self, iface):
        """Constructor.

        :param iface: An interface instance that will be passed to this class
            which provides the hook by which you can manipulate the QGIS
            application at run time.
        :type iface: QgisInterface
        """
        # Save reference to the QGIS interface
        self.iface = iface
        # initialize plugin directory
        self.plugin_dir = os.path.dirname(__file__)
        # initialize locale
        locale = QSettings().value('locale/userLocale')[0:2]
        locale_path = os.path.join(
            self.plugin_dir,
            'i18n',
            'globalfate_{}.qm'.format(locale))

        if os.path.exists(locale_path):
            self.translator = QTranslator()
            self.translator.load(locale_path)

            if qVersion() > '4.3.3':
                QCoreApplication.installTranslator(self.translator)


        # Declare instance attributes
        self.actions = []
        self.menu = self.tr(u'&GLOBAL-FATE')
        self.toolbar = self.iface.addToolBar(u'globalfate')
        self.toolbar.setObjectName(u'globalfate')
        
        abspath = os.path.abspath(__file__)
        dname = os.path.dirname(abspath)
        os.chdir(dname)
        
    # noinspection PyMethodMayBeStatic
    def tr(self, message):
        """Get the translation for a string using Qt translation API.

        We implement this ourselves since we do not inherit QObject.

        :param message: String for translation.
        :type message: str, QString

        :returns: Translated version of message.
        :rtype: QString
        """
        # noinspection PyTypeChecker,PyArgumentList,PyCallByClass
        return QCoreApplication.translate('globalfate', message)


    def add_action(
        self,
        icon_path,
        text,
        callback,
        enabled_flag=True,
        add_to_menu=True,
        add_to_toolbar=True,
        status_tip=None,
        whats_this=None,
        parent=None):
        """Add a toolbar icon to the toolbar.

        :param icon_path: Path to the icon for this action. Can be a resource
            path (e.g. ':/plugins/foo/bar.png') or a normal file system path.
        :type icon_path: str

        :param text: Text that should be shown in menu items for this action.
        :type text: str

        :param callback: Function to be called when the action is triggered.
        :type callback: function

        :param enabled_flag: A flag indicating if the action should be enabled
            by default. Defaults to True.
        :type enabled_flag: bool

        :param add_to_menu: Flag indicating whether the action should also
            be added to the menu. Defaults to True.
        :type add_to_menu: bool

        :param add_to_toolbar: Flag indicating whether the action should also
            be added to the toolbar. Defaults to True.
        :type add_to_toolbar: bool

        :param status_tip: Optional text to show in a popup when mouse pointer
            hovers over the action.
        :type status_tip: str

        :param parent: Parent widget for the new action. Defaults None.
        :type parent: QWidget

        :param whats_this: Optional text to show in the status bar when the
            mouse pointer hovers over the action.

        :returns: The action that was created. Note that the action is also
            added to self.actions list.
        :rtype: QAction
        """

        # Create the dialog (after translation) and keep reference
        self.dlg = globalfateDialog()

        # Set rasters and parameters names
        self.parameters = ["a", "b", "c", "d", "n", "excretion rate", "k", "w1", "w2", "save partial", "geo"]

        icon = QIcon(icon_path)
        action = QAction(icon, text, parent)
        action.triggered.connect(callback)
        action.setEnabled(enabled_flag)

        if status_tip is not None:
            action.setStatusTip(status_tip)

        if whats_this is not None:
            action.setWhatsThis(whats_this)

        if add_to_toolbar:
            self.toolbar.addAction(action)

        if add_to_menu:
            self.iface.addPluginToMenu(
                self.menu,
                action)

        self.actions.append(action)

        return action

    def initGui(self):
        """Create the menu entries and toolbar icons inside the QGIS GUI."""

        icon_path = ':/plugins/globalfate/icon.png'
        self.add_action(
            icon_path,
            text=self.tr(u'globalfate'),
            callback=self.run,
            parent=self.iface.mainWindow())

        # input rasters file name
        self.dlg.toolButton_1.clicked.connect(self.onInputFileButtonClicked_1)
        self.dlg.toolButton_2.clicked.connect(self.onInputFileButtonClicked_2)
        self.dlg.toolButton_3.clicked.connect(self.onInputFileButtonClicked_3)
        self.dlg.toolButton_4.clicked.connect(self.onInputFileButtonClicked_4)
        self.dlg.toolButton_5.clicked.connect(self.onInputFileButtonClicked_5)
        self.dlg.toolButton_6.clicked.connect(self.onInputFileButtonClicked_6)

        # partial results folder
        self.dlg.toolButton.clicked.connect(self.onInputFileButtonClicked)

        # button 'Run'
        self.dlg.pushButton.clicked.connect(self.globalfate_run)

        # button 'Cancel' and 'Reset'
        self.dlg.button_box.clicked.connect(self.globalfate_cancel)


    def unload(self):
        """Removes the plugin menu item and icon from QGIS GUI."""
        for action in self.actions:
            self.iface.removePluginMenu(
                self.tr(u'&GLOBAL-FATE'),
                action)
            self.iface.removeToolBarIcon(action)
        # remove the toolbar
        del self.toolbar


    def run(self):
        """Run method that performs all the real work"""
        # show the dialog
        self.dlg.show()
        # Run the dialog event loop
        result = self.dlg.exec_()
        # See if OK was pressed
        if result:
            # Do something useful here - delete the line containing pass and
            # substitute with your code.
            pass

    # function to take file name
    def onInputFileButtonClicked_1(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_1.clear()
            self.dlg.lineEdit_1.setText(filename)
            
    def onInputFileButtonClicked_2(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_2.clear()
            self.dlg.lineEdit_2.setText(filename)
            
    def onInputFileButtonClicked_3(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_3.clear()
            self.dlg.lineEdit_3.setText(filename)
            
    def onInputFileButtonClicked_4(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_4.clear()
            self.dlg.lineEdit_4.setText(filename)
            
    def onInputFileButtonClicked_5(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_5.clear()
            self.dlg.lineEdit_5.setText(filename)
    
    def onInputFileButtonClicked_6(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_6.clear()
            self.dlg.lineEdit_6.setText(filename)

    def onInputFileButtonClicked(self):
        filename = QFileDialog.getExistingDirectory()
        if filename:
            self.dlg.lineEdit.clear()
            self.dlg.lineEdit.setText(filename)

    def ongeoInputFileButtonClicked_1(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.geoW.lineEdit_1.clear()
            self.geoW.lineEdit_1.setText(filename)

    def ongeoInputFileButtonClicked_2(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.geoW.lineEdit_2.clear()
            self.geoW.lineEdit_2.setText(filename)
            
    def ongeoInputFileButtonClicked_3(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.geoW.lineEdit_3.clear()
            self.geoW.lineEdit_3.setText(filename)
            
    def ongeoInputFileButtonClicked_4(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.geoW.lineEdit_4.clear()
            self.geoW.lineEdit_4.setText(filename)

    
    # function to call from button 'Continue'
    def globalfate_run(self):
        current_os = platform.system()
        if current_os == 'Windows': aux = open("C:/tmp/dir.txt", "w")
        if current_os != 'Windows': aux = open("/tmp/dir.txt", "w")
        aux.write(os.getcwd())
        
        filename1 = self.dlg.lineEdit_1.text()
        filename2 = self.dlg.lineEdit_2.text()
        filename3 = self.dlg.lineEdit_3.text()
        filename4 = self.dlg.lineEdit_4.text()
        filename5 = self.dlg.lineEdit_5.text()
        filename6 = self.dlg.lineEdit_6.text()
        par_k = self.dlg.lineEdit_7.text()
        par_g = self.dlg.lineEdit_8.text()
        par_w1 = self.dlg.lineEdit_9.text()
        par_w2 = self.dlg.lineEdit_10.text()
        if not par_w2: par_w2 = 0
        if self.dlg.checkBox_1.isChecked(): geo = 0
        else: geo = 1
        if self.dlg.checkBox_3.isChecked(): 
            partial = 1
            dir_partial = self.dlg.lineEdit.text()
            if not dir_partial:
                msgBox = QMessageBox()
                msgBox.setText("Folder to save results is missing.\nTry again")
                msgBox.exec_()
                return
        else: partial = 0

        rasters = ["Hierarchical order", "Flow direction", "Contaminant consumption", "Human population", "Waste water rate to treatment plant"]
        filenames = [filename1, filename2, filename3, filename4, filename5]
        parnames = [7.2, 0.5, 0.27, 0.3, 0.044, par_g, par_k, par_w1, par_w2, partial, geo]
        
        file_rasters = open('files/input_rasters.txt', 'w')
        a=0 ## build input_rasters
        for i in range(len(rasters)):
            if check_filename(file_rasters, rasters[i], filenames[i]) is -1: 
                break
            else: 
                a+=1
        if filename6: file_rasters.write(filename6)
        file_rasters.close()
        ## end input_rasters
        
        file_params = open('files/input_parameters.txt', 'w')
        if a == len(rasters):
            for i in range(len(self.parameters)):
                if check_filename(file_params, self.parameters[i], parnames[i], p=1) is -1:
                    break
                else:
                    a+=1
        
        file_params.close()
        
        if partial == 1: aux.write(dir_partial)
        aux.close()
        # end input_parameters
            
        self.a=a # 5 + 11
        if a == (len(rasters)+len(self.parameters)):
            self.rasters = rasters
            if geo == 1:
                self.loadGeoRasters()
            if geo == 0:
                self.loadNoGeoRasters()
        
    def globalfate_cancel(self):
        self.dlg.lineEdit_1.clear()
        self.dlg.lineEdit_2.clear()
        self.dlg.lineEdit_3.clear()
        self.dlg.lineEdit_4.clear()
        self.dlg.lineEdit_5.clear()
        self.dlg.lineEdit_6.clear()
        self.dlg.lineEdit_7.clear()
        self.dlg.lineEdit_8.clear()
        self.dlg.lineEdit_9.clear()
        self.dlg.lineEdit_10.clear()
        self.dlg.lineEdit.clear()
        self.dlg.checkBox_1.setChecked(False)
        self.dlg.checkBox_3.setChecked(False)

    def globalfategeo1_cancel(self):
        self.geoW.lineEdit_1.clear()
        self.geoW.lineEdit_2.clear()
        self.geoW.lineEdit_3.clear()
        self.geoW.lineEdit_4.clear()

    def globalfategeo1_ok(self):
        filename1 = self.geoW.lineEdit_1.text()
        filename2 = self.geoW.lineEdit_2.text()
        filename3 = self.geoW.lineEdit_3.text()
        filename4 = self.geoW.lineEdit_4.text()
        filenames = [filename1, filename2, filename3, filename4]
        rasters=["Runoff", "Slope", "Lakes index", "Lakes volume"]
        file_rasters = open('files/input_rasters.txt', 'a')
            
        a=0
        for i in range(len(filenames)):
            if check_filename(file_rasters, rasters[i], filenames[i]) is -1: 
                break
            else: 
                a+=1
        self.rasters += rasters

        self.a += a

         # 9 + 11
        if self.a == (len(self.rasters)+len(self.parameters)):
            self.globalfate_on()
    
    
    def globalfategeo0_cancel(self):
        self.geoW.lineEdit_1.clear()
        self.geoW.lineEdit_2.clear()

    def globalfategeo0_ok(self):
        filename1 = self.geoW.lineEdit_1.text()
        filename2 = self.geoW.lineEdit_2.text()
        filenames = [filename1, filename2]
        rasters=["flow_acc", "RT"]
        file_rasters = open('files/input_rasters.txt', 'a')
        a=0
        for i in range(len(filenames)):
            if check_filename(file_rasters, rasters[i], filenames[i]) is -1: 
                break
            else: 
                a+=1
        self.rasters += rasters

        self.a += a # 7 + 11
        if self.a == (len(self.rasters)+len(self.parameters)):
            self.globalfate_on()
        

    def loadGeoRasters(self):
        self.geoW = globalfateDialog_geo1() #runoff + slope + lakes
        self.geoW.show()

        self.geoW.toolButton_1.clicked.connect(self.ongeoInputFileButtonClicked_1)
        self.geoW.toolButton_2.clicked.connect(self.ongeoInputFileButtonClicked_2)
        self.geoW.toolButton_3.clicked.connect(self.ongeoInputFileButtonClicked_3)
        self.geoW.toolButton_4.clicked.connect(self.ongeoInputFileButtonClicked_4)
        
        #button 'Ok'
        self.geoW.button_box.accepted.connect(self.globalfategeo1_ok)
        # button 'Cancel' and 'Reset'
        self.geoW.button_box.rejected.connect(self.globalfategeo1_cancel)

        
    def loadNoGeoRasters(self):
        self.geoW = globalfateDialog_geo0() #flow_acc + RT
        self.geoW.show()

        self.geoW.toolButton_1.clicked.connect(self.ongeoInputFileButtonClicked_1)
        self.geoW.toolButton_2.clicked.connect(self.ongeoInputFileButtonClicked_2)
        
        #button 'Ok'
        self.geoW.button_box.accepted.connect(self.globalfategeo0_ok)
        # button 'Cancel' and 'Reset'
        self.geoW.button_box.rejected.connect(self.globalfategeo0_cancel)

        
    def globalfate_on(self):
        msgBox = QMessageBox()
        msgBox.setText("GLOBAL-FATE has been launched ")
        msgBox.exec_()
        QApplication.setOverrideCursor(Qt.WaitCursor) # change pointer 
        start = time.time()
        ret=os.system("./GLOBALFATE");
        end = time.time()
        QApplication.restoreOverrideCursor()
            
        msgBox.setText("Elapsed time " + str(end-start) + " s")
        msgBox.exec_()
        if ret > 0:
            rlayer = QgsRasterLayer("files/contaminant_concentration.txt", "contaminant_concentration_result")
            QgsMapLayerRegistry.instance().addMapLayer(rlayer)


   
