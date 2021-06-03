import numpy as np
import uhd
from PySide2.QtWidgets import QApplication

from main_window import MyMainWindow
from uhd_filters import load_filters_from_device

if __name__ == "__main__":
    qapp = QApplication()

    filters = load_filters_from_device(8e6, 16e6)
    w = MyMainWindow(filters)
    w.show()

    qapp.exec_()
