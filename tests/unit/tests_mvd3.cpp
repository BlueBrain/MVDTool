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
#include <mvd/mvd_generic.hpp>
#define BOOST_TEST_MAIN mvd3Parser
#include <boost/test/included/unit_test.hpp>



BOOST_AUTO_TEST_CASE( isFile )
{
    using namespace MVD;

    BOOST_CHECK_EQUAL(MVD::is_mvd_file(MVD3_FILENAME), MVD::MVDType::MVD3);
    BOOST_CHECK_EQUAL(MVD::is_mvd_file(MVD2_FILENAME), MVD::MVDType::MVD2);
}



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

BOOST_AUTO_TEST_CASE( basicTestPositionRange )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    Positions neurons_pos = file.getPositions(Range(10,10));
    Positions all_neurons_pos = file.getPositions();

    BOOST_CHECK_EQUAL(neurons_pos.size(), 10);

    for(int i =0; i < 10; ++i){
        BOOST_CHECK_EQUAL_COLLECTIONS(neurons_pos[i].begin(), neurons_pos[i].end(),
                                      all_neurons_pos[i+10].begin(), all_neurons_pos[i+10].end());
    }

}



BOOST_AUTO_TEST_CASE( basicTestRotations )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    Rotations neurons_rot = file.getRotations();

    BOOST_CHECK_EQUAL(neurons_rot.shape()[0], 1000);
    BOOST_CHECK( std::abs(neurons_rot[0][0] - 0.) < std::numeric_limits<double>::epsilon()*10 );
    BOOST_CHECK( std::abs(neurons_rot[0][1] - -0.010005561303180392) < std::numeric_limits<double>::epsilon()*10 );
    BOOST_CHECK( std::abs(neurons_rot[0][2] - 0) < std::numeric_limits<double>::epsilon()*10 );
    BOOST_CHECK( std::abs(neurons_rot[0][3] - 0.99994994311865848) < std::numeric_limits<double>::epsilon()*10 );

}


BOOST_AUTO_TEST_CASE( basicTestRotationRange )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    Positions neurons_rot = file.getRotations(Range(10,10));
    Positions all_neurons_rot = file.getRotations();

    BOOST_CHECK_EQUAL(neurons_rot.size(), 10);

    for(int i =0; i < 10; ++i){
        BOOST_CHECK_EQUAL_COLLECTIONS(neurons_rot[i].begin(), neurons_rot[i].end(),
                                      all_neurons_rot[i+10].begin(), all_neurons_rot[i+10].end());
    }

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


BOOST_AUTO_TEST_CASE( basicTestMorphologiesRange )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> morpho = file.getMorphologies(Range(10,10));
    std::vector<std::string> all_morpho = file.getMorphologies();
    std::vector<std::string> list_morpho = file.listAllMorphologies();

    BOOST_CHECK_EQUAL(morpho.size(), 10);

    for(int i =0; i < 10; ++i){
        BOOST_CHECK_EQUAL(morpho[i], all_morpho[i+10]);
        BOOST_CHECK_EQUAL(morpho[i], list_morpho[file.getIndexMorphologies(Range(i, 1))[0]]);
    }

}



BOOST_AUTO_TEST_CASE( basicTestEtype )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> etype = file.getEtypes();

    BOOST_CHECK_EQUAL(etype[0], "cACint");
    BOOST_CHECK_EQUAL(etype[10], "cACint");
    BOOST_CHECK_EQUAL(etype[20], "cADpyr");

}



BOOST_AUTO_TEST_CASE( basicTestEtypeRange )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> etype = file.getEtypes(Range(10,10));
    std::vector<std::string> etype_all = file.getEtypes();
    std::vector<std::string> list_etypes = file.listAllEtypes();

    BOOST_CHECK_EQUAL(etype.size(), 10);

    for(int i =0; i < 10; ++i){
        BOOST_CHECK_EQUAL(etype[i], etype_all[i+10]);
        BOOST_CHECK_EQUAL(etype[i], list_etypes[file.getIndexEtypes(Range(i, 1))[0]]);
    }

}


