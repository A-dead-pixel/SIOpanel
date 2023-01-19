#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "AppComponent.hpp"
#include "controller/controller.hpp"
#include "utils.cpp"
#include "templates.cpp"

#include <unistd.h>
#include <string>

void run() {
    /* Register Components in scope of run() method */
    AppComponent components;

    /* Get router component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    /* Create MyController and add all of its endpoints to router */
    auto controller = std::make_shared<Controller>();
    router->addController(controller);

    /* Get connection handler component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

    /* Get connection provider component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
    oatpp::network::Server server(connectionProvider, connectionHandler);

    /* Priny info about server port */
    OATPP_LOGI("SIOpanel", "Server running on port %s", connectionProvider->getProperty("port").getData());

    /* Run server */
    server.run();
}

char cwd[FILENAME_MAX];
std::string scwd;
int main(){
    utils::prepare();
    getcwd(cwd, FILENAME_MAX);
    scwd=std::string(cwd);
    printf("Running in %s\n", scwd.data());
    
    load_templates(scwd+"/../templates/");
    load_config(scwd+"/../SIOpanel.conf");
    
    oatpp::base::Environment::init();

    /* Run App */
    run();

    oatpp::base::Environment::destroy();
    return 0;
}
