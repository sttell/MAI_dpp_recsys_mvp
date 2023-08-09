#include "student_request_handler.hpp"

#include "database/student_indexer.hpp"
#include "database/student.hpp"

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
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase&,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response
    ) {
        BaseRequestHandler::SetInternalErrorResponse(t_response, "GET Not implemented");
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
        auto opt_institute = form.getValue<std::string>("institute");
        auto opt_education_level = form.getValue<std::string>("education_level");
        auto opt_academic_group = form.getValue<std::string>("academic_group");
        auto opt_education_course = form.getValue<int8_t>("education_course");
        auto opt_age = form.getValue<unsigned int>("age");
        auto opt_it_status = form.getValue<bool>("it_status");

        if ( !opt_student_id.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized student id data type."); return; }
        if ( !opt_program_id.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized program id data type."); return; }
        if ( !opt_institute.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized institute data type."); return; }
        if ( !opt_education_level.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized education level data type."); return; }
        if ( !opt_academic_group.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized academic group data type."); return; }
        if ( !opt_education_course.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized education course data type."); return; }
        if ( !opt_age.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized age data type."); return; }
        if ( !opt_it_status.has_value() ) { SetBadRequestResponse(t_response, "Invalid data: unrecognized it status data type."); return; }

        database::StudentIndex student_indexer;

        student_indexer.SetExternalID(opt_student_id.value());
        student_indexer.SetProgramID(opt_program_id.value());
        student_indexer.SetInstituteId(opt_institute.value());
        student_indexer.SetEducationLevel(opt_education_level.value());
        student_indexer.SetAcademicGroup(opt_academic_group.value());
        student_indexer.SetEducationCourse(opt_education_course.value());
        student_indexer.SetAge(static_cast<int>(opt_age.value()));
        student_indexer.SetIsItStudent(opt_it_status.value());

        database::Student student;
        student.SetExternalID(opt_student_id.value());
        student.SetProgramID(opt_program_id.value());
        student.SetTeamID(common::ID::None);
        student.SetIsHaveTeam(false);

        auto status = student.InsertToDatabase(student_indexer);

        if ( status != database::DatabaseStatus::OK ) {
            BaseRequestHandler::SetBadRequestResponse( t_response, status.GetMessage() );
            return;
        }

        t_response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        t_response.setChunkedTransferEncoding(true);
        t_response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/success");
        root->set("title", "OK");
        root->set("status", Poco::Net::HTTPResponse::HTTP_REASON_OK);
        root->set("instance", "/user");
        root->set("id", student.GetExternalID().AsString());
        std::ostream &ostr = t_response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

    void StudentRequestHandler::HandleDeleteRequest(
            recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase&,
            recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase &t_response) {
        BaseRequestHandler::SetInternalErrorResponse(t_response, "DELETE Not implemented");
    }

    void
    StudentRequestHandler::HandleUpdateRequest(recsys_t2s::handlers::IRequestHandler::HTTPServerRequestBase&,
                                               recsys_t2s::handlers::IRequestHandler::HTTPServerResponseBase& t_response) {

        BaseRequestHandler::SetInternalErrorResponse(t_response, "UPDATE Not implemented");

    }

} // namespace recsys_t2s::handlers::impl