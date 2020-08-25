#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <uhd/types/filters.hpp>
#include <uhd/usrp/multi_usrp.hpp>

namespace po = boost::program_options;

void write_line_plt(const std::vector<boost::int16_t> &data);

int main(int argc, char **argv) {
  int mode = 0;
  double set_bw = 0;
  double rate = 0;
  std::string filter_path = "";

  /*
     double rate = 32e6;
     usrp->set_master_clock_rate(32e6);
     usrp->set_rx_rate(rate);

     uhd::tune_request_t tune_request = uhd::tune_request_t(600.1e6, 0);
     usrp->set_rx_freq(tune_request, 0);
     usrp->set_rx_gain(30,"");
     */

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "rate", po::value<double>()->default_value(32.0e6),
      "[rate] master clock rate")(
      "mask", po::value<std::string>()->default_value("rx_frontends/A/"),
      "[mask] mask to use for filter search")("show", po::value<std::string>(),
                                              "[path] show filter details");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  rate = vm["rate"].as<double>();
  std::string search_mask = vm["mask"].as<std::string>();

  // create a usrp device
  uhd::usrp::multi_usrp::sptr usrp;
  try {
    usrp = uhd::usrp::multi_usrp::make(std::string(""));
  } catch (...) {
    std::cout << "ERROR: No USRP found" << std::endl;
    return -1;
  }

  usrp->set_master_clock_rate(rate);

  std::cout << std::endl << "Using master clock rate: " << rate << std::endl;
  std::cout << "Using search mask: " << search_mask << std::endl;

  std::cout << std::endl
            << "Found the following filters matching \"" << search_mask
            << "\": " << std::endl;

  std::vector<std::string> filter_names = usrp->get_filter_names(search_mask);

  try {
    for (int i = 0; i < filter_names.size(); i++) {
      std::cout << filter_names[i] << std::endl;
      // uhd::filter_info_base::sptr filter = usrp->get_filter(filter_names[i]);
      // std::cout<<*filter<<std::endl;
    }
  } catch (...) {
  }

  std::cout << std::endl << "Active filters: " << std::endl;
  try {
    for (int i = 0; i < filter_names.size(); i++) {
      uhd::filter_info_base::sptr filter = usrp->get_filter(filter_names[i]);
      if (not filter->is_bypassed()) {
        std::cout << filter_names[i] << std::endl;
        // std::cout<<*filter<<std::endl;
      }
    }
  } catch (...) {
  }

  if (vm.count("show")) {
    std::cout << std::endl;
    filter_path = vm["show"].as<std::string>();
    std::cout << "Reading Path: " << filter_path << std::endl;
    uhd::filter_info_base::sptr filter = usrp->get_filter(filter_path);
    std::cout << *filter << std::endl;
  } else {
    std::cout << std::endl << "Available Info on Active Filters: " << std::endl;
    try {
      for (int i = 0; i < filter_names.size(); i++) {
        uhd::filter_info_base::sptr filter = usrp->get_filter(filter_names[i]);
        if (not filter->is_bypassed()) {
          std::cout << filter_names[i] << std::endl;
          std::cout << *filter << std::endl;
          if (filter->get_type() == uhd::filter_info_base::DIGITAL_I16) {
          }
          if (filter->get_type() == uhd::filter_info_base::DIGITAL_FIR_I16) {
            uhd::digital_filter_fir<boost::int16_t>::sptr fir =
                boost::dynamic_pointer_cast<
                    uhd::digital_filter_fir<boost::int16_t>>(filter);
            auto &taps = fir->get_taps();
            write_line_plt(taps);
            std::cout << "fir taps: " << std::endl;
            for (boost::int16_t tap : taps) {
              std::cout << tap << ", ";
            }
          }
        }
      }
    } catch (...) {
    }
  }

  //
  // change low-pass cut-off
  //
  // uhd::filter_info_base::sptr filter = usrp->get_filter(filter_path);
  // uhd::analog_filter_lp::sptr analog_filter =
  //  boost::dynamic_pointer_cast<uhd::analog_filter_lp>(filter);
  // double my_new_cutoff = 1e6;
  // analog_filter->set_cutoff(my_new_cutoff);
  // usrp->set_filter(filter_path, filter);

  //
  // write FIR coefficients
  //
  // static boost::int16_t bandstop_fir_coeffs[] = {
  //  0,0,0,0,0,0,0,-1,-1,0,1,4,6,9,11,11,10,7,4,0,-2,-3,-2,-1,0,-1,-3,-6,-5,0,13,33,57,80,
  //  96,99,87,62,30,0,-19,-25,-18,-6,0,-8,-29,-52,-53,0,137,371,692,1057,1395,1616,1628,
  //  1368,813,0,-976,-1976,-2843,-3432,32767,-3432,-2843,-1976,-976,0,813,1368,1628,1616,
  //  1395,1057,692,371,137,0,-53,-52,-29,-8,0,-6,-18,-25,-19,0,30,62,87,99,96,80,57,33,13,
  //  0,-5,-6,-3,-1,0,-1,-2,-3,-2,0,4,7,10,11,11,9,6,4,1,0,-1,-1,0,0,0,0,0,0
  //};
  //
  // uhd::filter_info_base::sptr filter = usrp->get_filter(filter_path);
  // uhd::digital_filter_fir<boost::int16_t>::sptr fir_filter =
  //  boost::dynamic_pointer_cast<uhd::digital_filter_fir<boost::int16_t>
  //  >(filter);
  // std::vector<boost::int16_t> coeff_vec;
  // coeff_vec.assign(bandstop_fir_coeffs, bandstop_fir_coeffs+128);
  // fir_filter->set_taps(coeff_vec);
  // usrp->set_filter(filter_path, filter);

  return 0;
}

