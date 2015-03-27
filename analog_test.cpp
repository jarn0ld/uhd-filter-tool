#include <uhd/utils/thread_priority.hpp>
#include <uhd/convert.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/types/filters.hpp>
#include <uhd/types/sensors.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <complex>
#include <string>
#include <map>

void write_back(uhd::filter_info_base::sptr filter, std::string path, bool rx, uhd::usrp::multi_usrp::sptr usrp)
{

    switch(filter->get_type())
    {
        case uhd::filter_info_base::ANALOG_LOW_PASS:
        {
            uhd::analog_filter_lp::sptr lp = boost::dynamic_pointer_cast<uhd::analog_filter_lp>(filter);
            if(rx)
            {
                usrp->set_filter(path, lp);
            } else {
                usrp->set_filter(path, lp);
            }
            break;
        }
        case uhd::filter_info_base::DIGITAL_I16:
        {
            uhd::digital_filter_base<boost::int16_t>::sptr dig = 
                boost::dynamic_pointer_cast<uhd::digital_filter_base<boost::int16_t> >(filter);
            if(rx)
            {
                usrp->set_filter(path, dig);
            } else {
                usrp->set_filter(path, dig);
            }
            break;
        }
        case uhd::filter_info_base::DIGITAL_FIR_I16:
        {
            uhd::digital_filter_fir<boost::int16_t>::sptr fir = 
                boost::dynamic_pointer_cast<uhd::digital_filter_fir<boost::int16_t> >(filter);
            if(rx)
            {
                usrp->set_filter(path, fir);
            } else {
                usrp->set_filter(path, fir);
            }
            break;
        }
        default:
            std::cout<<"unknown filter type"<<std::endl;
            break;
    }
}

int main()
{
    //create a usrp device
    const uhd::device_addrs_t device_addrs = uhd::device::find(std::string(""));
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(std::string(""));
    std::cout << usrp->get_pp_string() << std::endl;

    double rate = 32e6;
    usrp->set_master_clock_rate(32e6);
    usrp->set_rx_rate(rate);

    uhd::tune_request_t tune_request = uhd::tune_request_t(600.1e6, 0);
    usrp->set_rx_freq(tune_request, 0);
    usrp->set_rx_gain(30,"");

    std::map<std::string, uhd::filter_info_base::sptr> filters = usrp->get_rx_filters(0);
    for (std::map<std::string, uhd::filter_info_base::sptr>::iterator it=filters.begin(); it!=filters.end(); ++it) {
        if( it->second->get_type() == uhd::filter_info_base::ANALOG_LOW_PASS) {
            std::cout<<"Analog detected (channel 0):"<<std::endl;
            uhd::analog_filter_lp::sptr lp = boost::dynamic_pointer_cast<uhd::analog_filter_lp>(it->second);
            std::cout<<(it->first)<<std::endl;
            std::cout<<(*lp)<<std::endl;
            std::cout<<"Reprogramming to 60 MHz cutoff"<<std::endl;
            lp->set_cutoff(60e6);
            write_back(lp, it->first, true, usrp);
            break;
        }
    }
   
    std::cout<<"Readback test"<<std::endl; 
    filters = usrp->get_rx_filters(0);
    for (std::map<std::string, uhd::filter_info_base::sptr>::iterator it=filters.begin(); it!=filters.end(); ++it) {
        if( it->second->get_type() == uhd::filter_info_base::ANALOG_LOW_PASS) {
            std::cout<<"Analog detected (channel 0):"<<std::endl;
            uhd::analog_filter_lp::sptr lp = boost::dynamic_pointer_cast<uhd::analog_filter_lp>(it->second);
            std::cout<<(it->first)<<std::endl;
            std::cout<<(*lp)<<std::endl;
        }
    }
    return 0;
}
