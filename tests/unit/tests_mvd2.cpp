/*
 * @file tests_mvd2.cpp
 *
 * @brief unit tests for mvd2 parser
 *
 *
 **/


#include <mvd/mvd2.hpp>
#define BOOST_TEST_MAIN mvd2Parser
#include <boost/test/included/unit_test.hpp>



BOOST_AUTO_TEST_CASE( parser_tests )
{
    using namespace MVD2;

    MVD2File file(MVD2_FILENAME);

    BOOST_CHECK_EQUAL(file.getNbMorpho(), 1000);

}








