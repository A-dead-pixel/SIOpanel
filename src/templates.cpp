#pragma once
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <string>

#include "utils.cpp"

std::string routerPrefix;
std::vector <std::string> routerNums;
std::unordered_map <std::string, std::string> templates;
void load_templates(std::string path){
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        std::string path=entry.path().lexically_normal(),filename=entry.path().filename();
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

void load_config(std::string path){
    std::ifstream conf;
    std::string tmp;
    conf.open(path, std::ios::in);
    if (!conf.is_open()){
        printf("Error while loading pingconfig!");
        exit(70);
    }
    conf>>docker>>url>>siourl>>routerPrefix;
    docker="docker-compose -f "+docker+" ";
    while (conf){
        conf>>tmp;
        routerNums.push_back(tmp);
    }
}
