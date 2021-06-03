from typing import List, Tuple

import numpy as np
import scipy.signal
import uhd
from matplotlib.backends.backend_qt5agg import FigureCanvas
from matplotlib.backends.backend_qt5agg import \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure
from PySide2 import QtCore, QtWidgets
from PySide2.QtCore import QPoint, QPointF, QRect, QRectF, QSize
from PySide2.QtGui import QColor, QPainter, QPen
from PySide2.QtWidgets import (QGraphicsAnchor, QGraphicsItem, QGraphicsScene,
                               QGraphicsView, QGridLayout, QHBoxLayout, QLabel,
                               QMainWindow, QVBoxLayout, QWidget)


class Splitter(QGraphicsItem):
    def __init__(self):
        super().__init__()

    def boundingRect(self):
        return QRectF(0, 0, 100, 100)

    def paint(self, painter, option, z):
        p = painter
        w = 100
        h = 100
        p.setPen(QColor('red'))
        p.drawLine(QPoint(0, h/2), QPoint(w, h/2))


class Combiner(QGraphicsItem):
    def __init__(self):
        super().__init__()

    def boundingRect(self):
        return QRectF(0, 0, 100, 300)

    def paint(self, painter, option, x):
        p = painter
        w = 100
        h = 300
        h_16 = h/6.0
        h_36 = 3.0 * h_16
        h_56 = 5.0 * h_16
        p.setPen(QColor('red'))
        p.drawLine(QPoint(0, h_16), QPoint(w/2, h_16))
        p.drawLine(QPoint(0, h_56), QPoint(w/2, h_56))
        p.drawLine(QPoint(w/2, h_16), QPoint(w/2, h_56))
        p.drawLine(QPoint(w/2, h_36), QPoint(w, h_36))


class StraightConnector(QGraphicsItem):
    def __init__(self):
        super().__init__()

    def boundingRect(self):
        return QRectF(0, 0, 100, 100)

    def paint(self, painter, option, x):
        p = painter
        w = 100
        h = 100
        p.setPen(QColor('red'))
        p.drawLine(QPoint(0, h/2), QPoint(w, h/2))


class LowPassFlt(QGraphicsItem):
    def __init__(self):
        super().__init__()

    def boundingRect(self):
        return QRectF(0, 0, 100, 100)

    def paint(self, painter, option, x):
        p = painter
        w = 100
        h = 100
        p.setPen(QColor('red'))
        p.drawLine(QPoint(0, h/2), QPoint(10, h/2))
        p.drawLine(QPoint(w-10, h/2), QPoint(w, h/2))
        p.drawRect(10, 10, w-20, h-20)
        p.drawText(QRect(0, 0, w, h), QtCore.Qt.AlignVCenter |
                   QtCore.Qt.AlignHCenter, "LPF")


class LowPassFltWBypass(QGraphicsItem):
    def __init__(self, name, w=100, h=100):
        super().__init__()
        self._name = name
        self.w = w
        self.h = h

    def boundingRect(self):
        return QRectF(0, 0, self.w, self.h)

    def paint(self, painter, option, x):
        p = painter
        w = self.w
        h = self.h
        p.setPen(QColor('red'))
        p.drawLine(QPoint(0, h/2), QPoint(20, h/2))
        p.drawLine(QPoint(w-20, h/2), QPoint(w, h/2))
        p.drawRect(20, 20, w-40, h-40)
        p.drawText(QRect(0, 0, w, h), QtCore.Qt.AlignVCenter |
                   QtCore.Qt.AlignHCenter, self._name)

        p.drawLine(QPoint(10, h/2), QPoint(10, 10))
        p.drawLine(QPoint(10, 10), QPoint(w-10, 10))
        p.drawLine(QPoint(w-10, h/2), QPoint(w-10, 10))


class FilterChain(QGraphicsView):

    def __init__(self, filters: List[Tuple[str, uhd.libpyuhd.filters.digital_filter_base_i16]]):
        super().__init__()

        self._scene = QGraphicsScene()

        x = 0
        for flt in filters:
            lpf = LowPassFltWBypass(flt[0], 200, 150)
            lpf.setPos(x, 0)
            x += lpf.w
            self._scene.addItem(lpf)

        self.setScene(self._scene)
        self.setAlignment(
            QtCore.Qt.AlignTop | QtCore.Qt.AlignLeft)


class MainWidget(QWidget):
    def __init__(self, filters: List[Tuple[str, uhd.libpyuhd.filters.digital_filter_base_i16]]):
        super().__init__()

        self._layout = QVBoxLayout()
        self._filter_chain = FilterChain(filters)
        self._layout.addWidget(self._filter_chain)

        self._mpl_canvas_2 = FigureCanvas(Figure(figsize=(10, 10)))
        self._mpl_canvas = FigureCanvas(Figure(figsize=(10, 10)))

        self._axs = self._mpl_canvas.figure.subplots(5, 2)
        self._axs_2 = self._mpl_canvas_2.figure.subplots()
        decim_accu = 1
        for i, flt in enumerate(filters):
            decim = flt[1].get_decimation()
            print(decim)
            scale = flt[1].get_tap_full_scale()

            taps = np.array(flt[1].get_taps()) / scale
            self._axs[i][0].stem(taps)
            self._axs[i][0].grid()

            w, h = scipy.signal.freqz(taps)
            self._axs[i][1].plot(w/(2*np.pi),
                                 20*np.log10(np.abs(h)))
            self._axs[i][1].grid()

            self._axs_2.plot(w/(2*np.pi) * decim_accu, 20*np.log10(np.abs(h)))
            self._axs_2.grid()

            decim_accu /= decim

        self._axs_2.grid()
        self._axs_2.set_ylim(-60, 10)
        self._mpl_canvas.figure.tight_layout()
        self._layout.addWidget(self._mpl_canvas)
        self._layout.addWidget(self._mpl_canvas_2)
        self.setLayout(self._layout)


class MyMainWindow(QMainWindow):

    def __init__(self, filters: List[Tuple[str, uhd.libpyuhd.filters.digital_filter_base_i16]]):
        super(MyMainWindow, self).__init__()

        self.setWindowTitle("UHD Filter Explorer")
        # self.setGeometry(800, 600, 1000, 300)
        self.central_widget = MainWidget(filters)
        self.addToolBar(NavigationToolbar(
            self.central_widget._mpl_canvas_2, self))
        self.setCentralWidget(self.central_widget)
