#include "team_request_handler.hpp"

#include "Poco/JSON/Array.h"

#include <map>
#include <iostream>

namespace recsys_t2s::handlers::impl {

    TeamRequestHandler::TeamRequestHandler(std::string format) :
        BaseRequestHandler(std::move(format), HandlerType::Team, std::string{"team_handler"}) { /* Empty */ }

    void TeamRequestHandler::handleRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        try {
            if( t_request.getMethod() == HTTPServerRequestBase::HTTP_GET ) {
                HandleGetRequest(t_request, t_response);
                return;
            }
            if( t_request.getMethod() == HTTPServerRequestBase::HTTP_POST ) {
                HandlePostRequest(t_request, t_response);
                return;
            }
            if( t_request.getMethod() == HTTPServerRequestBase::HTTP_PATCH ) {
                HandleUpdateRequest(t_request, t_response);
                return;
            }
            if( t_request.getMethod() == HTTPServerRequestBase::HTTP_DELETE ) {
                HandleDeleteRequest(t_request, t_response);
                return;
            }

            SetBadRequestResponse(t_response, "Unrecognized request method.");
        } catch ( const Poco::Exception& e ) {

            BaseRequestHandler::SetInternalErrorResponse(
                    t_response,
                    std::string{"Unrecognized exception: "} + e.displayText()
            );

        } catch ( const std::exception& e ) {
            BaseRequestHandler::SetInternalErrorResponse(
                    t_response,
                    std::string{"Unrecognized exception: "} + e.what()
            );
        }
    }

    void TeamRequestHandler::HandleGetRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase&,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        BaseRequestHandler::SetInternalErrorResponse(t_response, "GET Not implemented");
    }

    void TeamRequestHandler::HandlePostRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {

        HTMLForm form(t_request);

        /* Check if form has valid fields */
        if( !ValidateRequestHasField(form, t_response, "team_id") ) return;
        if( !ValidateRequestHasField(form, t_response, "students") ) return;

        auto opt_team_id = form.getValue<unsigned int>("team_id");
        auto opt_students = form.getValue<Poco::JSON::Array>("students");

        if ( !opt_team_id.has_value() ) {
            BaseRequestHandler::SetBadRequestResponse(t_response, "Invalid data: unrecognized team ID data type.");
            return;
        }
        if ( !opt_students.has_value() ) {
            BaseRequestHandler::SetBadRequestResponse(t_response, "Invalid data: students field must be json array with unsigned integer identifiers.");
            return;
        }

        std::vector<unsigned long> students_id{};
        std::size_t students_count = opt_students.value().size();
        students_id.reserve(students_count);

        for ( std::size_t i = 0; i < students_count; ++i ) {
            auto student_id = opt_students.value().get(i);
            const std::string& str_value = student_id.toString();

            const bool is_integer = student_id.isInteger();
            if ( !is_integer ) {
                SetBadRequestResponse(
                        t_response,
                        "Invalid data: unrecognized student ID data type in students array."
                );
                return;
            }

            students_id.push_back(static_cast<unsigned long>(utils::StringToUnsignedLongLong(str_value)));

        }

        unsigned int team_id = opt_team_id.value();

        for ( [[maybe_unused]] const auto& student_id : students_id ) {
            // Check if student registred in system
        }
        // TODO: Implement
        // Is team id registred in system? If yes, then you can set 406 error.


        std::cout << "Add request: " << std::endl;
        std::cout << "\tTeam ID : " << team_id << std::endl;
        std::cout << "\tStudents: ";
        for ( auto& item : students_id ) {
            std::cout << item << ", ";
        }
        std::cout << std::endl;

        BaseRequestHandler::SetInternalErrorResponse(t_response, "POST Not implemented");
    }

    void TeamRequestHandler::HandleUpdateRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase&,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        BaseRequestHandler::SetInternalErrorResponse(t_response, "PATCH Not implemented");
    }

    void TeamRequestHandler::HandleDeleteRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase&,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        BaseRequestHandler::SetInternalErrorResponse(t_response, "DELETE Not implemented");
    }

} // namespace recsys_t2s::handlers::impl