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
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication
from PyQt4.QtGui import QAction, QIcon, QMessageBox, QPushButton, QLineEdit, QFileDialog
# Initialize Qt resources from file resources.py
import resources
# Import the code for the dialog
from globalfate_dialog import globalfateDialog
import os.path
from aux_functions import check_filename
import platform
from qgis.core import *


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
        # TODO: We are going to let the user set this up in a future iteration
        self.toolbar = self.iface.addToolBar(u'globalfate')
        self.toolbar.setObjectName(u'globalfate')
        
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

        # Set rasters names
        self.rasters = ["Hierarchical order", "Flow direction", "Runoff", "Slope", "Lakes index", "Lakes volume", "Contaminant consumption", "Human population", "Waste water rate to treatment plant"]

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
        self.dlg.toolButton_7.clicked.connect(self.onInputFileButtonClicked_7)
        self.dlg.toolButton_8.clicked.connect(self.onInputFileButtonClicked_8)
        self.dlg.toolButton_9.clicked.connect(self.onInputFileButtonClicked_9)
        self.dlg.toolButton_10.clicked.connect(self.onInputFileButtonClicked_10)

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
            
    def onInputFileButtonClicked_7(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_7.clear()
            self.dlg.lineEdit_7.setText(filename)
            
    def onInputFileButtonClicked_8(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_8.clear()
            self.dlg.lineEdit_8.setText(filename)

    def onInputFileButtonClicked_9(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_9.clear()
            self.dlg.lineEdit_9.setText(filename)
    
    def onInputFileButtonClicked_10(self):
        filename = QFileDialog.getOpenFileName()
        if filename:
            self.dlg.lineEdit_10.clear()
            self.dlg.lineEdit_10.setText(filename)
            
    # function to call from button 'Run'
    def globalfate_run(self):
        current_os = platform.system()
        filename1 = self.dlg.lineEdit_1.text()
        filename2 = self.dlg.lineEdit_2.text()
        filename3 = self.dlg.lineEdit_3.text()
        filename4 = self.dlg.lineEdit_4.text()
        filename5 = self.dlg.lineEdit_5.text()
        filename6 = self.dlg.lineEdit_6.text()
        filename7 = self.dlg.lineEdit_7.text()
        filename8 = self.dlg.lineEdit_8.text()
        filename9 = self.dlg.lineEdit_9.text()
        filename10= self.dlg.lineEdit_10.text()
        par_k = self.dlg.lineEdit_11.text()
        par_g = self.dlg.lineEdit_12.text()
        par_w1 = self.dlg.lineEdit_13.text()
        par_w2 = self.dlg.lineEdit_14.text()
        if self.dlg.checkBox_1.isChecked(): geo = 0
        else: geo = 1
        if self.dlg.checkBox_2.isChecked(): concentration = 0
        else: concentration = 1
        if self.dlg.checkBox_3.isChecked(): partial = 1
        else: partial = 0
        
        filenames = [filename1, filename2, filename3, filename4, filename5, filename6, filename7, filename8, filename9, filename10]
        parnames = [7.2, 0.5, 0.27, 0.3, 0.044, par_g, par_k, par_w1, par_w2, partial, geo, concentration]
        parameters = ["a", "b", "c", "d", "n", "excretion rate", "k", "w1", "w2", "save partial", "geo", "concentration"]
        if(current_os=='Windows'): file_rasters = open('C:/tmp/input_rasters.txt', 'w')
        if(current_os!='Windows'): file_rasters = open('/tmp/input_rasters.txt', 'w')
        
        a=0
        #for i in range(len(self.rasters)):
        #    if check_filename(file_rasters, self.rasters[i], filenames[i]) is -1: 
        #        break
        #    else: 
        #        a+=1
        a=9
        
        if(current_os=='Windows'): file_params = open('C:/tmp/input_parameters.txt', 'w')
        if(current_os!='Windows'): file_params = open('/tmp/input_parameters.txt', 'w')
        
        if a == len(self.rasters):
            for i in range(len(parameters)):
                if check_filename(file_params, parameters[i], parnames[i], p=1) is -1:
                    break
                else:
                    a+=1

        if a == (len(self.rasters)+len(parameters)):
            if filename10: file_rasters.write(filename10)
            file_rasters.close()
            file_params.close()
        
        if a == (len(self.rasters)+len(parameters)):
            os.system("./GLOBALFATE");
            msgBox = QMessageBox()
            msgBox.setText("GLOBAL-FATE has been launched")
            msgBox.exec_()

        rlayer = QgsRasterLayer("C:/Users/cfont/Documents/projectes/contaminant_C_2treat_mask.tif", "capa")
        QgsMapLayerRegistry.instance().addMapLayer(rlayer)

        ret=1
        #ret=os.system("./GLOBALFATE")
        ret=ret/256
        msgBox = QMessageBox()
        msgBox.setText(str(ret))
        msgBox.exec_()
        
        #start = time.time()
        #os.system("./GLOBALFATE")
        #end = time.time()
        #print(end-start)

        # call global-fate
        # https://csl.name/post/c-functions-python/
        # https://docs.python.org/2/extending/extending.html
        # import subprocess
        # subprocess.check_call([r"C:\pathToYourProgram\yourProgram.exe", "your", "arguments", "comma", "separated"])

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
        self.dlg.lineEdit_11.clear()
        self.dlg.lineEdit_12.clear()
        self.dlg.lineEdit_13.clear()
        self.dlg.lineEdit_14.clear()
        self.dlg.checkBox_1.setChecked(False)
        self.dlg.checkBox_2.setChecked(False)
        self.dlg.checkBox_3.setChecked(False)
        self.dlg.checkBox_4.setChecked(False)


   
