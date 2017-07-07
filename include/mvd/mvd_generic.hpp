/*
 * Copyright (C) 2015 Adrien Devresse <adrien.devresse@epfl.ch>
 *               2017 Fernando Pereira <fernando.pereira@epfl.ch>
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
#ifndef MVD_GENERIC_HPP
#define MVD_GENERIC_HPP

#include "mvd_base.hpp"
#include "mvd2.hpp"
#include "mvd3.hpp"


namespace MVD {


///
/// \brief open opens a mvd file, either MV2 or MVD3
/// \param filename
/// \return Simple ptr to an MVD2File/MVD3File object (MVDFile interface)
/// We should use unique_ptr here, but keeping compilation without c++11
///
inline MVDFile* open_mvd(const std::string & filename) {
    if( MVD::is_mvd_file(filename) == MVDType::MVD2 ) {
        return new MVD2::MVD2File(filename);
    }

    return new MVD3::MVD3File(filename);
}

} //MVD


#endif // MVD_GENERIC_HPP
