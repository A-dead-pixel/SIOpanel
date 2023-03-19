#include "utils.h"
#include "config.h"

#include <unistd.h>
#include <string>
#include <cstdio>
#include <mutex>
#include <fstream>

using namespace std;

namespace utils{

static std::mutex counter_mutex;
static int id_counter=0;


string get_unused_id(){
    unique_lock <mutex> lk(counter_mutex); // the destructor will unlock
    return to_string(++id_counter);
}

bool checknumid(const string &id){ // 1 => failed check
    try {
        int numid=stoi(id);
        return numid>id_counter||numid<=0;
    }
    catch (const exception& exception) {
        return 1;
    }
}

bool read_whole_file(string &dest, const string path){
    string tmp;
    ifstream file;
    file.open(path, ios::in);
    if (!file.is_open())
        return 0;
    while (file){
        getline(file, tmp);
        dest+=tmp+'\n';
    }
    return 1;
}

string registration_check(){
    string command="bash -c 'curl -Ls --connect-timeout 0.5 "+config::siourl+"/register'";
    if (system(command.data()))
        return "???";
    command=command.substr(0, command.size()-2)+" | grep Contest.group > /dev/null'";
    return system(command.data()) ? "zamknięta" : "otwarta";
}

bool insert(string &str, const string &ins){ // true if succeeded
    auto pos=str.find("%s");
    if (pos>=str.size())
        return 0;
    str.replace(pos, 2, ins);
    return 1;
}


} // namespace std
