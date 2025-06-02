from PyQt5.QtWidgets import QComboBox as QtQComboBox
from PyQt5.QtCore import pyqtSignal

class QComboBox(QtQComboBox):
    clicked = pyqtSignal()

    def showPopup(self):
        self.clicked.emit()
        super(QComboBox, self).showPopup()