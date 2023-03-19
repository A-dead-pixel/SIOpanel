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
