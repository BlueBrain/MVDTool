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
[[deprecated("For newer formats and API use open()")]]
inline MVDFile* open_mvd(const std::string & filename) {
    switch(_mvd_format(filename)) {
        case MVDType::MVD2:
            return new MVD2::MVD2File(filename);
            break;
        case MVDType::MVD3:
            return new MVD3::MVD3File(filename);
            break;
        default:
            throw std::runtime_error("Format not supported");
    }
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
inline std::shared_ptr<File> open(const std::string& filename,
                                  const std::string& population="default") {
    std::shared_ptr<File> mvdfile;
    switch(_mvd_format(filename)) {
        case MVDType::MVD2:
            throw std::runtime_error("MVD2 not supported in the new MVD API");
        case MVDType::MVD3:
            try {
                mvdfile.reset(new MVD3::MVD3File(filename));
                mvdfile->size();
            } catch (...) {
                throw std::runtime_error("invalid MVD3 file");
            }
            break;
        default:
            try {
                mvdfile.reset(new SonataFile(filename, population));
                mvdfile->size();
            } catch (...) {
                throw std::runtime_error("invalid Sonata file");
            }
            break;
    }
    return mvdfile;
}


} //MVD


#endif // MVD_GENERIC_HPP