void write_rx_fir_i16(uhd::usrp::multi_usrp::sptr usrp,
                      std::vector<boost::int16_t> coeffs,
                      uhd::digital_filter_fir<boost::int16_t>::sptr filter,
                      std::string path) {
  filter->set_taps(coeffs);
  usrp->set_filter(path, filter);
}

void write_back_rx_filter(uhd::usrp::multi_usrp::sptr usrp,
                          uhd::filter_info_base::sptr filter,
                          std::string path) {

  switch (filter->get_type()) {
  case uhd::filter_info_base::ANALOG_LOW_PASS: {
    uhd::analog_filter_lp::sptr lp =
        boost::dynamic_pointer_cast<uhd::analog_filter_lp>(filter);

    usrp->set_filter(path, lp);
    break;
  }
  case uhd::filter_info_base::DIGITAL_I16: {
    uhd::digital_filter_base<boost::int16_t>::sptr dig =
        boost::dynamic_pointer_cast<uhd::digital_filter_base<boost::int16_t>>(
            filter);

    usrp->set_filter(path, dig);
    break;
  }
  case uhd::filter_info_base::DIGITAL_FIR_I16: {
    uhd::digital_filter_fir<boost::int16_t>::sptr fir =
        boost::dynamic_pointer_cast<uhd::digital_filter_fir<boost::int16_t>>(
            filter);

    usrp->set_filter(path, fir);
    break;
  }
  default:
    std::cout << "unknown filter type" << std::endl;
    break;
  }
}

void write_line_plt(const std::vector<boost::int16_t> &data) {
  std::ofstream plt_file;
  plt_file.open("plot.py");
  plt_file << "import numpy as np" << std::endl;
  plt_file << "from scipy import signal" << std::endl;
  plt_file << "from matplotlib import pyplot as plt" << std::endl;
  plt_file << "data = [";
  for (boost::int16_t elem : data) {
    plt_file << elem << ", ";
  }
  plt_file << "]" << std::endl;
  plt_file << "w, h = signal.freqz(data)" << std::endl;
  plt_file << "plt.plot(w, 20 * np.log10(abs(h)))" << std::endl;
  plt_file << "plt.plot(w, np.unwrap(np.angle(h)))" << std::endl
           << "plt.show()" << std::endl;
  plt_file.close();
}
