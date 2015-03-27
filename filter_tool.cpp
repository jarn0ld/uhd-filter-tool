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

namespace po = boost::program_options;

/*
 * This program demonstrates basic use cases of the uhd filter API.
 * It shows how to list available filter of a system and how to show information
 * about those filters.
 * If you want to write filters you can do so as follows (FIR example):
 *
 * boost::int16_t fir_taps[]={......}; //len 128 vector
 * uhd::filter_info_base::sptr filter = usrp->get_filter(filter_path);
 * uhd::digital_filter_fir<boost::int16_t>::sptr fir_filter = boost::dynamic_pointer_cast<uhd::digital_filter_fir<boost::int16_t> >(filter);
 * std::vector<boost::int16_t> taps_vect;
 * taps_vect.assign(fir_taps, fir_taps+128);
 * fir_filter->set_taps(taps_vect);
 * usrp->set_filter(filter_path, filter);
 */
int main(int argc, char** argv)
{
    int mode = 0;
    double rate = 0;
    std::string search_mask = "";
    std::string filter_path = "";

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("rate", po::value<double>()->default_value(32.0e6), "master clock rate")
        ("list", "list the available filter names. Use --mask to list only certain filters")
        ("print","print filter information. Use --path to only print one filter. Use --mask to print a group of filters")
        ("path", po::value<std::string>(), "specify a particular filter")
        ("mask", po::value<std::string>()->default_value(""), "a search mask");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    rate = vm["rate"].as<double>();
    search_mask = vm["mask"].as<std::string>();

    if (vm.count("list")) {
        mode = 0;
    }

    if (vm.count("print")) {
        if(vm.count("path")) {
            mode = 2;
            filter_path = vm["path"].as<std::string>();
        } else {
            mode = 1;
        }
    }

    //create a usrp device
    const uhd::device_addrs_t device_addrs = uhd::device::find(std::string(""));
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(std::string(""));

    usrp->set_master_clock_rate(rate);

    std::cout<<std::endl;
    std::cout<<"##############################################################################"<<std::endl;
    std::cout<<"############################## Filter API Tool ###############################"<<std::endl;
    std::cout<<"##############################################################################"<<std::endl;
    std::cout<<std::endl;

    if(mode == 0) {
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

    if(mode == 1) {
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

    if(mode == 2) {
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<"############################## Filter Print ##################################"<<std::endl;
        std::cout<<"##############################################################################"<<std::endl;
        std::cout<<std::endl;

        try {
            std::cout<<filter_path<<std::endl;
            uhd::filter_info_base::sptr filter = usrp->get_filter(filter_path);
            std::cout<<*filter<<std::endl;
        }catch(...) {

        }
    }

    std::cout<<std::endl;
    return 0;
}
