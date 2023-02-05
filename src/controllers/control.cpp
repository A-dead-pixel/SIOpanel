#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "utils.h"
#include "config.h"
#include "controllers/main_controller.cpp"

#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <queue>
#include <filesystem>

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
    string outfile="results/"+id,final_command=command_base;
    utils::insert(final_command, command);
    utils::insert(final_command, outfile);
    unique_lock <mutex> lck(outMt);
    if (filesystem::exists(outfile))
        return 0;
    ofstream file(outfile);
    file.flush();
    file.close();
    lck.unlock();
    commands.push(thread(executeCommand, final_command, "results/"+id+".ret", mt));
    return 1;
}

#include OATPP_CODEGEN_BEGIN(ApiController)

ENDPOINT("GET", "/control/", Controller::control_redirect) {
    return redirect("/control/"+utils::get_unused_id()+"/")
}

ENDPOINT("GET", "/control/{id}/", Controller::get_control_panel,
        PATH(String, id)) {
    if (utils::checknumid(id))
        return createResponse(Status::CODE_403, templates["403.html"]);
    std::string outfile,result,statusle;
    if (utils::read_whole_file(outfile, std::string("results/"+id))){
        result=templates["result.html"];
        if (utils::read_whole_file(status, std::string("results/"+id+".ret")))
            status="Egzekucja zakończona "+status;
        else
            status="Egzekucja w trakcie...";
        utils::insert(result, status);
        utils::insert(result, outfile);
    }
    else{
        result=templates["control_panel.html"];
        status=utils::registration_check();
        utils::insert(result, status);
    }
    return createResponse(Status::CODE_200, result);
}

ENDPOINT("POST", "/control/{id}/{type}", Controller::control_handle_post,
        PATH(String, id), PATH(String, type), BODY_STRING(String, body)) {
    if (utils::checknumid(id))
        return createResponse(Status::CODE_403, templates["403.html"]);
    if (scheduleCommand(body, id, type))
        return redirect("/control/"+id+"/");
    return createResponse(Status::CODE_500, templates["command_failed.html"]);
}

#include OATPP_CODEGEN_END(ApiController)
