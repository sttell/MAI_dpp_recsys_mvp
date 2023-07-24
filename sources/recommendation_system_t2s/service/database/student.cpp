#include "student.hpp"

#include "database.hpp"
#include "database/student_indexer.hpp"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/Transaction.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Data::Transaction;


#define TABLE_NAME "Students"
#define CREATE_TABLE_REQUEST                                              \
    "CREATE TABLE IF NOT EXISTS `" TABLE_NAME "` "                        \
    "(`id` "                "INT "         "NOT NULL " "AUTO_INCREMENT,"  \
    "`external_id` "        "INT "         "NOT NULL,"                    \
    "`program_id` "         "INT "         "NOT NULL,"                    \
    "`team_id` "            "INT "         "NOT NULL,"                    \
    "`index_id` "           "INT "         "NOT NULL,"                    \
    "`academic_group` "     "VARCHAR(32) " "NOT NULL,"                    \
    "`institute_id` "       "TINYINT "     "NOT NULL,"                    \
    "`education_level_id` " "TINYINT "     "NOT NULL,"                    \
    "`education_course` "   "TINYINT "     "NOT NULL,"                    \
    "`age` "                "TINYINT "     "NOT NULL,"                    \
    "`is_have_team` "       "BOOLEAN "     "NOT NULL,"                    \
    "`is_it` "              "BOOLEAN "     "NOT NULL,"                    \
    "`add_datetime` "       "DATETIME "    "DEFAULT CURRENT_TIMESTAMP, "  \
    "`last_update` "        "DATETIME "    "DEFAULT CURRENT_TIMESTAMP, "  \
    "PRIMARY KEY (`id`), "                                                \
    "KEY `ext_id` (`external_id`),"                                     \
    "KEY `tm_id`  (`team_id`),"                                         \
    "KEY `idx_id` (`index_id`)"                                         \
    ");"

#define ALL_FIELDS \
        "external_id, program_id, team_id, index_id, academic_group, institute_id, education_level_id, education_course, age, is_have_team, is_it"

#define SELECT_CHECK_BY_EXTERNAL_ID_REQUEST \
    "SELECT id FROM " TABLE_NAME " WHERE external_id=?"

#define SELECT_BY_ID_REQUEST \
    "SELECT " ALL_FIELDS " FROM " TABLE_NAME " WHERE id=?"

#define SELECT_BY_EXTERNAL_ID_REQUEST \
    "SELECT " ALL_FIELDS " FROM " TABLE_NAME " WHERE external_id=?"

#define DELETE_BY_EXTERNAL_ID_REQUEST \
    "DELETE FROM " TABLE_NAME " WHERE external_id=? RETURNING id"

#define INSERT_STUDENT_REQUEST               \
    "INSERT INTO " TABLE_NAME " "            \
    "(" ALL_FIELDS ") "                      \
    "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

#define UPDATE_STUDENT_REQUEST      \
    "UPDATE " TABLE_NAME " SET "    \
    "external_id=? "                \
    "program_id=? "                 \
    "team_id=? "                    \
    "index_id=? "                   \
    "academic_group=? "             \
    "institute_id=? "               \
    "education_level_id=? "         \
    "education_course=? "           \
    "age=? "                        \
    "is_have_team=? "               \
    "is_it=? WHERE external_id=?"

namespace recsys_t2s::database {

