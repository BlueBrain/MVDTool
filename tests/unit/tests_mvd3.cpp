/*
 * @file tests_mvd2.cpp
 *
 * @brief unit tests for mvd2 parser
 *
 *
 **/


#include <mvd/mvd3.hpp>
#define BOOST_TEST_MAIN mvd3Parser
#include <boost/test/included/unit_test.hpp>



BOOST_AUTO_TEST_CASE( basicNumber )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    BOOST_CHECK_EQUAL(file.getNbNeuron(), 1000);

}


BOOST_AUTO_TEST_CASE( InvalidFile )
{
    using namespace MVD3;

    try{
        // try to open MVD2 as invalid file to verify consistency check
        MVD3File file(MVD2_FILENAME);

        BOOST_FAIL("Should not open with success");
    }catch(...){

    }

}


BOOST_AUTO_TEST_CASE( basicTestPosition )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    Positions neurons_pos = file.getPositions();

}


