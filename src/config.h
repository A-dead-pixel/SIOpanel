#pragma once
#include <vector>
#include <unordered_map>
#include <string>

namespace config{

extern std::string router_base,panelurl,siourl,docker_base,command_base;
extern std::vector <std::string> router_nums;
extern std::unordered_map <std::string, std::string> templates;

void load_templates(const std::string path);
void load_config(const std::string path);

} // namespace config
