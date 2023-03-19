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

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "app_component.h"
#include "controllers/main_controller.h"
#include "config.h"
#include "control_utils.h"

#include <unistd.h>
#include <ctime>
#include <filesystem>
#include <string>
#include <csignal>
#include <mutex>

static std::mutex serverMt;
static int retnum=0;

void run() {
    /* Register Components in scope of run() method */
    AppComponent components;

    /* Get router component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    /* Create MyController and add all of its endpoints to router */
    auto controller = std::make_shared<controllers::Controller>();
    router->addController(controller);

    /* Get connection handler component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

    /* Get connection provider component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);
    
    /* for unblocking the connection handler or sth, see https://github.com/oatpp/oatpp/issues/173 */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);

    /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
    oatpp::network::Server server(connectionProvider, connectionHandler);
    /* Print info about server port */
    OATPP_LOGI("SIOpanel", "Server running on port %s.", connectionProvider->getProperty("port").getData());

    /* Run server */
    std::thread server_thread([&server]{server.run();});
    serverMt.lock();
    server.stop();
    clientConnectionProvider->get();
    connectionHandler->stop();
    server_thread.join();
    OATPP_LOGI("SIOpanel", "Server stopped.");
}

void cleanup(int signal_num){
    serverMt.unlock();
    retnum=signal_num;
}

int main(){
    std::string cwd="SIOpanel_"+std::to_string(time(0));
    if (!std::filesystem::create_directory(cwd)){
        printf("SIOpanel: directory already exists, wait a second\n");
        return 69;
    }
    chdir(cwd.data());
    
    std::filesystem::create_directory("results");
    std::filesystem::create_directory("timestamps");
    printf("Running in %s\n", cwd.data());
    
    config::load_templates("../templates");
    config::load_config("../SIOpanel.toml");
    

    signal(SIGINT, cleanup);
    signal(SIGABRT, cleanup);
    signal(SIGTERM, cleanup);
    
    /* Run App */
    serverMt.lock();
    oatpp::base::Environment::init();
    run();
    oatpp::base::Environment::destroy();

    while (!commands.empty()){
        commands.front().join();
        commands.pop();
    }
    return retnum;
}
