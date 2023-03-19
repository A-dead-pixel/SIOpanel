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

#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <boost/thread/thread.hpp>

extern std::queue <boost::thread> commands;

namespace controllers{
    
void executeCommand(std::string command, std::string resultfile, std::mutex *mt);
bool scheduleCommand(std::string body, std::string id, std::string type);

} // namespace controllers
