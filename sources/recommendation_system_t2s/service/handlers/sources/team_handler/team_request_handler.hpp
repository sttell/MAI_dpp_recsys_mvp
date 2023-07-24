#ifndef RECOMMENDATION_SYSTEM_TEAM_REQUEST_HANDLER_HPP
#define RECOMMENDATION_SYSTEM_TEAM_REQUEST_HANDLER_HPP

#include "handlers/sources/base_handler/base_request_handler.hpp"

#include <string>

namespace recsys_t2s::handlers::impl {

    class TeamRequestHandler final : public BaseRequestHandler {
    public:

        TeamRequestHandler( std::string format );

    public:

        void handleRequest( HTTPServerRequestBase& request, HTTPServerResponseBase& response ) override;

    private:
        void HandleGetRequest   ( HTTPServerRequestBase& request, HTTPServerResponseBase& response );
        void HandlePostRequest  ( HTTPServerRequestBase& request, HTTPServerResponseBase& response );
        void HandleDeleteRequest( HTTPServerRequestBase& request, HTTPServerResponseBase& response );
        void HandleUpdateRequest( HTTPServerRequestBase& request, HTTPServerResponseBase& response );

    };

} // namespace recsys_t2s::handlers::impl

#endif //RECOMMENDATION_SYSTEM_TEAM_REQUEST_HANDLER_HPP
