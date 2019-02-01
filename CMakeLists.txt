cmake_minimum_required(VERSION 2.8)

set(BOOST_REQUIRED_COMPONENTS
    program_options
    system
)

FIND_PACKAGE(Boost 1.36 COMPONENTS ${BOOST_REQUIRED_COMPONENTS})

find_package(UHD "3.10.0" REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${UHD_INCLUDE_DIRS}
)

add_executable(usrp-filter-explorer usrp-filter-explorer.cpp)
target_link_libraries(usrp-filter-explorer ${UHD_LIBRARIES} ${Boost_LIBRARIES})
