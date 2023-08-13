#ifndef RECOMMENDATION_SYSTEM_DATABASE_HPP
#define RECOMMENDATION_SYSTEM_DATABASE_HPP

#include <memory>

#include "common/id.hpp"
#include "common/optional_with_status.hpp"

#include "Poco/Data/Session.h"
#include "Poco/Data/SessionPool.h"

using Session = Poco::Data::Session;
using SessionPool = Poco::Data::SessionPool;

#define START_STATEMENT_SECTION try {

#define END_STATEMENT_SECTION_VOID \
} catch ( Poco::Data::MySQL::ConnectionException& e ) {                                                         \
    std::cerr << "Connection to database error: " << e.displayText() << std::endl;                              \
    return;                                 \
} catch ( Poco::Data::MySQL::StatementException& e ) {                                                          \
    std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;                \
    return;                                  \
}

#define END_STATEMENT_SECTION_WITH_STATUS \
} catch ( Poco::Data::MySQL::ConnectionException& e ) {                                                         \
    std::cerr << "Connection to database error: " << e.displayText() << std::endl;                              \
    return DatabaseStatus{DatabaseStatus::ERROR_CONNECTION, e.displayText()};                                 \
} catch ( Poco::Data::MySQL::StatementException& e ) {                                                          \
    std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;                \
    return DatabaseStatus{DatabaseStatus::ERROR_STATEMENT, e.displayText()};                                  \
}

#define END_STATEMENT_SECTION_WITH_OPT(return_type)                                                                      \
} catch ( Poco::Data::MySQL::ConnectionException& e ) {                                                         \
    std::cerr << "Connection to database error: " << e.displayText() << std::endl;                              \
    return OPT_WITH_STATUS_ERR(ERROR_CONNECTION, e.displayText(), return_type);                                 \
} catch ( Poco::Data::MySQL::StatementException& e ) {                                                          \
    std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;                \
    return OPT_WITH_STATUS_ERR(ERROR_STATEMENT, e.displayText(), return_type);                                  \
}

namespace recsys_t2s::config { class DatabaseConfig; }

namespace recsys_t2s::database {

    class Database final {
    private:
        constexpr Database() : m_IsConnected(false) {};

    public:

        static Database* Instance();

        void BindConfig(std::shared_ptr<config::DatabaseConfig> config);

        bool TryConnect();

        inline bool IsConnected() const noexcept { return m_IsConnected; }

        static common::ID SelectLastInsertedID();

        Session CreateSession();

    private:

        bool m_IsConnected;
        std::unique_ptr<SessionPool> m_Pool;
        std::shared_ptr<config::DatabaseConfig> m_Config;

    };

} // namespace recsys_t2s::database

#endif //RECOMMENDATION_SYSTEM_DATABASE_HPP
