#!/usr/bin/python3
import uhd

if __name__ == "__main__":
    usrp = uhd.usrp.MultiUSRP("type=b200")
    usrp.set_master_clock_rate(16e6)
    usrp.set_rx_rate(8e6)
    uhd_filter_names = usrp.get_rx_filter_names(0)
    filters = []
    for name in uhd_filter_names:
        uhd_filter = usrp.get_rx_filter(name, 0)
        filters.append((name, uhd_filter))

    filters.sort(key=lambda x: x[1].get_position_index())

    analog_filters = [f for f in filters if (
        isinstance(f[1], uhd.filters.AnalogFilterLP))]
    digital_active_filters = [f for f in filters if (
        not f[1].is_bypassed() and isinstance(f[1], uhd.filters.DigitalFilterBaseI16))]

    for f in analog_filters:
        print(f[1])

    for f in digital_active_filters:
        print(type(f[1]))
