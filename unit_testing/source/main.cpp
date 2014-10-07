#include <unittest++/UnitTest++.h>
#include <ctime>

#define TEST_ID_NUMBER_MAC 0

#if TEST_ID_NUMBER_MAC == 1
    #include "snapshot_data_stream_operator_test_case.hpp"
#elif TEST_ID_NUMBER_MAC == 2
    #include "retrieve_snapshot_information_test_case.hpp"
#elif TEST_ID_NUMBER_MAC == 3
    #include "filesystem_class_test_cases.hpp"
#endif

namespace
{
}

int main(__attribute__((unused)) int c, __attribute__((unused)) char **v)
{
    srand(std::time(NULL));
    return UnitTest::RunAllTests();
}