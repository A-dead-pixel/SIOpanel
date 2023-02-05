#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "app_component.hpp"
#include "controllers/main_controller.cpp"
#include "utils.h"
#include "config.h"

#include <unistd.h>
#include <string>
#include <csignal>
#include <mutex>

std::mutex exitMt,serverMt;

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
    /* Print info about server port */
    OATPP_LOGI("SIOpanel", "Server running on port %s.", connectionProvider->getProperty("port").getData());

    /* Run server */
    std::thread server_thread([&server]{server.run();});
    serverMt.lock();
    connectionProvider->stop();
    connectionHandler->stop();
    server.stop();
    OATPP_LOGI("SIOpanel", "Server stopped.");
    
}

int retnum=0;
void cleanup(int signal_num){
    exitMt.lock();
    serverMt.unlock();
    retnum=signal_num;
}

int main(){
    utils::prepare();
    char cwd[FILENAME_MAX];
    getcwd(cwd, FILENAME_MAX);
    std::string scwd=std::string(cwd);
    printf("Running in %s\n", scwd.data());
    
    config::load_templates(scwd+"/../templates/");
    config::load_config(scwd+"/../SIOpanel.conf");
    
    oatpp::base::Environment::init();

    signal(SIGINT, cleanup);
    signal(SIGABRT, cleanup);
    signal(SIGTERM, cleanup);
    
    /* Run App */
    serverMt.lock();
    run();

    oatpp::base::Environment::destroy();
    return retnum;
}
