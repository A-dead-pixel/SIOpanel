#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "utils.cpp"
#include "templates.cpp"
#include OATPP_CODEGEN_BEGIN(ApiController)

#include <vector>
#include <string>
#include <thread>
#include <filesystem>

class Controller : public oatpp::web::server::api::ApiController {
public:
  Controller(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:
  
    ENDPOINT("GET", "/control/", control_redirect) {
        auto response=createResponse(Status::CODE_307);
        std::string id=utils::get_unused_id();
        response->putHeader("location", url+"/control/"+id+"/");
        return response;
    }
    
    ENDPOINT("GET", "/control/{id}/", get_control_panel,
            PATH(String, id)) {
        int numid=utils::getnumid(id);
        if (!numid)
            return createResponse(Status::CODE_403, templates["403.html"]);
        std::string outfile,result,status,retfile;
        if (utils::read_whole_file(outfile, std::string("results/"+id))){
            result=templates["result.html"];
            if (utils::read_whole_file(retfile, std::string("results/"+id+".ret")))
                retfile="Egzekucja zakończona "+retfile;
            else
                retfile="Egzekucja w trakcie...";
            utils::insert(result, retfile);
            utils::insert(result, outfile);
        }
        else{
            result=templates["control_panel.html"];
            status=utils::registration_check() ? "otwarta" : "zamknięta";
            utils::insert(result, status);
        }
        return createResponse(Status::CODE_200, result);
    }
    
    ENDPOINT("POST", "/control/{id}/{type}", control_handle_post,
            PATH(String, id), PATH(String, type), BODY_STRING(String, body)) {
        int numid=utils::getnumid(id);
        if (!numid)
            return createResponse(Status::CODE_403, templates["403.html"]);
        if (utils::scheduleCommand(body, id, type)){
            auto ret=createResponse(Status::CODE_303);
            ret->putHeader("location", url+"/control/"+id+"/");
            return ret;
        }
        return createResponse(Status::CODE_500, templates["command_failed.html"]);
    }

    ENDPOINT("GET", "/ping", ping_routers){
        std::string res;
        struct jajco {std::thread t; std::string p; bool *ret;};
        std::vector <jajco> th;
        for (std::string i : routerNums){
            th.push_back({});
            th.back().p=i;
            th.back().ret=new bool;
            th.back().t=std::thread(utils::ping, routerPrefix+i, th.back().ret);
        }
        for (jajco& i : th){
            i.t.join();
            res+="ST"+i.p+(*(i.ret) ? " is down :(\n" : " is up :)\n");
            delete i.ret;
        }
        return createResponse(Status::CODE_200, res);
    }
};

#include OATPP_CODEGEN_END(ApiController)
