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

#include <filesystem>
#include <cstdio>

#include <toml++/toml.h>

#include "config.h"
#include "utils.h"

namespace config{
using namespace std;

string router_base,panelurl,siourl,docker_base,command_base;
vector <std::string> router_nums;
unordered_map <string, string> templates;

void load_templates(const string path){
    for (const auto& entry : filesystem::directory_iterator(path)){
        string filepath=entry.path().lexically_normal();
        if (filepath.ends_with(".html")){ // this can be expanded
            printf("Loading  %s\n", filepath.data());
            if (!utils::read_whole_file(templates[entry.path().filename()], filepath)){
                printf("Error while loading!");
                exit(70);
            }
        }
        else
            printf("Skipping %s\n", path.data());
    }
}
void load_config(const string path){
    try {
        toml::table tbl=toml::parse_file(path);
        command_base=tbl["sio"]["command_base"].value_or(string());
        docker_base=tbl["sio"]["docker_base"].value_or(string());
        siourl=tbl["sio"]["url"].value_or(string());
    }
    catch (const toml::parse_error &err){
        printf("Config parsing failed:\n%s\n", err.description().data());
        exit(1);
    }
}

} // namespace config
