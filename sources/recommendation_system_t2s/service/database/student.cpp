#include "student.hpp"

#include "database.hpp"
#include "recsys_database.hpp"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/Transaction.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/String.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Data::Transaction;

#define TABLE_NAME "Students"
#define SELECT_REQUEST_PREFIX \
    "SELECT " \
    "external_id, program_id, descriptor, " \
    "institute_id, education_level, education_course, academic_group, age, is_it_student " \
    "FROM " TABLE_NAME " "

#define SELECT_BY_EXTERNAL_ID_REQUEST SELECT_REQUEST_PREFIX "WHERE external_id=?"

#define UPDATE_STUDENT_REQUEST \
    "UPDATE " TABLE_NAME " SET " \
    "program_id=?, descriptor=?, "\
    "institute_id=?, education_level=?, education_course=?, academic_group=?, age=?, is_it_student=?, " \
    "last_update_datetime=NOW() "\
    "WHERE external_id=?"

namespace functions {

    using student_t = recsys_t2s::database::Student;

    std::pair<bool, std::optional<std::string>>
    ValidateBuildIndexInfo(const student_t& student) {

        if ( student.GetInstituteId() == recsys_t2s::common::InstituteID::None ) {
            return std::make_pair(false, "Error while indexing: Student have None institute id.");
        }
        if ( student.GetEducationLevel() == recsys_t2s::common::EducationLevel::None ) {
            return std::make_pair(false, "Error while indexing: Student have None education level.");
        }
        if ( student.GetEducationCourse() == recsys_t2s::common::EducationCourse::None ) {
            return std::make_pair(false, "Error while indexing: Student have unknown course.");
        }
        if ( student.GetAge() < 0 ) {
            return std::make_pair(false, "Error while indexing: Student have incorrect age.");
        }

        return std::make_pair(true, std::make_optional<std::string>());
    }

} // namespace functions

namespace recsys_t2s::database {


    optional_with_status<common::ID> Student::UpdateStudent(recsys_t2s::database::Student &updated) {

        /* Поиск текущего студента. Существует ли он в базе. */
        auto [status, opt_exists_student] = RecSysDatabase::SearchStudentByExternalID(updated.m_ExternalID);
        if ( status != DatabaseStatus::OK ) {
            return OPT_WITH_STATUS(status, std::make_optional<common::ID>());
        } else if ( !opt_exists_student.has_value() ) {
            std::string message{ "Student with ID " + updated.m_ExternalID.AsString() + " not exists." };
            return OPT_WITH_STATUS_ERR(ERROR_NOT_EXISTS, message, common::ID);
        }

        /* Подсчет обновленного дескриптора */
        auto [ is_created, opt_message ] = updated.UpdateDescriptor();
        if ( !is_created ) {
            return OPT_WITH_STATUS_ERR(ERROR_CREATE_DESCRIPTOR, opt_message.value(), common::ID);
        }

        /* Statement execution */
        START_STATEMENT_SECTION

            Session session = database::Database::Instance()->CreateSession();
            Statement statement(session);

            Descriptor descriptor = updated.GetDescriptor();
            auto* data = reinterpret_cast<unsigned char*>(descriptor.data());
            Poco::Data::BLOB in_descriptor(data, DESCRIPTOR_BYTES);

            std::string in_external_id = updated.m_ExternalID.AsString();
            std::string in_program_id = updated.m_ProgramID.AsString();
            std::string in_institute_id = updated.m_InstituteId.AsString();
            std::string in_education_level = updated.m_EducationLevel.AsString();
            std::string in_education_course = updated.m_EducationCourse.AsString();
            std::string in_academic_group = updated.m_AcademicGroup;
            std::string in_age = std::to_string(updated.m_Age);
            std::string in_is_it = std::to_string(updated.m_IsItStudent);

            statement << UPDATE_STUDENT_REQUEST,
                         use(in_program_id),
                         use(in_descriptor),
                         use(in_institute_id),
                         use(in_education_level),
                         use(in_education_course),
                         use(in_academic_group),
                         use(in_age),
                         use(in_is_it),
                         use(in_external_id);

            statement.execute();

        END_STATEMENT_SECTION_WITH_OPT(common::ID)

        return OPT_WITH_STATUS_OK(updated.m_ExternalID);
    }

    std::pair<bool, std::optional<std::string>> Student::UpdateDescriptor() const {

        auto [is_valid, message] = functions::ValidateBuildIndexInfo(*this);
        if ( !is_valid ) {
            return std::make_pair(is_valid, message);
        }

        constexpr auto institutes_count = static_cast<uint8_t>(recsys_t2s::common::InstituteID::Type::_count_);

        const auto institute_index = static_cast<uint8_t>(GetInstituteId()) - 1;
        const auto education_level_index = institutes_count + static_cast<uint8_t>(GetEducationLevel()) - 2;

        auto student_course = static_cast<int8_t>(GetEducationCourse());
        if ( GetEducationLevel() == recsys_t2s::common::EducationLevel::Magistracy ) {
            student_course += 4;
        }

        m_Descriptor[ DESCRIPTOR_INSTITUTES_FIELDS_START_ID + institute_index ]       = 1.f;
        m_Descriptor[ DESCRIPTOR_GRADUATE_FIELDS_START_ID   + education_level_index ] = 1.f;

        m_Descriptor[ DescriptorField::STUDENT_COURSE ] = static_cast<DescriptorType>(student_course) / 6.f;
        m_Descriptor[ DescriptorField::STUDENT_AGE ]    = static_cast<DescriptorType>(GetAge()) / 100.f;
        m_Descriptor[ DescriptorField::STUDENT_IS_IT ]  = static_cast<DescriptorType>(GetIsItStudent());

        return std::make_pair(true, std::make_optional<std::string>());
    }

} // namespace recsys_t2s::database