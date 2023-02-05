#include "utils.h"

namespace utils{
using namespace std;
string get_unused_id(){
    unique_lock <mutex> lk(tmpfile_mutex); // the destructor will unlock
    return to_string(++id_counter);
}

bool checknumid(string& id){ // 1 => failed check
    try {
        int numid=stoi(id);
        return numid>id_counter||numid<=0;
    }
    catch (const exception& exception) {
        return 1;
    }
}

void prepare(){
    string dirname="SIOpanel_"+to_string(time(0));
    if (!filesystem::create_directory(dirname)){
        printf("SIOpanel: directory already exists, wait a second\n");
        exit(69);
    }
    chdir(dirname.data());
    filesystem::create_directory("results");
    filesystem::create_directory("timestamps");
}

bool read_whole_file(string& dest, string path){
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
    string command="bash -c 'curl -Ls --connect-timeout 0.5 "+siourl+"/register'";
    if (system(command.data()))
        return "???";
    command=command.substr(0, command.size()-2)+" | grep Contest.group > /dev/null'";
    return system(command.data()) ? "zamknięta" : "otwarta";
}

void insert(string &str, string &ins){
    str.replace(str.find("%s"), 2, ins);
}


} // namespace std
