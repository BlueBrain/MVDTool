/*
 * Copyright (C) 2019, Blue Brain Project, EPFL
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
#include <sstream>

#include <mvdtool/tsv.hpp>

#define BOOST_TEST_MODULE tsvParser
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( ReadTSVFile )
{
    using namespace TSV;

    TSVFile mecombofile(TSV_FILENAME);

    auto infos = mecombofile.get({"dSTUT_1_87dd39e6b0255ec053001f16da85b0e0"},
                                 {"87dd39e6b0255ec053001f16da85b0e0"});

    std::string referenceTSVInfo("87dd39e6b0255ec053001f16da85b0e0, 1, L1_DAC, dSTUT, dSTUT_321707905, dSTUT_1_87dd39e6b0255ec053001f16da85b0e0, 0, 0.1");
    const MEComboEntry& entry = infos[0];
    std::stringstream ss;
    ss << entry.morphologyName << ", "
       << entry.layer << ", "
       << entry.fullMType << ", "
       << entry.eType << ", "
       << entry.eModel << ", "
       << entry.comboName << ", "
       << entry.thresholdCurrent  << ", "
       << entry.holdingCurrent;

    BOOST_CHECK_EQUAL(referenceTSVInfo, ss.str());

}

BOOST_AUTO_TEST_CASE( TSVFileParsing )
{
    using namespace TSV;

    TSVFile mecombofile(TSV_FILENAME);

    // Ensure it throws when asking mecombo that doesn't exist in the mecombo.tsv file
    BOOST_CHECK_THROW(mecombofile.get({"dSTUT_1_87dd39e6b0255ec053001f16da85b0e0"},
                                      {"a4dc631127a7bde0adf5f58634397757"}),
                      TSVException);

}
