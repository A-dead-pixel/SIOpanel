#pragma once
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "config.cpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

using oatpp::web::server::api::ApiController;

class Controller : public ApiController {
public:
    Controller(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : ApiController(objectMapper){}
private:
    std::shared_ptr<ApiController::OutgoingResponse> redirect(oatpp::String loc){
        auto resp=ApiController::createResponse(Status::CODE_303, "");
        resp->putHeader("location", loc);
        return resp;
    }
};

#include OATPP_CODEGEN_END(ApiController)
