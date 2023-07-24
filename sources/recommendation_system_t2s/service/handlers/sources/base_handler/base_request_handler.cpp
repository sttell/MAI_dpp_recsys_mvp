#include "base_request_handler.hpp"

#include <Poco/JSON/Object.h>

#include <utility>

using RequestHandlerNs = recsys_t2s::handlers::IRequestHandler;

namespace recsys_t2s::handlers::impl {

    BaseRequestHandler::BaseRequestHandler(
            std::string format, recsys_t2s::handlers::HandlerType type, std::string instance_name
    ) : IRequestHandler(std::move(format), type, std::move(instance_name)) { /* Empty */ }

    void BaseRequestHandler::handleRequest(
            RequestHandlerNs::HTTPServerRequestBase&,
            RequestHandlerNs::HTTPServerResponseBase& t_response
    ) {
        SetBadRequestResponse(t_response, "Unrecognized request.");
    }

    /**
     * @brief Заполнение BadRequest(400) формы ответа
     * @param response HTML ответ для записи.
     * @param t_description - описание ошибки.
     */
    void BaseRequestHandler::SetBadRequestResponse(
            RequestHandlerNs::HTTPServerResponseBase& t_response,
            const std::string& t_description
    ) {
        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/bad_request");
        root->set("title", "Bad request error");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_BAD_REQUEST);
        root->set("detail", t_description);
        root->set("instance", this->Instance());
        std::ostream& ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    /**
     * @brief Заполнение Unauthorized(401) формы ответа
     * @param response HTML ответ для записи.
     * @param t_description - описание ошибки.
     */
    void BaseRequestHandler::SetUnauthorizedResponse(
            RequestHandlerNs::HTTPServerResponseBase& t_response,
            const std::string& t_description
    ) {
        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/unauthorized_error");
        root->set("title", "User unauthorized.");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_UNAUTHORIZED);
        root->set("detail", t_description);
        root->set("instance", this->Instance());

        std::ostream& ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    /**
     * @brief Заполнение Forbidden(403) формы ответа
     * @param response HTML ответ для записи.
     * @param t_description - описание ошибки.
     */
    void BaseRequestHandler::SetPermissionDeniedResponse(
            RequestHandlerNs::HTTPServerResponseBase& t_response,
            const std::string& t_description
    ) {
        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_FORBIDDEN);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/forbidden_error");
        root->set("title", "Permission denied.");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_FORBIDDEN);
        root->set("detail", t_description);
        root->set("instance", this->Instance());

        std::ostream& ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    /**
     * @brief Заполнение NotFound(404) формы ответа
     * @param t_response HTML ответ для записи.
     * @param t_description - описание ошибки.
     */
    void BaseRequestHandler::SetNotFoundResponse(
            RequestHandlerNs::HTTPServerResponseBase& t_response,
            const std::string& t_description
    ) {
        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_acceptable_error");
        root->set("title", "Not found error.");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_NOT_FOUND);
        root->set("detail", t_description);
        root->set("instance", this->Instance());

        std::ostream& ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    /**
     * @brief Заполнение NotAcceptable(406) формы ответа
     * @param t_response HTML ответ для записи.
     * @param t_description - описание ошибки.
     */
    void BaseRequestHandler::SetNotAcceptableResponse(
            RequestHandlerNs::HTTPServerResponseBase& t_response,
            const std::string& t_description
    ) {
        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_ACCEPTABLE);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_acceptable_error");
        root->set("title", "Not acceptable error.");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_NOT_ACCEPTABLE);
        root->set("detail", t_description);
        root->set("instance", this->Instance());

        std::ostream& ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }


    /**
     * @brief Заполнение InternalError(500) формы ответа
     * @param t_response HTML ответ для записи.
     * @param t_description - описание ошибки.
     */
    void BaseRequestHandler::SetInternalErrorResponse(
            RequestHandlerNs::HTTPServerResponseBase& t_response, const std::string& t_description
    ) {
        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/internal_error");
        root->set("title", "Internal Server Error");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_INTERNAL_SERVER_ERROR);
        root->set("detail", t_description);
        root->set("instance", this->Instance());
        std::ostream& ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    bool BaseRequestHandler::ValidateRequestHasField(
            HTMLForm& t_form,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response,
            const char* t_field_key
    ) {

        if ( !t_form.has(t_field_key) ) {
            SetBadRequestResponse(t_response, std::string{"Request require the field ["} + t_field_key + "]");
            return false;
        }

        return true;
    }

} // namespace recsys_t2s::handlers::impl