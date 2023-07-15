#ifndef RECOMMENDATION_SYSTEM_I_REQUEST_HANDLER_HPP
#define RECOMMENDATION_SYSTEM_I_REQUEST_HANDLER_HPP

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#include "handler_type.hpp"

namespace recsys_t2s::handlers {

namespace impl {

    using HTTPRequestHandlerBase = Poco::Net::HTTPRequestHandler;

} // namespace impl

    class IRequestHandler : public impl::HTTPRequestHandlerBase {

    protected:
        using HTTPServerRequestBase = Poco::Net::HTTPServerRequest;
        using HTTPServerResponseBase = Poco::Net::HTTPServerResponse;

    public:
        IRequestHandler(std::string format, HandlerType type, std::string instance_name) :
            format_(std::move(format)), type_(type), instance_(std::move(instance_name)) { /* Empty */ };

    public:

        void handleRequest(HTTPServerRequestBase &request, HTTPServerResponseBase &response) override = 0;

        [[nodiscard]] inline HandlerType GetType() const noexcept { return type_; };

    protected:

        std::string& Instance() noexcept { return instance_; };

        inline std::string& GetFormat() noexcept { return format_; };

        /* 400 */
        virtual void SetBadRequestResponse(HTTPServerResponseBase& response, const std::string& description) = 0;

        /* 401 */
        virtual void SetUnauthorizedResponse(HTTPServerResponseBase& response, const std::string& description) = 0;

        /* 403 */
        virtual void SetPermissionDeniedResponse(HTTPServerResponseBase& response, const std::string& description) = 0;

        /* 404 */
        virtual void SetNotFoundResponse(HTTPServerResponseBase& response, const std::string& description) = 0;

        /* 406 */
        virtual void SetNotAcceptableResponse(HTTPServerResponseBase& response, const std::string& description) = 0;

        /* 500 */
        virtual void SetInternalErrorResponse(HTTPServerResponseBase& response, const std::string& description) = 0;

    private:
        std::string format_;
        HandlerType type_;
        mutable std::string instance_;

    };

} // namespace recsys_t2s::handlers

#endif //RECOMMENDATION_SYSTEM_I_REQUEST_HANDLER_HPP
