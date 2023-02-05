#include "config.h"


namespace config{
using std::string;

void load_templates(string path){
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        string path=entry.path().lexically_normal(),filename=entry.path().filename();
        if (path.ends_with(".html")){ // this can be expanded
            printf("Loading  %s\n", path.data());
            if (!utils::read_whole_file(templates[filename], path)){
                printf("Error while loading!");
                exit(70);
            }
        }
        else
            printf("Skipping %s\n", path.data());
    }
}
void load_config(string path){
    try {
        toml::table tbl=toml::parse_file("SIOpanel.toml");
        command_base=tbl["sio"]["command_base"].value_or(string());
        docker_base=tbl["sio"]["docker_base"].value_or(string());
        siourl=tbl["sio"]["url"].value_or(string());
        router_base=tbl["ping"]["router_base"].value_or(string());
        toml::array *nums=tbl["ping"]["router_nums"].as_array();
        nums->for_each([](toml::value<string> &i){router_nums.push_back(string(i));});
    }
    catch (const toml::parse_error &err){
        printf("Config parsing failed:\n%s\n", err.description().data());
        exit(1);
    }
}

} // namespace config
