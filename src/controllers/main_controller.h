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

#pragma once
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "utils.h"
#include "config.h"
#include "control_utils.h"


#include OATPP_CODEGEN_BEGIN(ApiController)

namespace controllers{

class Controller : public oatpp::web::server::api::ApiController {

private:
    std::shared_ptr<oatpp::web::server::api::ApiController::OutgoingResponse> redirect(oatpp::String loc){
        auto resp=ApiController::createResponse(Status::CODE_303, "");
        resp->putHeader("location", loc);
        return resp;
    }

public:
    Controller(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper){}

    ENDPOINT("GET", "/", root_redirect) {
        return redirect("/control/"+utils::get_unused_id()+"/");
    }

    ENDPOINT("GET", "/control/", control_redirect) {
        return redirect("/control/"+utils::get_unused_id()+"/");
    }

    ENDPOINT("GET", "/control/{id}/", get_control_panel,
            PATH(String, id)) {
        if (utils::checknumid(id)){
            printf("%s\n", config::templates["403.html"].data());
            return createResponse(Status::CODE_403, config::templates["403.html"]);
        }
        std::string outfile,result,status;
        if (utils::read_whole_file(outfile, std::string("results/"+id))){
            result=config::templates["result.html"];
            if (utils::read_whole_file(status, std::string("results/"+id+".ret")))
                status="Egzekucja zakończona "+status;
            else
                status="Egzekucja w trakcie...";
            utils::insert(result, status);
            utils::insert(result, outfile);
        }
        else{
            result=config::templates["control_panel.html"];
            status=utils::registration_check();
            utils::insert(result, status);
        }
        return createResponse(Status::CODE_200, result);
    }

    ENDPOINT("POST", "/control/{id}/{type}", control_handle_post,
            PATH(String, id), PATH(String, type), BODY_STRING(String, body)) {
        if (utils::checknumid(id))
            return createResponse(Status::CODE_403, config::templates["403.html"]);
        if (scheduleCommand(body, id, type))
            return redirect("/control/"+id+"/");
        return createResponse(Status::CODE_500, config::templates["command_failed.html"]);
    }

};


} // namespace controllers

#include OATPP_CODEGEN_END(ApiController)
