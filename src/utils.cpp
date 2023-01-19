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

std::string url,siourl,docker;

namespace utils{
using namespace std;
mutex tmpfile_mutex;
int id_counter=0;
string get_unused_id(){
    unique_lock <mutex> lk(tmpfile_mutex); // the destructor will unlock
    return to_string(++id_counter);
}

int getnumid(string id){
    int numid;
    try {
        numid=stoi(id);
    }
    catch (const exception& exception) {
        numid=0;
    }
    return numid<=id_counter ? max(numid, 0) : 0; // 0 means wrong
}

void ping(string addr, bool *ret){
    addr="ping -c1 -s1 -W0.1 "+addr+" > /dev/null 2>&1";
    *ret=system(addr.data());
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
    std::string tmp;
    std::ifstream file;
    file.open(path, ios::in);
    if (!file.is_open())
        return 0;
    while (file){
        std::getline(file, tmp);
        dest+=tmp+'\n';
    }
    return 1;
}

mutex restartMt,phaseMt,regMt,outMt;
queue <thread> commands;

void executeCommand(string command, string resultfile, mutex *mt){
    unique_lock <mutex> lck(*mt);
    printf("Starting execution: %s\n", command.data());
    int ret=system(command.data());
    ofstream file(resultfile);
    file<<(ret ? "porażką\n" : "sukcesem\n");
    printf("Finished execution: %s\n", command.data());
}

bool scheduleCommand(string body, string id, string type){
    string command="";
    mutex *mt=nullptr;
    if (type.starts_with("reset_"))
        mt=&restartMt;
    if (type=="reset_spr")
        command=docker+"exec web rm sioworkersd.db;"+docker+"exec web ./manage.py supervisor restart evalmgr filetracker-server receive_from_workers sioworkersd unpackmgr";
    if (type=="reset_web")
        command=docker+"exec web ./manage.py supervisor restart uwsgi notifications-server";
    if (type=="reset_docker")
        command=docker+"exec web rm sioworkersd.db;"+docker+"down;"+docker+"up -d";
    if (type=="phase_all"){
        command=docker+"exec web ./manage.py update_scores -a";
        mt=&phaseMt;
    }
    if (type=="phase_by_id"){ // contest_id=XXXX
        if (body.size()<12)
            return 0;
        command=docker+"exec web ./manage.py update_scores ";
        for (char z : body.substr(11))
            if (isalnum(z)||z=='_'||z=='-')
                command+=z;
        mt=&phaseMt;
    }
    if (type=="registration"){ // action=X
        if (body.size()<8)
            return 0;
        command=docker+"exec web ./manage.py talent_view_groups ";
        if (body[7]=='c'||body[7]=='o'){
            command+="-";
            command+=body[7];
        }
        mt=&regMt;
    }
    if (command=="")
        return 0;
    string outfile="results/"+id;
    command="bash -c '("+command+") > "+outfile+" 2>&1'";
    unique_lock <mutex> lck(outMt);
    if (filesystem::exists(outfile))
        return 0;
    ofstream file(outfile);
    file.flush();
    file.close();
    lck.unlock();
    commands.push(thread(executeCommand, command, "results/"+id+".ret", mt));
    return 1;
}

bool registration_check(){
    return !system(string("bash -c 'curl -Ls --connect-timeout 0.5 "+siourl+"/register | grep Contest.group > /dev/null'").data());
}

void insert(string &str, string &ins){
    str.replace(str.find("%s"), 2, ins);
}


} // namespace std
