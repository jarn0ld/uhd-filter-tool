import numpy as np
import scipy.signal
from matplotlib import pyplot as plt

import uhd_filters

if __name__ == "__main__":
    filters_avail = uhd_filters.load_filters_from_device(8e6, 16e6)

    h_total = None
    for flt_tup in filters_avail:
        flt_name = flt_tup[0]
        flt = flt_tup[1]
        scale = flt.get_tap_full_scale()
        taps = np.array(flt.get_taps()) / scale
        print(f"taps = {taps}")
        dec = int(flt.get_decimation())
        if h_total is None:
            h_total = taps[::dec]
            print(h_total)
            continue
        fig, ax = plt.subplots()
        h_total = np.convolve(h_total, taps, mode="full")
        h_total = h_total[::dec]
        ax.stem(h_total)
        print(h_total)

    w, h = scipy.signal.freqz(h_total)
    fig, ax = plt.subplots(2, 1)
    ax[0].plot(w/(2*np.pi), 20*np.log10(np.abs(h)))
    ax[1].stem(h_total)
    plt.show()
