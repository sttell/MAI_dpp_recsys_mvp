#include "http_server.hpp"

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/DateTimeFormat.h"

#include "../database/database.hpp"
#include "../database/recsys_database.hpp"

#include "handlers/include/handlers/request_handler_factory.hpp"

#include <iostream>
#include <chrono>
#include <thread>

using ServerSocket = Poco::Net::ServerSocket;
using SocketAddress = Poco::Net::SocketAddress;
using HTTPServer = Poco::Net::HTTPServer;
using HTTPServerParams = Poco::Net::HTTPServerParams;
using DateTimeFormat = Poco::DateTimeFormat;

#define SAFE_TRY try {
#define SAFE_CATCH(error_while_prefix) \
    } catch (const Poco::Exception& e) { \
        std::cerr << error_while_prefix << e.displayText() << std::endl; \
        return EXIT_FAILURE;                               \
    } catch (const std::exception& e) {\
        std::cerr << error_while_prefix << e.what() << std::endl;        \
        return EXIT_FAILURE;           \
    }

namespace recsys_t2s {

    HTTPWebServer::HTTPWebServer() : m_IsHelpRequested(false) { }

    void HTTPWebServer::initialize( Poco::Util::Application& t_self ) {
        loadConfiguration();
        ServerApplication::initialize(t_self);
    }

    int HTTPWebServer::main( const std::vector<std::string>& t_args ) {

        /**
         * Для корректной инициализации требуется минимум 1 аргумент - путь к конфигурационному файлу.
         * Если он не задан, то возвращается код 1 и программа завершается.
         */
        if( t_args.empty()) {
            std::cerr <<
                "Error. Application required the service config path as first command line argument."
                << std::endl;
            return EXIT_FAILURE;
        }

        /**
         * Инициализация конфига из файла.
         */
        SAFE_TRY
            m_Config = std::make_shared<config::Config>(t_args.at(0));
        SAFE_CATCH("Error while loading config: ");


        /**
         * Попытка подключения к базе данных.
         * Конфигурируется в database config.
         * Выполняет несколько попыток подключения с заданным интервалом пока не подключится к БД.
         */
        SAFE_TRY

            DATABASE_INST->BindConfig(m_Config->GetDatabaseConfig());
            DATABASE_INST->TryConnect();
            if ( !DATABASE_INST->IsConnected() ) {
                std::cout << "Failed connect to database." << std::endl;
                return EXIT_FAILURE;
            }

        SAFE_CATCH("Error while trying connect to database: ");

        SAFE_TRY
            database::RecSysDatabase::Init();
        SAFE_CATCH("Error while init Recomendation System database: ");

        std::unique_ptr<ServerSocket> server_socket;
        std::unique_ptr<HTTPServer> server;
        SAFE_TRY
            server_socket = std::make_unique<ServerSocket>(
                        SocketAddress(
                                *m_Config->GetNetworkConfig()->GetIpAddress(),
                                *m_Config->GetNetworkConfig()->GetPort()
                        )
            );
            server = std::make_unique<HTTPServer>(
                    new handlers::RequestHandlerFactory(DateTimeFormat::SORTABLE_FORMAT),
                    *server_socket,
                    new HTTPServerParams);

        SAFE_CATCH("Error while init server: ")

        std::cout << "Application will started as " << server_socket->address().toString() << std::endl;

        server->start();
        waitForTerminationRequest();
        server->stop();

        return Application::EXIT_OK;
    }

} // namespace search_service