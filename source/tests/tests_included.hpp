#if UNIT_TEST_PROG == true
#ifndef TESTS_TESTS_INCLUDED_HPP_INCLUDED
#define TESTS_TESTS_INCLUDED_HPP_INCLUDED
#include <unittest++/UnitTest++.h>

#include "test_cases/retrieve_snapshot_information_test_case.hpp"
#include "test_cases/snapshot_data_stream_operator_test_case.hpp"
#include "test_cases/test_setting_streams.hpp"

namespace
{
    inline int run_tests()
    {
        return UnitTest::RunAllTests();
    }
}

#endif
#endif