    DatabaseStatus Student::Init() {
        try {

            Session session = database::Database::Instance()->CreateSession();

            Statement create_statement(session);
            create_statement << CREATE_TABLE_REQUEST, now;

            create_statement.execute();

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() };
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() };
        }

        return DatabaseStatus{ DatabaseStatus::OK };
    }

    DatabaseStatus Student::InsertToDatabase() {

        {
            auto [status, is_exists] = Student::CheckIfExistsByExternalID(m_ExternalID);

            if ( status != DatabaseStatus::OK ) {
                return status;
            }

            if ( is_exists.value() ) {
                std::string message{ "Student with ID " };
                message += std::to_string(m_ExternalID) + " exists.";
                return DatabaseStatus{ DatabaseStatus::ERROR_OBJECT_EXISTS, message };
            }
        }




        /* Add index to database */
        // TODO: Add transaction
        StudentIndex student_index;
        {
            auto [status, opt_student_index] = database::StudentIndex::AddStudentIndex(*this);

            if (status != DatabaseStatus::OK) {
                return status;
            }

            student_index = opt_student_index.value();
        }

        this->m_IndexID = student_index.GetID();

        try {
            Session session = Database::Instance()->CreateSession();
            Statement insert_statement(session);

            std::string ext_id_str = m_ExternalID.AsString();
            std::string program_id_str = m_ProgramID.AsString();
            std::string team_id_str = m_TeamID.AsString();
            std::string index_id_str = m_IndexID.AsString();
            std::string academic_group = m_AcademicGroup;
            std::string institute_id_str = m_InstituteId.AsString();
            std::string education_level_str = m_EducationLevel.AsString();
            std::string education_course_str = m_EducationCourse.AsString();
            std::string age = std::to_string(m_Age);
            std::string is_have_team_str = std::to_string(m_IsHaveTeam);
            std::string is_it_str = std::to_string(m_IsItStudent);

            insert_statement << std::string(INSERT_STUDENT_REQUEST),
                                use(ext_id_str),
                                use(program_id_str),
                                use(team_id_str),
                                use(index_id_str),
                                use(academic_group),
                                use(institute_id_str),
                                use(education_level_str),
                                use(education_course_str),
                                use(age),
                                use(is_have_team_str),
                                use(is_it_str);

            insert_statement.execute();
            m_ID = Database::SelectLastInsertedID();
            
        } catch ( Poco::Data::MySQL::StatementException& e ) {

            std::cerr << "Statement Error while insert student: " << e.displayText() << std::endl;
            return DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() };

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {

            std::cerr << "Connection Error while insert student: " << e.displayText() << std::endl;
            return DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() };

        }

        return DatabaseStatus{ DatabaseStatus::OK };

    }

    optional_with_status<bool> Student::CheckIfExistsByExternalID(const common::ID& external_id) {

        try {

            Session session = database::Database::Instance()->CreateSession();

            Statement select_statement(session);

            std::string ext_id_str = external_id.AsString();

            common::ID::id_type inner_id;
            select_statement << SELECT_CHECK_BY_EXTERNAL_ID_REQUEST, use(ext_id_str), into(inner_id), now;

            size_t selected_count = select_statement.execute();

            if ( selected_count == 0 ) {
                return std::make_pair(DatabaseStatus::OK, false);
            }

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() }, false);
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() }, false);
        }

        return std::make_pair(DatabaseStatus::OK, true);

    }

    optional_with_status<Student> Student::SearchByID(const common::ID& id) {

        Student student;

        try {

            Session session = database::Database::Instance()->CreateSession();

            Statement select_statement(session);

            std::string id_str = id.AsString();

            common::ID::id_type external_id;
            common::ID::id_type program_id;
            common::ID::id_type team_id;
            common::ID::id_type index_id;
            common::ID::id_type institute_id;
            common::ID::id_type education_level_id;
            int8_t education_course;
            int32_t age;
            bool is_have_team;
            bool is_it;

            select_statement << SELECT_BY_ID_REQUEST, 
                                use(id_str),
                                into(external_id),
                                into(program_id),
                                into(team_id),
                                into(index_id),
                                into(institute_id),
                                into(education_level_id),
                                into(education_course),
                                into(age),
                                into(is_have_team),
                                into(is_it),
                                now;

            size_t selected_count = select_statement.execute();

            if ( selected_count == 0 ) {
                return std::make_pair(DatabaseStatus::OK, std::make_optional<Student>());
            }

            student.m_ExternalID = external_id;
            student.m_ProgramID = program_id;
            student.m_TeamID = team_id;
            student.m_IndexID = index_id;
            student.m_InstituteId = institute_id;
            student.m_EducationLevel = static_cast<common::EducationLevel::Type>(education_level_id);
            student.m_EducationCourse = education_course;
            student.m_Age = age;
            student.m_IsHaveTeam = is_have_team;
            student.m_IsItStudent = is_it;

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() }, std::make_optional<Student>());
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() }, std::make_optional<Student>());
        }

        return std::make_pair(DatabaseStatus::OK, student);
    }

    optional_with_status<Student> Student::SearchByExternalID(const common::ID& t_external_id) {
        Student student;

        try {

            Session session = database::Database::Instance()->CreateSession();

            Statement select_statement(session);

            std::string ext_id_str = t_external_id.AsString();

            common::ID::id_type external_id;
            common::ID::id_type program_id;
            common::ID::id_type team_id;
            common::ID::id_type index_id;
            common::ID::id_type institute_id;
            common::ID::id_type education_level_id;
            int8_t education_course;
            int32_t age;
            bool is_have_team;
            bool is_it;

            select_statement << SELECT_BY_EXTERNAL_ID_REQUEST,
                                use(ext_id_str),
                                into(external_id),
                                into(program_id),
                                into(team_id),
                                into(index_id),
                                into(institute_id),
                                into(education_level_id),
                                into(education_course),
                                into(age),
                                into(is_have_team),
                                into(is_it),
                                now;

            size_t selected_count = select_statement.execute();

            if ( selected_count == 0 ) {
                return std::make_pair(DatabaseStatus::OK, std::make_optional<Student>());
            }

            student.m_ExternalID = external_id;
            student.m_ProgramID = program_id;
            student.m_TeamID = team_id;
            student.m_IndexID = index_id;
            student.m_InstituteId = institute_id;
            student.m_EducationLevel = static_cast<common::EducationLevel::Type>(education_level_id);
            student.m_EducationCourse = education_course;
            student.m_Age = age;
            student.m_IsHaveTeam = is_have_team;
            student.m_IsItStudent = is_it;

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() }, std::make_optional<Student>());
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() }, std::make_optional<Student>());
        }

        return std::make_pair(DatabaseStatus::OK, student);
    }

    optional_with_status<common::ID> Student::DeleteByExternalID(const common::ID& t_external_id) {
        Student student;

        common::ID::id_type deleted_id;
        try {

            Session session = database::Database::Instance()->CreateSession();

            Statement select_statement(session);

            std::string ext_id_str = t_external_id.AsString();
            select_statement << DELETE_BY_EXTERNAL_ID_REQUEST, use(ext_id_str), into(deleted_id), now;
            size_t selected_count = select_statement.execute();

            if ( selected_count == 0 || deleted_id == common::ID::None ) {
                return std::make_pair(DatabaseStatus::OK, std::make_optional<common::ID>());
            }

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() }, std::make_optional<common::ID>());
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() }, std::make_optional<common::ID>());
        }

        return std::make_pair(DatabaseStatus::OK, deleted_id);
    }

    optional_with_status<common::ID> Student::UpdateStudent(const recsys_t2s::database::Student &updated) {

        auto [status, exists_student] = Student::SearchByExternalID(updated.m_ExternalID);
        if ( status != DatabaseStatus::OK  ) {
            return std::make_pair(status, std::make_optional<common::ID>());
        } else if ( !exists_student.has_value() ) {
            return std::make_pair(DatabaseStatus::ERROR_NOT_EXISTS, std::make_optional<common::ID>());
        }

        try {

            Session session = database::Database::Instance()->CreateSession();

            Statement update_statement(session);

            std::string ext_id_str = updated.m_ExternalID.AsString();

            common::ID::id_type external_id;
            common::ID::id_type program_id;
            common::ID::id_type team_id;
            common::ID::id_type index_id;
            common::ID::id_type institute_id;
            common::ID::id_type education_level_id;
            int8_t education_course;
            int32_t age;
            bool is_have_team;
            bool is_it;

            update_statement << UPDATE_STUDENT_REQUEST,
                                use(ext_id_str),
                                into(external_id),
                                into(program_id),
                                into(team_id),
                                into(index_id),
                                into(institute_id),
                                into(education_level_id),
                                into(education_course),
                                into(age),
                                into(is_have_team),
                                into(is_it),
                                now;

            size_t selected_count = update_statement.execute();

            if ( selected_count == 0 ) {
                return std::make_pair(DatabaseStatus::OK, std::make_optional<common::ID>());
            }

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() }, std::make_optional<common::ID>());
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() }, std::make_optional<common::ID>());
        }

        return std::make_pair(DatabaseStatus::OK, std::make_optional<common::ID>(updated.m_ExternalID));
    }

} // namespace recsys_t2s::database