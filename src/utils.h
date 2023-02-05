#pragma once
#include <unistd.h>
#include <string>
#include <cstdio>
#include <mutex>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <queue>

#include "config.h"

namespace utils{
using namespace std;

mutex tmpfile_mutex;
int id_counter=0;
string get_unused_id();
bool checknumid(string& id);
void prepare();
bool read_whole_file(string& dest, string path);
string registration_check();
void insert(string &str, string &ins);

} // namespace utils
