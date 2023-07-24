#ifndef RECOMMENDATION_SYSTEM_BASE_REQUEST_HANDLER_HPP
#define RECOMMENDATION_SYSTEM_BASE_REQUEST_HANDLER_HPP

#include "handlers/include/handlers/i_request_handler.hpp"
#include "handlers/sources/html_form/html_form.hpp"

namespace recsys_t2s::handlers::impl {

    class BaseRequestHandler : public IRequestHandler {
    public:

        BaseRequestHandler( std::string format, HandlerType type, std::string instance_name );

    public:

        void handleRequest( HTTPServerRequestBase& request, HTTPServerResponseBase& response ) override;

    protected:

        /* 400 */
        void SetBadRequestResponse( HTTPServerResponseBase& response, const std::string& description ) override;

        /* 401 */
        void SetUnauthorizedResponse( HTTPServerResponseBase& response, const std::string& description ) override;

        /* 403 */
        void SetPermissionDeniedResponse( HTTPServerResponseBase& response, const std::string& description ) override;

        /* 404 */
        void SetNotFoundResponse( HTTPServerResponseBase& response, const std::string& description ) override;

        /* 406 */
        void SetNotAcceptableResponse( HTTPServerResponseBase& response, const std::string& description ) override;

        /* 500 */
        void SetInternalErrorResponse( HTTPServerResponseBase& response, const std::string& description ) override;

        bool ValidateRequestHasField(
                HTMLForm& form, HTTPServerResponseBase& response, const char* field_key
        );

    };

} // namespace recsys_t2s::handlers::impl

#endif //RECOMMENDATION_SYSTEM_BASE_REQUEST_HANDLER_HPP
