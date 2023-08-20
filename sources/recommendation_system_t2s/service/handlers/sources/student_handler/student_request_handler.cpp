#include "student_request_handler.hpp"

#include "database/student.hpp"
#include "database/recsys_database.hpp"

namespace recsys_t2s::handlers::impl {

    StudentRequestHandler::StudentRequestHandler(std::string format) :
            BaseRequestHandler(std::move(format), HandlerType::Team, std::string{"student_handler"}) { /* Empty */ }


    void StudentRequestHandler::handleRequest(recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
                                              recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response) {

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

    void StudentRequestHandler::HandleGetRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        HTMLForm form(t_request);

        if (!ValidateRequestHasField(form, t_response, "student_id")) return;

        auto opt_student_id = form.getValue<unsigned int>("student_id");

        if ( !opt_student_id.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized student id data type.");


        auto [search_student_status, ranged_ids] = database::RecSysDatabase::GetRecommendationListForStudent(opt_student_id.value());
        if ( search_student_status != database::DatabaseStatus::OK )
            HANDLER_RETURN_BAD_REQUEST(t_response, search_student_status.GetMessage());

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

    void StudentRequestHandler::HandlePostRequest(recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
                                                  recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response) {

        HTMLForm form(t_request);

        if (!ValidateRequestHasField(form, t_response, "student_id")) return;
        if (!ValidateRequestHasField(form, t_response, "program_id")) return;
        if (!ValidateRequestHasField(form, t_response, "institute")) return;
        if (!ValidateRequestHasField(form, t_response, "education_level")) return;
        if (!ValidateRequestHasField(form, t_response, "academic_group")) return;
        if (!ValidateRequestHasField(form, t_response, "education_course")) return;
        if (!ValidateRequestHasField(form, t_response, "age")) return;
        if (!ValidateRequestHasField(form, t_response, "it_status")) return;

        auto opt_student_id = form.getValue<unsigned int>("student_id");
        auto opt_program_id = form.getValue<unsigned int>("program_id");
        auto opt_team_id = form.getValue<int>("team_id");
        auto opt_institute = form.getValue<std::string>("institute");
        auto opt_education_level = form.getValue<std::string>("education_level");
        auto opt_academic_group = form.getValue<std::string>("academic_group");
        auto opt_education_course = form.getValue<int8_t>("education_course");
        auto opt_age = form.getValue<unsigned int>("age");
        auto opt_it_status = form.getValue<bool>("it_status");

        if ( !opt_student_id.has_value() ) HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized student id data type.");
        if ( !opt_program_id.has_value() ) HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized program id data type.");
        if ( !opt_institute.has_value() ) HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized institute data type.");
        if ( !opt_education_level.has_value() ) HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized education level data type.");
        if ( !opt_academic_group.has_value() ) HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized academic group data type.");
        if ( !opt_education_course.has_value() ) HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized education course data type.");
        if ( !opt_age.has_value() ) HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized age data type.");
        if ( !opt_it_status.has_value() ) HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized it status data type.");
        if ( !opt_team_id.has_value() && form.has("team_id") )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized team id data type.");

        database::Student student;
        student.SetExternalID(opt_student_id.value());
        student.SetProgramID(opt_program_id.value());
        student.SetInstituteId(opt_institute.value());
        student.SetEducationLevel(opt_education_level.value());
        student.SetAcademicGroup(opt_academic_group.value());
        student.SetEducationCourse(opt_education_course.value());
        student.SetAge(static_cast<int>(opt_age.value()));
        student.SetIsItStudent(opt_it_status.value());
        if ( opt_team_id.has_value() )
            student.SetTeamID(opt_team_id.value());

        auto status = database::RecSysDatabase::InsertStudent(student);
        if ( status != database::DatabaseStatus::OK )
            HANDLER_RETURN_BAD_REQUEST(t_response, status.GetMessage());

        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/success");
        root->set("title", "OK");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_OK);
        root->set("instance", "/user");
        std::ostream &ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    void StudentRequestHandler::HandleDeleteRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response) {

        HTMLForm form(t_request);

        if (!ValidateRequestHasField(form, t_response, "student_id")) return;
        auto opt_student_id = form.getValue<unsigned int>("student_id");
        if ( !opt_student_id.has_value() )
            HANDLER_RETURN_BAD_REQUEST(t_response, "Invalid data: unrecognized student id data type.");


        auto status = database::RecSysDatabase::DeleteStudentByExternalID(common::ID(opt_student_id.value()));
        if ( status != database::DatabaseStatus::OK )
            HANDLER_RETURN_BAD_REQUEST(t_response, status.GetMessage());

        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/success");
        root->set("title", "OK");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_OK);
        root->set("instance", "/user");
        std::ostream &ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);

    }

    void
    StudentRequestHandler::HandleUpdateRequest(recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase& t_request,
                                               recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response) {

        HTMLForm form(t_request);

        if (!ValidateRequestHasField(form, t_response, "student_id")) return;

        auto opt_student_id = form.getValue<unsigned int>("student_id");
        auto opt_program_id = form.getValue<unsigned int>("program_id");
        auto opt_institute = form.getValue<std::string>("institute");
        auto opt_team_id = form.getValue<int>("team_id");
        auto opt_education_level = form.getValue<std::string>("education_level");
        auto opt_academic_group = form.getValue<std::string>("academic_group");
        auto opt_education_course = form.getValue<int8_t>("education_course");
        auto opt_age = form.getValue<unsigned int>("age");
        auto opt_it_status = form.getValue<bool>("it_status");

        if ( !opt_student_id.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized student id data type."); return; }

        database::Student student;
        student.SetExternalID(opt_student_id.value());

        if ( opt_program_id.has_value() ) student.SetProgramID(opt_program_id.value());
        if ( opt_institute.has_value() ) student.SetInstituteId(opt_institute.value());
        if ( opt_education_level.has_value() ) student.SetEducationLevel(opt_education_level.value());
        if ( opt_academic_group.has_value() ) student.SetAcademicGroup(opt_academic_group.value());
        if ( opt_education_course.has_value() ) student.SetEducationCourse(opt_education_course.value());
        if ( opt_age.has_value() ) student.SetAge(static_cast<int>(opt_age.value()));
        if ( opt_it_status.has_value() ) student.SetIsItStudent(opt_it_status.value());
        if ( opt_team_id.has_value() ) student.SetTeamID(opt_team_id.value());

        std::map<std::string, bool> fields_in_form;
        fields_in_form["student_id"] = form.has("student_id");
        fields_in_form["program_id"] = form.has("program_id");
        fields_in_form["institute"] = form.has("institute");
        fields_in_form["team_id"] = form.has("team_id");
        fields_in_form["education_level"] = form.has("education_level");
        fields_in_form["academic_group"] = form.has("academic_group");
        fields_in_form["education_course"] = form.has("education_course");
        fields_in_form["age"] = form.has("age");
        fields_in_form["it_status"] = form.has("it_status");

        auto status = database::RecSysDatabase::UpdateStudent(student, fields_in_form);
        if ( status != database::DatabaseStatus::OK )
            HANDLER_RETURN_BAD_REQUEST(t_response, status.GetMessage());

        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/success");
        root->set("title", "OK");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_OK);
        root->set("instance", "/user");
        std::ostream &ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);

    }

} // namespace recsys_t2s::handlers::impl