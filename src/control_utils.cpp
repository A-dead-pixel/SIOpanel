/*
    SIOpanel, a simple webUI for "mamaging" a SIO2 instance.
    Copyright (C) 2023 Olaf Targowski
    Contact: olaf.targowski@otsrv.net

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "control_utils.h"

#include "utils.h"
#include "config.h"

#include <string>
#include <mutex>
#include <fstream>
#include <queue>
#include <filesystem>
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>

std::queue <boost::thread> commands;

namespace controllers{
using namespace std;
using config::docker_base;

static mutex restartMt,phaseMt,regMt,commandMt;

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
        command=docker_base+"exec web rm sioworkersd.db;"+docker_base+"exec web ./manage.py supervisor restart evalmgr filetracker-server receive_from_workers sioworkersd unpackmgr";
    if (type=="reset_web")
        command=docker_base+"exec web ./manage.py supervisor restart uwsgi notifications-server";
    if (type=="reset_docker")
        command=docker_base+"exec web rm sioworkersd.db;"+docker_base+"down;"+docker_base+"up -d";
    if (type=="phase_all"){
        command=docker_base+"exec web ./manage.py update_scores -a";
        mt=&phaseMt;
    }
    if (type=="phase_by_id"){ // contest_id=XXXX
        if (body.size()<12)
            return 0;
        command=docker_base+"exec web ./manage.py update_scores ";
        for (char z : body.substr(11))
            if (isalnum(z)||z=='_'||z=='-')
                command+=z;
        mt=&phaseMt;
    }
    if (type=="registration"){ // action=X
        if (body.size()<8)
            return 0;
        command=docker_base+"exec web ./manage.py talent_view_groups ";
        if (body[7]=='c'||body[7]=='o'){
            command+="-";
            command+=body[7];
        }
        mt=&regMt;
    }
    if (command=="")
        return 0;
    string outfile="results/"+id,final_command=config::command_base;
    utils::insert(final_command, command);
    utils::insert(final_command, outfile);
    unique_lock <mutex> lck(commandMt);
    if (filesystem::exists(outfile))
        return 0;
    ofstream file(outfile);
    file.flush();
    file.close();
    // so we don't run out of memory because of the queue
    while (commands.size()&&commands.front().try_join_for(boost::chrono::milliseconds(1)))
        commands.pop();
    commands.push(boost::thread(executeCommand, final_command, "results/"+id+".ret", mt));
    lck.unlock();
    return 1;
}

} // namespace controllers
