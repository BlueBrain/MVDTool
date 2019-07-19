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
#include "sonata.hpp"


namespace MVD {


///
/// \brief open opens a mvd file, either MV2 or MVD3
/// \param filename
/// \return Simple ptr to an MVD2File/MVD3File object (MVDFile interface)
/// We should use unique_ptr here, but keeping compilation without c++11
///
[[deprecated("use open()")]]
inline MVDFile* open_mvd(const std::string & filename) {
    if( MVD::is_mvd_file(filename) == MVDType::MVD2 ) {
        return new MVD2::MVD2File(filename);
    }

    return new MVD3::MVD3File(filename);
}

///
/// \brief open
///
/// Opens a file with the associated class. A basic check for file type is
/// performed, but no guarantees about file correctness are made.
///
/// \param filename the path of the file to open
/// \return a shared pointer to a mvd::File object
///
inline std::shared_ptr<File> open(const std::string& filename) {
    const std::string mvd3 = ".mvd3";

    if (filename.size() >= mvd3.size() && filename.compare(filename.size() - mvd3.size(),
                                                           mvd3.size(),
                                                           mvd3) == 0) {
        auto ptr = std::shared_ptr<File>(new MVD3::MVD3File(filename));
        try {
            ptr->size();
            return ptr;
        } catch (...) {
            throw std::runtime_error("invalid MVD3 file");
        }
    }

    // The only other file format we understand at this point is SONATA
    auto ptr = std::shared_ptr<File>(new SonataFile(filename));
    try {
        ptr->size();
    } catch (...) {
        throw std::runtime_error("can't determine file type for: " + filename);
    }
    return ptr;
}

} //MVD


#endif // MVD_GENERIC_HPP
