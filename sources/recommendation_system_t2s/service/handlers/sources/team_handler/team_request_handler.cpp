#include "team_request_handler.hpp"

#include "Poco/JSON/Array.h"

#include "database/recsys_database.hpp"

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
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        HTMLForm form(t_request);

        if (!ValidateRequestHasField(form, t_response, "team_id")) return;

        auto opt_team_id = form.getValue<unsigned int>("team_id");

        if ( !opt_team_id.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized team id data type.");


        auto [recommendation_status, ranged_ids] = database::RecSysDatabase::GetRecommendationListForTeam(opt_team_id.value());
        if ( recommendation_status != database::DatabaseStatus::OK )
            HANDLER_RETURN_BAD_REQUEST(t_response, recommendation_status.GetMessage());

        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");

        Poco::JSON::Array array_of_ids;
        for( const auto& id : ranged_ids.value() )
            array_of_ids.add(static_cast<int>(id));

        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/success");
        root->set("title", "OK");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_OK);
        root->set("instance", "/user");
        root->set("teams", array_of_ids);
        std::ostream &ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    void TeamRequestHandler::HandlePostRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {

        HTMLForm form(t_request);

        /* Check if form has valid fields */
        if( !ValidateRequestHasField(form, t_response, "team_id") ) return;
        if( !ValidateRequestHasField(form, t_response, "team_lead_id") ) return;
        if( !ValidateRequestHasField(form, t_response, "students") ) return;

        auto opt_team_id = form.getValue<unsigned int>("team_id");
        auto opt_team_lead_id = form.getValue<unsigned int>("team_lead_id");
        auto opt_students = form.getValue<Poco::JSON::Array>("students");

        if ( !opt_team_id.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized team ID data type.")

        if ( !opt_team_lead_id.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized team lead ID data type.")

        if ( !opt_students.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: students field must be json array with unsigned integer identifiers.")

        /* Convert JSON Array student IDs to vector of common::ID */
        std::set<common::ID> students_id{};

        for ( std::size_t i = 0; i < opt_students.value().size(); ++i ) {
            auto student_id = opt_students.value().get(i);
            const std::string& str_value = student_id.toString();

            const bool is_integer = student_id.isInteger();

            if ( !is_integer )
                HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized student ID data type in students array.")

            students_id.insert(utils::StringToUnsignedLongLong(str_value));

        }

        unsigned int team_id = opt_team_id.value();
        unsigned int team_lead_id = opt_team_lead_id.value();

        students_id.insert(team_lead_id);

        database::Team team;
        team.SetExternalID(team_id);
        team.SetTeamLeadID(team_lead_id);
        team.SetStudentExternalIndices(students_id);

        auto status = database::RecSysDatabase::InsertTeam(team);
        if ( status != database::DatabaseStatus::OK )
            HANDLER_RETURN_BAD_REQUEST(t_response, status.GetMessage())

        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/success");
        root->set("title", "OK");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_OK);
        root->set("instance", "/team");
        std::ostream &ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    void TeamRequestHandler::HandleUpdateRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        HTMLForm form(t_request);

        /* Check if form has valid fields */
        if( !ValidateRequestHasField(form, t_response, "team_id") ) return;

        auto opt_team_id = form.getValue<unsigned int>("team_id");
        auto opt_team_lead_id = form.getValue<unsigned int>("team_lead_id");

        if ( !opt_team_id.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized team ID data type.")

        if ( form.has("team_id") && !opt_team_lead_id.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized team lead ID data type.")

        std::map<std::string, bool> fields_in_form;
        fields_in_form["team_id"] = form.has("team_id");
        fields_in_form["team_lead_id"] = form.has("team_lead_id");

        database::Team team;
        team.SetExternalID(opt_team_id.value());
        if ( form.has("team_lead_id") )
            team.SetTeamLeadID(opt_team_lead_id.value());

        auto load_students_status = team.LoadStudents(true);
        if ( load_students_status != database::DatabaseStatus::OK )
            HANDLER_RETURN_BAD_REQUEST(t_response, load_students_status.GetMessage());

        auto status = database::RecSysDatabase::UpdateTeam(team, fields_in_form);
        if ( status != database::DatabaseStatus::OK && status != database::DatabaseStatus::NOTHING_TO_DO )
            HANDLER_RETURN_BAD_REQUEST(t_response, status.GetMessage())

        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/success");
        root->set("title", "OK");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_OK);
        root->set("instance", "/team");
        std::ostream &ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    void TeamRequestHandler::HandleDeleteRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        HTMLForm form(t_request);

        /* Check if form has valid fields */
        if( !ValidateRequestHasField(form, t_response, "team_id") ) return;

        auto opt_team_id = form.getValue<unsigned int>("team_id");

        if ( !opt_team_id.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized team ID data type.")

        unsigned int team_id = opt_team_id.value();

        auto status = database::RecSysDatabase::DeleteTeamByExternalID(team_id);
        if ( status != database::DatabaseStatus::OK )
            HANDLER_RETURN_BAD_REQUEST(t_response, status.GetMessage())

        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/success");
        root->set("title", "OK");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_OK);
        root->set("instance", "/team");
        std::ostream &ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

} // namespace recsys_t2s::handlers::impl