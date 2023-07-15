#include "database.hpp"

#include "../config/server_config.hpp"

#include "Poco/Data/Transaction.h"
#include "Poco/Data/Binding.h"
#include "Poco/Data/MySQL/Connector.h"

#include <sstream>

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
        try {

            std::string connection_string{};
            connection_string += "host="     + *m_Config->GetHostIP()               + ";";
            connection_string += "user="     + *m_Config->GetLogin()                + ";";
            connection_string += "db="       + *m_Config->GetDatabaseName()         + ";";
            connection_string += "port="     + std::to_string(*m_Config->GetPort()) + ";";
            connection_string += "password=" + *m_Config->GetPassword()             + ";";

            std::cout << "Try connect to database. Connection request:\n\t" << connection_string << std::endl;
            Poco::Data::MySQL::Connector::registerConnector();
            m_Pool = std::make_unique<Poco::Data::SessionPool>(Poco::Data::MySQL::Connector::KEY, connection_string);
            m_IsConnected = true;
            return true;

        } catch ( const std::exception& e ) {
            std::cerr << "Error connect to database: " << e.what() << std::endl;
        }

        return false;
    }

    Poco::Data::Session Database::CreateSession(){
        return Poco::Data::Session(m_Pool->get());
    }

} // namespace recsys_t2s::database