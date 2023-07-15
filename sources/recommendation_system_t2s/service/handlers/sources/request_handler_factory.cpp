#include "handlers/request_handler_factory.hpp"

#include "../../common/errors.hpp"

#include <iostream>

namespace recsys_t2s::handlers {

    RequestHandlerFactory::HTTPRequestHandlerBase*
    RequestHandlerFactory::createRequestHandler(const RequestHandlerFactory::HTTPServerRequestBase& t_request) {

        try {
            return HandlerFactory::Create(m_Format, t_request.getURI());
        } catch ( const common::exceptions::BadURI& e ) {
            std::cerr << "Failed request handler create: " << e.what() << std::endl;
        }

        return nullptr;

    }

} // namespace recsys_t2s::handlers