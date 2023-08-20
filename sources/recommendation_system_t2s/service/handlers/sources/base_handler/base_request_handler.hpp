#ifndef RECOMMENDATION_SYSTEM_BASE_REQUEST_HANDLER_HPP
#define RECOMMENDATION_SYSTEM_BASE_REQUEST_HANDLER_HPP

#include "handlers/include/handlers/i_request_handler.hpp"
#include "handlers/sources/html_form/html_form.hpp"

#define _HANDLER_RETURN_REQUEST_IMPL_CALL(response_setter_function, response_obj, message) { \
    response_setter_function(response_obj, message);                                         \
    return;                                                                                  \
}

#define HANDLER_RETURN_BAD_REQUEST(response_obj, message) { _HANDLER_RETURN_REQUEST_IMPL_CALL(SetBadRequestResponse, response_obj, message) }
#define HANDLER_RETURN_UNAUTHORIZED(response_obj, message) { _HANDLER_RETURN_REQUEST_IMPL_CALL(SetUnauthorizedResponse, response_obj, message) }
#define HANDLER_RETURN_PERMISSION_DENIED(response_obj, message) { _HANDLER_RETURN_REQUEST_IMPL_CALL(SetPermissionDeniedResponse, response_obj, message) }
#define HANDLER_RETURN_NOT_FOUND(response_obj, message) { _HANDLER_RETURN_REQUEST_IMPL_CALL(SetNotFoundResponse, response_obj, message) }
#define HANDLER_RETURN_NOT_ACCEPTABLE(response_obj, message) { _HANDLER_RETURN_REQUEST_IMPL_CALL(SetNotAcceptableResponse, response_obj, message) }
#define HANDLER_RETURN_INTERNAL_ERROR(response_obj, message) { _HANDLER_RETURN_REQUEST_IMPL_CALL(SetInternalErrorResponse, response_obj, message) }

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
