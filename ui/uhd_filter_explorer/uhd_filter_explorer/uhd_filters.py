from typing import List, Tuple

import numpy as np
import uhd


def load_filters_from_device(
    rate: float,
    mcr: float
) -> List[Tuple[str, uhd.libpyuhd.filters.digital_filter_base_i16]]:

    usrp = uhd.usrp.MultiUSRP("type=b200")
    usrp.set_master_clock_rate(16e6)
    usrp.set_rx_rate(8e6)
    uhd_filter_names = usrp.get_rx_filter_names(0)

    filters = []
    for name in uhd_filter_names:
        uhd_filter = usrp.get_rx_filter(name, 0)
        filters.append((name, uhd_filter))

    filters.sort(key=lambda x: x[1].get_position_index())

    digital_active_filters = [f for f in filters if (
        not f[1].is_bypassed() and isinstance(f[1], uhd.filters.DigitalFilterBaseI16))]

    return digital_active_filters


class FirFilter:
    def __init__(self, decim: int, taps: np.ndarray, is_active=True):
        self.decim = decim
        self.taps = taps
        self.is_active = is_active


FIR_1 = FirFilter(
    2,
    [
        0, 0, 1, 0, -2, 0, 3, 0, -5, 0, 8, 0, -11, 0, 17, 0, -24, 0, 33, 0, -45,
        0, 61, 0, -80, 0, 104, 0, -134, 0, 169, 0, -213, 0, 264, 0, -327, 0, 401, 0, -489, 0, 595, 0, -
        724, 0, 880, 0, -1075, 0, 1323, 0, -1652, 0, 2114, 0, -
        2819, 0, 4056, 0, -6883, 0, 20837, 32767,
        20837, 0, -6883, 0, 4056, 0, -2819, 0, 2114, 0, -1652, 0, 1323, 0, -1075, 0, 880, 0, -724, 0, 595, 0, -489, 0, 401, 0, -327, 0, 264, 0, -
        213, 0, 169, 0, -134, 0, 104, 0, -80, 0, 61, 0, -45, 0, 33, 0, -
        24, 0, 17, 0, -11, 0, 8, 0, -5, 0, 3, 0, -2, 0, 1, 0, 0, 0
    ],
    True)

HB_1 = FirFilter(
    2,
    [
        -8, 0, 42, 0, -147, 0, 619, 1013, 619,
        0, -147, 0, 42, 0, -8
    ],
    True)

HB_2 = FirFilter(2, [-9, 0, 73, 128, 73, 0, -9], True)
HB_3 = FirFilter(2, [1, 4, 6, 4, 1], True)
