#pragma once
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <string>

#include "utils.h"

#include <toml++/toml.h>

std::string router_base,panelurl,siourl,docker_base,command_base;
std::vector <std::string> router_nums;
std::unordered_map <std::string, std::string> templates;

namespace config{

void load_templates(std::string path);
void load_config(std::string path);

} // namespace config
