from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication, Qt
from PyQt4.QtGui import QAction, QIcon, QMessageBox, QPushButton, QLineEdit, QFileDialog, QApplication
from globalfate_dialog_geo0 import globalfateDialog_geo0
from globalfate_dialog_geo1 import globalfateDialog_geo1



def geo1_dialog(self):
    self.geoW = globalfateDialog_geo1() #runoff + slope + lakes
    self.geoW.show()

    self.geoW.toolButton_1.clicked.connect(self.ongeoInputFileButtonClicked_1)
    self.geoW.toolButton_2.clicked.connect(self.ongeoInputFileButtonClicked_2)
    self.geoW.toolButton_3.clicked.connect(self.ongeoInputFileButtonClicked_3)
    self.geoW.toolButton_4.clicked.connect(self.ongeoInputFileButtonClicked_4)
        
    # button 'Cancel' and 'Reset'
    self.geoW.button_box.clicked.connect(self.globalfategeo1_cancel)

    #button 'Ok'
    self.geoW.button_box.accepted.connect(self.globalfategeo1_ok)
        

def geo0_dialog(self):
    self.geoW = globalfateDialog_geo0() #flow_acc + RT
    self.geoW.show()

    self.geoW.toolButton_1.clicked.connect(self.ongeoInputFileButtonClicked_1)
    self.geoW.toolButton_2.clicked.connect(self.ongeoInputFileButtonClicked_2)
        
    # button 'Cancel' and 'Reset'
    self.geoW.button_box.clicked.connect(self.globalfategeo0_cancel)

    #button 'Ok'
    self.geoW.button_box.accepted.connect(self.globalfategeo0_ok)

