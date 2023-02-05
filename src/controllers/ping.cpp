#pragma once
#include "controllers/main_controller.cpp"
#include "config.h"

#include <vector>
#include <string>
#include <thread>

struct ping_thread{
    std::thread t;
    std::string p,*ret;
};

void ping(std::string addr, std::string *ret){
    addr="ping -c1 -s1 -W0.1 "+addr+" > /dev/null 2>&1";
    *ret=system(addr.data()) ? " is down" : " is up";
}

#include OATPP_CODEGEN_BEGIN(ApiController)

ENDPOINT("GET", "/ping", Controller::ping_routers){
    std::string res,ip;
    std::vector <ping_thread> th;
    for (std::string i : routerNums){
        th.push_back({std::thread(), i, new std::string});
        ip=router_base;
        utils::insert(ip, i);
        th.back().t=std::thread(ping, ip, th.back().ret);
    }
    for (ping_thread& i : th){
        i.t.join();
        res+="ST"+i.p+*(i.ret);
        delete i.ret;
    }
    return createResponse(Status::CODE_200, res);
}

#include OATPP_CODEGEN_END(ApiController)
