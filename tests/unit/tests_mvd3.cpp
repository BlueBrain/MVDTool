/*
 * Copyright (C) 2015 Adrien Devresse <adrien.devresse@epfl.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#include <mvd/mvd3.hpp>
#define BOOST_TEST_MAIN mvd3Parser
#include <boost/test/included/unit_test.hpp>



BOOST_AUTO_TEST_CASE( basicNumber )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    BOOST_CHECK_EQUAL(file.getNbNeuron(), 1000);
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

    BOOST_CHECK_EQUAL(neurons_pos.shape()[0], 1000);
    BOOST_CHECK_EQUAL(neurons_pos[0][0], static_cast<double>(40.821401));
    BOOST_CHECK_EQUAL(neurons_pos[0][1], static_cast<double>(1986.506637));
    BOOST_CHECK_EQUAL(neurons_pos[0][2], static_cast<double>(10.788424));

}





BOOST_AUTO_TEST_CASE( basicTestRotations )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    Rotations neurons_rot = file.getRotations();

    BOOST_CHECK_EQUAL(neurons_rot.shape()[0], 1000);
    BOOST_CHECK_EQUAL(neurons_rot[0][0], static_cast<double>(0));
    BOOST_CHECK_EQUAL(neurons_rot[0][1], static_cast<double>(-1.146572));
    BOOST_CHECK_EQUAL(neurons_rot[0][2], static_cast<double>(0));
    BOOST_CHECK_EQUAL(neurons_rot[0][3], static_cast<double>(0));

}


BOOST_AUTO_TEST_CASE( basicTestMorphologies )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> morpho = file.getMorphologies();

    BOOST_CHECK_EQUAL(morpho[0], "sm090227a1-2_idC");
    BOOST_CHECK_EQUAL(morpho[20], "dend-C280998A-P3_axon-sm110131a1-3_INT_idA");
    BOOST_CHECK_EQUAL(morpho[21], "dend-rr110114C1_idA_axon-sm110131a1-3_INT_idA");

}



BOOST_AUTO_TEST_CASE( basicTestEtype )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> morpho = file.getEtypes();

    BOOST_CHECK_EQUAL(morpho[0], "cACint");
    BOOST_CHECK_EQUAL(morpho[10], "cACint");
    BOOST_CHECK_EQUAL(morpho[20], "cADpyr");

}


BOOST_AUTO_TEST_CASE( basicTestMypes )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> morpho = file.getMtypes();

    BOOST_CHECK_EQUAL(morpho[0], "L1_SLAC");
    BOOST_CHECK_EQUAL(morpho[10], "L1_SLAC");
    BOOST_CHECK_EQUAL(morpho[20], "L23_PC");

}



BOOST_AUTO_TEST_CASE( basicTestSynClass )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> morpho = file.getSynapseClass();

    BOOST_CHECK_EQUAL(morpho[0], "INH");
    BOOST_CHECK_EQUAL(morpho[10], "INH");
    BOOST_CHECK_EQUAL(morpho[20], "EXC");

}
