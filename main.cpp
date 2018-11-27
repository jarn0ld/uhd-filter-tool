#include <uhd/utils/thread.hpp>
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
#include <math.h>
#include <fftw3.h>

static boost::int16_t bandstop_fir_coeffs[]={0,0,0,0,0,0,0,-1,-1,0,1,4,6,9,11,11,10,7,4,0,-2,-3,-2,-1,0,-1,-3,-6,-5,0,13,33,57,80,96,99,87,62,30,0,-19,-25,-18,-6,0,-8,-29,-52,-53,0,137,371,692,1057,1395,1616,1628,1368,813,0,-976,-1976,-2843,-3432,32767,-3432,-2843,-1976,-976,0,813,1368,1628,1616,1395,1057,692,371,137,0,-53,-52,-29,-8,0,-6,-18,-25,-19,0,30,62,87,99,96,80,57,33,13,0,-5,-6,-3,-1,0,-1,-2,-3,-2,0,4,7,10,11,11,9,6,4,1,0,-1,-1,0,0,0,0,0,0};
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    int mode = 0;
    double set_bw = 0;
    double rate = 0;
    std::string search_mask = "";
    std::string filter_path = "";

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("list", "list all filters")
        ("print", "print filter")
        ("path", po::value<std::string>(), "filter path")
        ("rate", po::value<double>()->default_value(32.0e6) ,"master clock rate")
        ("set", "set filter bw")
        ("write_fir", "write fir filter");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    rate = vm["rate"].as<double>();

    if(vm.count("mask")) {
        search_mask = vm["mask"].as<std::string>();
    }

    if (vm.count("list")) {
        std::cout << "List mode selected" << std::endl;
        mode = 1;
    }

    if (vm.count("print")) {
        std::cout << "Print mode selected" << std::endl;
        if(vm.count("path")) {
            mode = 2;
            filter_path = vm["path"].as<std::string>();
        } else {
            mode = 0;
        }
    }

    if (vm.count("set")) {
        std::cout << "Set BW mode selected" << std::endl;
        mode = 3;

        if(vm.count("path"))
        {
            filter_path = vm["path"].as<std::string>();
        }
        if(vm.count("bw"))
        {
            set_bw = vm["bw"].as<double>();
        }
    }

    if (vm.count("write_fir")) {
        std::cout << "Write fir mode selected" << std::endl;
        mode = 4;

        if(vm.count("path"))
        {
            std::cout << "path: " << std::endl;
            filter_path = vm["path"].as<std::string>();
            std::cout << "path: " <<filter_path<< std::endl;
        }
    }

    //create a usrp device
    const uhd::device_addrs_t device_addrs = uhd::device::find(std::string(""));
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(std::string(""));

    usrp->set_master_clock_rate(rate); //this will lead to 128 tap Rx fir and 64 tap Tx fir

    std::cout<<std::endl;
    std::cout<<"##############################################################################"<<std::endl;
    std::cout<<"############################## Filter API Tool ###############################"<<std::endl;
    std::cout<<"##############################################################################"<<std::endl;
    std::cout<<std::endl;

    if(mode == 0) {
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<"############################## Filter Print All ##############################"<<std::endl;
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<std::endl;

        std::vector<std::string> filter_names = usrp->get_filter_names(search_mask);

        try {
            for(int i = 0;i < filter_names.size(); i++) {
                std::cout<<filter_names[i]<<std::endl;
                uhd::filter_info_base::sptr filter = usrp->get_filter(filter_names[i]);
                std::cout<<*filter<<std::endl;
            }
        }catch(...) {

        }
    }

    if(mode == 1) {
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<"############################## Filter List ###################################"<<std::endl;
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<std::endl;

        std::vector<std::string> filter_names = usrp->get_filter_names(search_mask);

        try {
            for(int i = 0;i < filter_names.size(); i++) {
                std::cout<<filter_names[i]<<std::endl;
            }
        }catch(...) {

        }
    }

    if(mode == 2) {
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<"############################## Filter Print ##################################"<<std::endl;
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<std::endl;

        uhd::filter_info_base::sptr filter = usrp->get_filter(filter_path);
        std::cout<<*filter<<std::endl;
    }

    if(mode == 3) {
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<"############################## Filter Set ####################################"<<std::endl;
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<std::endl;

        uhd::filter_info_base::sptr filter = usrp->get_filter(filter_path);
        std::cout<<*filter<<std::endl;

        uhd::analog_filter_lp::sptr analog_filter = boost::dynamic_pointer_cast<uhd::analog_filter_lp>(filter);
        analog_filter->set_cutoff(set_bw);
        usrp->set_filter(filter_path, filter);

        filter = usrp->get_filter(filter_path);
        std::cout<<*filter<<std::endl;
    }

    if(mode == 4) {
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<"############################## Update FIR Filter #############################"<<std::endl;
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<std::endl;
        std::cout<<filter_path<<std::endl;

        uhd::filter_info_base::sptr filter = usrp->get_filter(filter_path);
        std::cout<<*filter<<std::endl;

        uhd::digital_filter_fir<boost::int16_t>::sptr fir_filter =
          boost::dynamic_pointer_cast<
            uhd::digital_filter_fir<boost::int16_t> >(filter);

        std::vector<boost::int16_t> taps_vect;

        taps_vect.assign(bandstop_fir_coeffs, bandstop_fir_coeffs+128);

        fir_filter->set_taps(taps_vect);

        usrp->set_filter(filter_path, filter);

        filter = usrp->get_filter(filter_path);

        std::cout<<*filter<<std::endl;
    }

    std::cout<<std::endl;
    return 0;
}
