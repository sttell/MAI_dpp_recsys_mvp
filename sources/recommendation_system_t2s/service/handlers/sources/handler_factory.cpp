#include "handlers/include/handlers/handler_factory.hpp"

#include "base_handler/base_request_handler.hpp"

#include "../../common/errors.hpp"

#include <optional>
#include <map>
#include <iostream>

namespace {

    std::optional<recsys_t2s::handlers::HandlerType> GetTypeByURI( const std::string& URI ) {
        static const std::map<std::string, recsys_t2s::handlers::HandlerType> handlers_uri_map = {
                { "/team", recsys_t2s::handlers::HandlerType::Team },
                { "/student", recsys_t2s::handlers::HandlerType::Student }
        };

        for( const auto& [ handler_uri, handler_type ]: handlers_uri_map ) {
            if( URI.find(handler_uri) != std::string::npos ) {
                return handler_type;
            }
        }
        return std::make_optional<recsys_t2s::handlers::HandlerType>();
    }

} // namespace [functions]

namespace recsys_t2s::handlers {

    IRequestHandler *HandlerFactory::Create( const std::string& t_format, const std::string& t_request_URI ) {

        std::cout << "Request: " << t_format << " " << t_request_URI << std::endl;

        const std::optional<HandlerType> type = GetTypeByURI(t_request_URI);

        if( !type.has_value()) {
            throw common::exceptions::BadURI("Failed to create request handler by URI: " + t_request_URI);
        }

        switch( type.value()) {
            case HandlerType::Student:
            case HandlerType::Team:
            case HandlerType::Base:
            case HandlerType::_count_: { /* Pass */ }
        }

        return new impl::BaseRequestHandler(t_format, HandlerType::Base, "base_handler");
    }

} // namespace handler