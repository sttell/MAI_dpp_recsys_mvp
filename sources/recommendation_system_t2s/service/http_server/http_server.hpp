#ifndef RECOMMENDATION_SYSTEM_HTTP_SERVER_HPP
#define RECOMMENDATION_SYSTEM_HTTP_SERVER_HPP

#include "Poco/Util/ServerApplication.h"

#include "config/server_config.hpp"

namespace recsys_t2s {

    namespace impl {

        using ServerApplicationBase = Poco::Util::ServerApplication;

    } // namespace impl

    class HTTPWebServer : public impl::ServerApplicationBase {
    public:
        HTTPWebServer();

        ~HTTPWebServer() override = default;

    protected:
        void initialize( Application& t_self ) override;

        int main( [[maybe_unused]] const std::vector<std::string>& t_args ) override;

    private:
        bool m_IsHelpRequested{ false };
        std::shared_ptr<config::Config> m_Config{ nullptr };
    };

} // namespace search_service


#endif //RECOMMENDATION_SYSTEM_HTTP_SERVER_HPP
