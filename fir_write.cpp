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

static boost::int16_t bandstop_fir_coeffs[]={0,0,0,0,0,0,0,-1,-1,0,1,4,6,9,11,11,10,7,4,0,-2,-3,-2,-1,0,-1,-3,-6,-5,0,13,33,57,80,96,99,87,62,30,0,-19,-25,-18,-6,0,-8,-29,-52,-53,0,137,371,692,1057,1395,1616,1628,1368,813,0,-976,-1976,-2843,-3432,32767,-3432,-2843,-1976,-976,0,813,1368,1628,1616,1395,1057,692,371,137,0,-53,-52,-29,-8,0,-6,-18,-25,-19,0,30,62,87,99,96,80,57,33,13,0,-5,-6,-3,-1,0,-1,-2,-3,-2,0,4,7,10,11,11,9,6,4,1,0,-1,-1,0,0,0,0,0,0};

void write_back(uhd::filter_info_base::sptr filter,std::string path, bool rx, uhd::usrp::multi_usrp::sptr usrp)
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
            uhd::digital_filter_base<boost::int16_t>::sptr dig = boost::dynamic_pointer_cast<uhd::digital_filter_base<boost::int16_t> >(filter);
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
            uhd::digital_filter_fir<boost::int16_t>::sptr fir = boost::dynamic_pointer_cast<uhd::digital_filter_fir<boost::int16_t> >(filter);
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
    double rate = 1e6;
    int N = 1024;
    size_t num;

    //create a usrp device
    const uhd::device_addrs_t device_addrs = uhd::device::find(std::string(""));
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(std::string(""));
    std::cout << usrp->get_pp_string() << std::endl;

    usrp->set_master_clock_rate(32e6);
    usrp->set_rx_rate(rate);

    uhd::tune_request_t tune_request = uhd::tune_request_t(600.1e6, 0);
    usrp->set_rx_freq(tune_request, 0);
    usrp->set_rx_gain(30,"");

    std::map<std::string, uhd::filter_info_base::sptr> filters = usrp->get_rx_filters(1);
    for (std::map<std::string, uhd::filter_info_base::sptr>::iterator it=filters.begin(); it!=filters.end(); ++it) {
        if( it->second->get_type() == uhd::filter_info_base::DIGITAL_FIR_I16) {
            std::cout<<"FIR detected (channel 1):"<<std::endl;
            uhd::digital_filter_fir<boost::int16_t>::sptr fir = boost::dynamic_pointer_cast<uhd::digital_filter_fir<boost::int16_t> >(it->second);
            std::cout<<(*fir)<<std::endl;

            std::vector<boost::int16_t> coeffs;
            coeffs.assign(bandstop_fir_coeffs, bandstop_fir_coeffs + 128);

            fir->set_taps(coeffs);
            usrp->set_filter(it->first, fir);
            std::cout<<"New taps written"<<std::endl;
        }
    }

    filters = usrp->get_rx_filters(0);
    for (std::map<std::string, uhd::filter_info_base::sptr>::iterator it=filters.begin(); it!=filters.end(); ++it) {
        if( it->second->get_type() == uhd::filter_info_base::DIGITAL_FIR_I16) {
            std::cout<<"FIR detected (channel 1):"<<std::endl;
            uhd::digital_filter_fir<boost::int16_t>::sptr fir = boost::dynamic_pointer_cast<uhd::digital_filter_fir<boost::int16_t> >(it->second);
            std::cout<<(*fir)<<std::endl;
        }
    }

    filters = usrp->get_rx_filters(1);
    for (std::map<std::string, uhd::filter_info_base::sptr>::iterator it=filters.begin(); it!=filters.end(); ++it) {
        if( it->second->get_type() == uhd::filter_info_base::DIGITAL_FIR_I16) {
            std::cout<<"FIR detected (channel 2):"<<std::endl;
            uhd::digital_filter_fir<boost::int16_t>::sptr fir = boost::dynamic_pointer_cast<uhd::digital_filter_fir<boost::int16_t> >(it->second);
            std::cout<<(*fir)<<std::endl;
        }
    }

    return 0;
}
