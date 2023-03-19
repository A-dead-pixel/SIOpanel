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
