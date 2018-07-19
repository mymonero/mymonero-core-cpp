
cmake_minimum_required(VERSION 3.4.1)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(TEST)
enable_testing()

#Find openSSL
find_package(OpenSSL COMPONENTS
	crypto REQUIRED
	ssl REQUIRED
)

#Prep ourselves for compiling boost
find_package(Boost COMPONENTS 
    unit_test_framework REQUIRED
    atomic REQUIRED
    chrono REQUIRED
    context REQUIRED
    date_time REQUIRED
    exception REQUIRED
    filesystem REQUIRED
    graph REQUIRED
    iostreams REQUIRED
    math_c99 REQUIRED
    math_c99f REQUIRED
    math_c99l REQUIRED
    math_tr1 REQUIRED
    math_tr1f REQUIRED
    math_tr1l REQUIRED
    prg_exec_monitor REQUIRED
    program_options REQUIRED
    random REQUIRED
    regex REQUIRED
    serialization REQUIRED
    signals REQUIRED
    system REQUIRED
    test_exec_monitor REQUIRED
    thread REQUIRED
    timer REQUIRED
    wave REQUIRED
    wserialization REQUIRED
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories("src")
include_directories("test")

# keeping test files in a separate source directory 
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/test_*.cpp)

set(
    SRC_FILES
    #
    # src/monero_address_utils.hpp
    # src/monero_address_utils.cpp
    # src/monero_paymentID_utils.hpp
    # src/monero_paymentID_utils.cpp
    # src/monero_wallet_utils.hpp
    # src/monero_wallet_utils.cpp
    src/tools__ret_vals.hpp
    src/tools__ret_vals.cpp
)

#Run through each source
foreach(testSrc ${TEST_SRCS})
    # extract the filename without an extension (NAME_WE)
    get_filename_component(testName ${testSrc} NAME_WE)
    add_executable(
        ${testName}
        ${testSrc}
        ${SRC_FILES}
    )
    target_link_libraries(
        ${testName}
        #
        OpenSSL::SSL
        ${Boost_LIBRARIES} 
    )
    set_target_properties(
        ${testName} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/build/products
    )
    add_test(
        NAME ${testName} 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/products 
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build/products/${testName} --catch_system_error=yes
    )
endforeach(testSrc)
