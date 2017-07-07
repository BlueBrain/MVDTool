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
#include <mvd/mvd_base.hpp>
#include <mvd/mvd2.hpp>
#define BOOST_TEST_MAIN mvd2Parser
#include <boost/test/included/unit_test.hpp>



BOOST_AUTO_TEST_CASE( parser_tests )
{
    using namespace MVD2;

    MVD2File file(MVD2_FILENAME);

    BOOST_CHECK_EQUAL(file.getNbNeuron(), 1000);
    BOOST_CHECK_EQUAL(file.getNbMorphoType(), 9);
    BOOST_CHECK_EQUAL(file.getNbMorpho(), 52);
    BOOST_CHECK_EQUAL(file.getNbColumns(), 10);
}




struct MVD2Checker{
    MVD2Checker():
        neuron_counter(0),
        morph_array(0){

    }

    int operator ()(MVD2::DataSet type, const char* line){
        using namespace MVD2;
        switch(type){
            case(NeuronLoaded):{
                std::string name;
                int database, column, minicolumn, layer, morph, elec;
                std::vector<float> xyzr;
                std::string metype;

                parseNeuronLine(line, name, database, column, minicolumn, layer, morph, elec, xyzr, metype);
                BOOST_CHECK(layer <= 6 && layer >= 0);

                if(neuron_counter ==0){ // let's check neuron 1 consistency
                    float coords[] = { 40.821401, 1986.506637, 10.788424, -1.146572};
                    BOOST_CHECK_EQUAL(name, "sm090227a1-2_idC");
                    BOOST_CHECK_EQUAL(metype, "cACint2090_L1_SLAC_1_sm090227a1-2_idC");
                    BOOST_CHECK_EQUAL_COLLECTIONS(xyzr.begin(), xyzr.end(), coords, coords +4);
                    BOOST_CHECK_EQUAL(database, 0);
                    BOOST_CHECK_EQUAL(column, 0);
                    BOOST_CHECK_EQUAL(minicolumn, 1);
                    BOOST_CHECK_EQUAL(layer, 0);
                    BOOST_CHECK_EQUAL(morph, 0);
                    BOOST_CHECK_EQUAL(elec, 0);

                }

                if(neuron_counter ==146){
                    float coords[] = { 26.931492, 1771.498694, 84.290181, -125.718090};
                    BOOST_CHECK_EQUAL(name, "sm101103b1-2_INT_idC");
                    BOOST_CHECK_EQUAL(metype, "cACint2090_L23_MC_2_sm101103b1-2_INT_idC");
                    BOOST_CHECK_EQUAL_COLLECTIONS(xyzr.begin(), xyzr.end(), coords, coords +4);
                    BOOST_CHECK_EQUAL(database, 0);
                    BOOST_CHECK_EQUAL(column, 0);
                    BOOST_CHECK_EQUAL(minicolumn, 7);
                    BOOST_CHECK_EQUAL(layer, 1);
                    BOOST_CHECK_EQUAL(morph, 2);
                    BOOST_CHECK_EQUAL(elec, 0);


                }
                neuron_counter +=1;
                break;
            }
            case(CircuitSeeds):{
                double s1, s2, s3;
                parseSeedInitLine(line, s1, s2, s3);
                BOOST_CHECK_EQUAL(s1, 837632.000000);
                BOOST_CHECK_EQUAL(s2, 2906729.000000);
                BOOST_CHECK_EQUAL(s3, 4236279.000000);
                break;
            }
            case(MorphTypes):{
                std::string name1, name2, name3;
                parseMorphTypeLine(line, name1, name2, name3);
                if(morph_array ==0){
                    BOOST_CHECK_EQUAL(name1, "L1_SLAC");
                    BOOST_CHECK_EQUAL(name2, "INT");
                    BOOST_CHECK_EQUAL(name3, "INH");
                }
                morph_array +=1;
                break;
            }
            case(ElectroTypes):{
                std::string str;
                parseElectroTypeLine(line, str);
                BOOST_CHECK( str.compare("cACint") ==0 || str.compare("cADpyr") ==0);
                // TEST for
                BOOST_CHECK( str.compare( boost::trim_copy(str)) == 0);
                break;
            }
            default:{

            }
        }
        return 0;
    }

    size_t neuron_counter;
    size_t morph_array;
};


// test extracted from functionalizer
BOOST_AUTO_TEST_CASE( inputDataMVD2Parsing )
{

    using namespace MVD2;


    MVD2File file(MVD2_FILENAME);

    MVD2Checker checker;
    file.parse(checker);



    BOOST_CHECK_EQUAL(checker.neuron_counter, 1000);

}



////////////////////////////////////////////////////
/// New common API
////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( basicTestPosition )
{
    using namespace MVD2;

    MVD2File file(MVD2_FILENAME);

    MVD::Positions neurons_pos = file.getPositions();

    BOOST_CHECK_EQUAL(neurons_pos.shape()[0], 1000);
    BOOST_CHECK_EQUAL(neurons_pos[0][0], static_cast<double>(40.821401));
    BOOST_CHECK_EQUAL(neurons_pos[0][1], static_cast<double>(1986.506637));
    BOOST_CHECK_EQUAL(neurons_pos[0][2], static_cast<double>(10.788424));

}


BOOST_AUTO_TEST_CASE( basicTestPositionRange )
{
    using namespace MVD2;

    MVD2File file(MVD2_FILENAME);

    MVD::Positions neurons_pos = file.getPositions(MVD::Range(10,10));
    MVD::Positions all_neurons_pos = file.getPositions();

    BOOST_CHECK_EQUAL(neurons_pos.size(), 10);

    for(int i =0; i < 10; ++i){
        BOOST_CHECK_EQUAL_COLLECTIONS(neurons_pos[i].begin(), neurons_pos[i].end(),
                                      all_neurons_pos[i+10].begin(), all_neurons_pos[i+10].end());
    }

}


BOOST_AUTO_TEST_CASE( basicTestRotations )
{
    using namespace MVD2;

    MVD2File file(MVD2_FILENAME);

    MVD::Positions neurons_pos = file.getRotations();

    BOOST_CHECK_EQUAL(neurons_pos.shape()[0], 1000);
    BOOST_CHECK_EQUAL(neurons_pos[0][0], static_cast<double>(-1.146572));
    BOOST_CHECK_EQUAL(neurons_pos[146][0], static_cast<double>(-125.718090));

}


BOOST_AUTO_TEST_CASE( basicTestRotationsRange )
{
    using namespace MVD2;

    MVD2File file(MVD2_FILENAME);

    MVD::Positions neurons_pos = file.getRotations(MVD::Range(100, 50));

    BOOST_CHECK_EQUAL(neurons_pos.shape()[0], 50);
    BOOST_CHECK_EQUAL(neurons_pos[46][0], static_cast<double>(-125.718090));

}