BOOST_AUTO_TEST_CASE( basicTestMypes )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> mtype = file.getMtypes();

    BOOST_CHECK_EQUAL(mtype[0], "L1_SLAC");
    BOOST_CHECK_EQUAL(mtype[10], "L1_SLAC");
    BOOST_CHECK_EQUAL(mtype[20], "L23_PC");

}



BOOST_AUTO_TEST_CASE( basicTestRegions )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> region = file.getRegions();

    BOOST_CHECK_EQUAL(region[1], "L1");
    BOOST_CHECK_EQUAL(region[9], "L2");
    BOOST_CHECK_EQUAL(region[10], "L3");
    BOOST_CHECK_EQUAL(region[15], "L23");
}



BOOST_AUTO_TEST_CASE( basicTestHypercolumns )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<boost::int32_t> hcolumns = file.getHyperColumns();

    BOOST_CHECK_EQUAL(hcolumns[0], 0);
    BOOST_CHECK_EQUAL(hcolumns[10], 0);
    BOOST_CHECK_EQUAL(hcolumns[20], 0);

}



BOOST_AUTO_TEST_CASE( basicTestminicolumns )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<boost::int32_t> mini_columns = file.getMiniColumns();

    BOOST_CHECK_EQUAL(mini_columns[100], 1);
    BOOST_CHECK_EQUAL(mini_columns[101], 2);
    BOOST_CHECK_EQUAL(mini_columns[102], 3);

}



BOOST_AUTO_TEST_CASE( basicLayer )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<boost::int32_t> layer = file.getLayers();

    BOOST_CHECK_EQUAL(layer[0], 1);
    BOOST_CHECK_EQUAL(layer[100], 2);
    BOOST_CHECK_EQUAL(layer[200], 3);

}



BOOST_AUTO_TEST_CASE( basicTestMtypeRange )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> mtype = file.getMtypes(Range(10,10));
    std::vector<std::string> mtype_all = file.getMtypes();
    std::vector<std::string> list_mtype = file.listAllMtypes();

    BOOST_CHECK_EQUAL(mtype.size(), 10);

    for(int i =0; i < 10; ++i){
        BOOST_CHECK_EQUAL(mtype[i], mtype_all[i+10]);
        BOOST_CHECK_EQUAL(mtype[i], list_mtype[file.getIndexMtypes(Range(i, 1))[0]]);
    }

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


BOOST_AUTO_TEST_CASE( basicTestSynClassRange )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<std::string> syn_class = file.getSynapseClass();
    std::vector<std::string> syn_class_all = file.getSynapseClass();
    std::vector<std::string> list_syn_class = file.listAllSynapseClass();

    for(int i =0; i < 10; ++i){
        BOOST_CHECK_EQUAL(syn_class[i], syn_class_all[i+10]);
        BOOST_CHECK_EQUAL(syn_class[i], list_syn_class[file.getIndexSynapseClass(Range(i, 1))[0]]);
    }

}


BOOST_AUTO_TEST_CASE( basicTestSeeds )
{
    using namespace MVD3;

    MVD3File file(MVD3_FILENAME);

    std::vector<double> seeds = file.getCircuitSeeds();

    BOOST_CHECK_EQUAL(seeds.size(), 4);

    double values[] = { 837632.0, 2906729.0, 4236279.0, 0};

    for(size_t i = 0; i < seeds.size(); ++i){
        BOOST_CHECK_EQUAL(seeds[i], values[i]);
    }

}


BOOST_AUTO_TEST_CASE( genericMVDFile )
{
    using namespace MVD;

    MVDFile *file = open_mvd(MVD3_FILENAME);

    Positions neurons_pos = file->getPositions();

    BOOST_CHECK_EQUAL(neurons_pos.shape()[0], 1000);
    BOOST_CHECK_EQUAL(neurons_pos[0][0], static_cast<double>(40.821401));
    BOOST_CHECK_EQUAL(neurons_pos[0][1], static_cast<double>(1986.506637));
    BOOST_CHECK_EQUAL(neurons_pos[0][2], static_cast<double>(10.788424));

    delete file;
}
