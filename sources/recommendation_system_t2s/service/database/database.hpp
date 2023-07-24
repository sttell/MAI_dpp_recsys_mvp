#ifndef RECOMMENDATION_SYSTEM_DATABASE_HPP
#define RECOMMENDATION_SYSTEM_DATABASE_HPP

#include <memory>

#include "common/id.hpp"

#include "Poco/Data/Session.h"
#include "Poco/Data/SessionPool.h"

using Session = Poco::Data::Session;
using SessionPool = Poco::Data::SessionPool;

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
