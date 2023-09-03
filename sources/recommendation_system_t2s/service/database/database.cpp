#include "database.hpp"

#include "../config/server_config.hpp"

#include "Poco/Data/Transaction.h"
#include "Poco/Data/Binding.h"
#include "Poco/Data/MySQL/Connector.h"

#include <sstream>
#include <thread>
#include <chrono>

using Poco::Data::Keywords::use;
using Poco::Data::Keywords::into;
using Poco::Data::Keywords::range;

namespace recsys_t2s::database {

    Database* Database::Instance(){
        static Database* _instance = new Database();
        return _instance;
    }

    void Database::BindConfig( std::shared_ptr<config::DatabaseConfig> config ) {
        m_Config = std::move(config);
    }

    bool Database::TryConnect() {

        /* Инициализируем параметры которые отвечают за параметры логики подключения */
        const size_t max_try_count = *m_Config->GetTryConnectCount();
        const std::chrono::milliseconds connect_delay(*m_Config->GetTryConnectDelay());

        /* Инициализация строки отвечающей за подключение базы данных */
        std::stringstream connection_ss;
        connection_ss << "host=" << *m_Config->GetHostIP() << ";"
                      << "user=" << *m_Config->GetLogin() << ";"
                      << "db=" << *m_Config->GetDatabaseName() << ";"
                      << "port=" << *m_Config->GetPort() << ";"
                      << "password=" << *m_Config->GetPassword() << ";";
        const std::string connection_string_request = connection_ss.str();

        // Счетчик попыток подключения
        size_t try_counter{ 1 };

        std::cout << "Start trying connect to database with request: " << connection_string_request << std::endl;

        while ( try_counter <= max_try_count) {

            std::cout << "Try [ " << try_counter << " / " << max_try_count << "] connect to database..." << std::endl;

            /* Попытка подключиться к базе данных */
            try {

                Poco::Data::MySQL::Connector::registerConnector();
                m_Pool = std::make_unique<Poco::Data::SessionPool>(Poco::Data::MySQL::Connector::KEY,
                                                                   connection_string_request);
                if (m_Pool == nullptr) {
                    continue;
                }
                Poco::Data::Session test_session(m_Pool->get());

                /* В случае если не возникло исключений, считаем базу данных подключенной. Возвращаемся. */
                m_IsConnected = true;
                return true;
            } catch ( const Poco::Data::ConnectionFailedException& e ) {
                std::cerr << "Connection error: " << e.message() << " while try connect to " << *m_Config->GetHostIP() << ":" << *m_Config->GetPort() << std::endl;
            } catch ( const Poco::Exception& e) {
                std::cerr << "Poco exception while connect to database: " << e.displayText() << std::endl;
            } catch ( const std::exception& e ) {
                std::cerr << "Standard exception while connect to database: " << e.what() << std::endl;
            }

            /* Следующая попытка подключения к DB через connect_delay миллисекунд */
            try_counter++;
            std::this_thread::sleep_for(connect_delay);
        }

        /* Если мы так и не вышли из функции в рамках цикла, то истекли все попытки подключения к бд. */
        return false;
    }

    common::ID Database::SelectLastInsertedID() {
        int database_id{ common::ID::None };

        Session session = Database().Instance()->CreateSession();

        Poco::Data::Statement select(session);

        select << "SELECT LAST_INSERT_ID()", into(database_id), range(0, 1);
        if (!select.done()) {
            select.execute();
        }

        return {database_id};
    }

    Poco::Data::Session Database::CreateSession(){
        return Poco::Data::Session(m_Pool->get());
    }

} // namespace recsys_t2s::database