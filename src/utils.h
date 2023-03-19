/*
    SIOpanel, a simple webUI for "mamaging" a SIO2 instance.
    Copyright (C) 2023 Olaf Targowski
    Contact: olaf.targowski@otsrv.net

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include <string>
#include <mutex>

namespace utils{

std::string get_unused_id();
bool checknumid(const std::string& id);
bool read_whole_file(std::string& dest, const std::string path);
std::string registration_check();
bool insert(std::string &str, const std::string &ins);

} // namespace utils
