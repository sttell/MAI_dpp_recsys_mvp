#ifndef RECOMMENDATION_SYSTEM_REQUEST_HANDLER_FACTORY_HPP
#define RECOMMENDATION_SYSTEM_REQUEST_HANDLER_FACTORY_HPP

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"

#include "handler_factory.hpp"

namespace recsys_t2s::handlers {

    namespace impl {

        using HTTPRequestHandlerFactoryBase = Poco::Net::HTTPRequestHandlerFactory;

    } // namespace impl

    class RequestHandlerFactory : public impl::HTTPRequestHandlerFactoryBase {

        using HTTPServerRequestBase = Poco::Net::HTTPServerRequest;
        using HTTPRequestHandlerBase = Poco::Net::HTTPRequestHandler;

    public:
        explicit RequestHandlerFactory( std::string format ) : m_Format(std::move(format)) { }

        HTTPRequestHandlerBase *createRequestHandler( const HTTPServerRequestBase& request ) override;

    private:
        std::string m_Format;
    };

} // namespace recsys_t2s

#endif //RECOMMENDATION_SYSTEM_REQUEST_HANDLER_FACTORY_HPP
