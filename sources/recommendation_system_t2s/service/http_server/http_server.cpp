#include "http_server.hpp"

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/DateTimeFormat.h"

#include "../database/database.hpp"

#include "handlers/include/handlers/request_handler_factory.hpp"

#include <iostream>

using ServerSocket = Poco::Net::ServerSocket;
using SocketAddress = Poco::Net::SocketAddress;
using HTTPServer = Poco::Net::HTTPServer;
using HTTPServerParams = Poco::Net::HTTPServerParams;
using DateTimeFormat = Poco::DateTimeFormat;


namespace recsys_t2s {

    HTTPWebServer::HTTPWebServer() : m_IsHelpRequested(false) { }

    void HTTPWebServer::initialize( Poco::Util::Application& t_self ) {
        loadConfiguration();
        ServerApplication::initialize(t_self);
    }

    int HTTPWebServer::main( const std::vector<std::string>& t_args ) {

        if( t_args.empty()) {
            std::cerr << "Error. Application required the service config path as first command line argument."
                      << std::endl;
            return EXIT_FAILURE;
        }

        m_Config = std::make_shared<config::Config>(t_args.at(0));

        const std::shared_ptr<config::NetworkConfig> network_config = m_Config->GetNetworkConfig();
        if( !database::Database::Instance()->IsConnected()) {
            database::Database::Instance()->BindConfig(m_Config->GetDatabaseConfig());
            bool result = database::Database::Instance()->TryConnect();
            if( !result ) {
                std::cerr << "Failed connect to database." << std::endl;
                return EXIT_DATAERR;
            }
        }
        std::cout << "Connected." << std::endl;

        ServerSocket svs( SocketAddress(*network_config->GetIpAddress(), *network_config->GetPort()));
        HTTPServer srv(new handlers::RequestHandlerFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);

        std::cout << "Application will started as " << svs.address().toString() << std::endl;

        srv.start();
        waitForTerminationRequest();
        srv.stop();

        return Application::EXIT_OK;
    }

} // namespace search_service