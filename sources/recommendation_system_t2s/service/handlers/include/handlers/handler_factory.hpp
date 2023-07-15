#ifndef RECOMMENDATION_SYSTEM_HANDLER_FACTORY_HPP
#define RECOMMENDATION_SYSTEM_HANDLER_FACTORY_HPP

#include "i_request_handler.hpp"

namespace recsys_t2s::handlers {

    class HandlerFactory {
    public:
        static IRequestHandler* Create( const std::string& format, const std::string& request_URI );
    };

} // namespace recsys_t2s::handlers

#endif //RECOMMENDATION_SYSTEM_HANDLER_FACTORY_HPP
