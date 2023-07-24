#include "student_indexer.hpp"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/DateTime.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

#define TABLE_NAME "StudentIndexes"
#define CREATE_TABLE_REQUEST \
    "CREATE TABLE IF NOT EXISTS `" TABLE_NAME "` "                     \
    "(`id` "                "INT "      "NOT NULL " "AUTO_INCREMENT, " \
    "`external_id` "        "INT "      "NOT NULL,"                    \
    "`vector` "             "BLOB "     "NOT NULL,"                    \
    "`add_datetime` "       "DATETIME " "DEFAULT CURRENT_TIMESTAMP, "  \
    "`last_update` "        "DATETIME " "DEFAULT CURRENT_TIMESTAMP, "  \
    "PRIMARY KEY (`id`), "                                               \
    "KEY `ext_id` (`external_id`)"                                     \
    ");"


#define SELECT_CHECK_BY_EXTERNAL_ID_REQUEST \
    "SELECT id FROM " TABLE_NAME " WHERE external_id=?"

#define INSERT_STUDENT_INDEX_REQUEST               \
    "INSERT INTO " TABLE_NAME " "            \
    "(external_id, vector) "                      \
    "VALUES(?, ?);"

#define UPDATE_STUDENT_INDEX_REQUEST               \
    "UPDATE " TABLE_NAME " SET "            \
    "external_id=?, "            \
    "vector=?, "                                    \
    "last_update=NOW() "                               \
    "WHERE external_id=?"

namespace functions {

    using student_t = recsys_t2s::database::Student;
    using index_vector_t = recsys_t2s::database::StudentIndex::index_vector_t;

    void ValidateBuildIndexInfo(const student_t& student) {

        if ( student.GetInstituteId() == recsys_t2s::common::InstituteID::None ) {
            throw std::runtime_error("Error while indexing: Student have None institute id.");
        }
        if ( student.GetEducationLevel() == recsys_t2s::common::EducationLevel::None ) {
            throw std::runtime_error("Error while indexing: Student have None education level.");
        }
        if ( student.GetEducationCourse() == recsys_t2s::common::EducationCourse::None ) {
            throw std::runtime_error("Error while indexing: Student have unknown course.");
        }
        if ( student.GetAge() < 0 ) {
            throw std::runtime_error("Error while indexing: Student have incorrect age.");
        }
        if ( student.GetIsHaveTeam() ) {
            throw std::runtime_error("Error while indexing: Student have a team.");
        }

    }

    /**
     * [0]  - Is student from 1 institute. [0 - 1]
     * [1]  - Is student from 2 institute. [0 - 1]
     * [2]  - Is student from 3 institute. [0 - 1]
     * [3]  - Is student from 4 institute. [0 - 1]
     * [4]  - Is student from 5 institute. [0 - 1]
     * [5]  - Is student from 6 institute. [0 - 1]
     * [6]  - Is student from 7 institute. [0 - 1]
     * [7]  - Is student from 8 institute. [0 - 1]
     * [8]  - Is student from 9 institute. [0 - 1]
     * [9]  - Is student from И10 institute. [0 - 1]
     * [10] - Is student from Т11 institute. [0 - 1]
     * [11] - Is student from Т12 institute. [0 - 1]
     * [12] - Is student from C institute. [0 - 1]
     * [13] - Is bacalauriat student [0 - 1]
     * [14] - Is specialist student [0 - 1]
     * [15] - Is magister student [0 - 1]
     * [16] - Student course [0 - 1]
     * [17] - Student age [0 - 1]
     * [18] - Is have it status [0 - 1]
     * @param student
     * @return
     */
    index_vector_t StudentToVector( const student_t& student ) {

        ValidateBuildIndexInfo(student);

        index_vector_t output_vector{ 0.f };

        constexpr auto institutes_count = static_cast<uint8_t>(recsys_t2s::common::InstituteID::Type::_count_);
        constexpr auto education_level_count = static_cast<uint8_t>(recsys_t2s::common::EducationLevel::_count_);

        const auto institute_index = static_cast<uint8_t>(student.GetInstituteId()) - 1;
        const auto education_level_index = institutes_count + static_cast<uint8_t>(student.GetEducationLevel()) - 2;

        constexpr auto student_course_index = education_level_count + institutes_count - 2;
        constexpr auto student_age_index = student_course_index + 1;
        constexpr auto student_it_status_index = student_age_index + 1;

        auto student_course = static_cast<int8_t>(student.GetEducationCourse());
        if ( student.GetEducationLevel() == recsys_t2s::common::EducationLevel::Magistracy ) {
            student_course += 4;
        }

        output_vector[ institute_index ] = 1.f;
        output_vector[ education_level_index ] = 1.f;
        output_vector[ student_course_index ] = static_cast<float>(student_course) / 6.f;
        output_vector[ student_age_index ] = static_cast<float>(student.GetAge()) / 100.f;
        output_vector[ student_it_status_index ] = static_cast<float>(student.GetIsItStudent());

        return output_vector;
    }

} // namespace functions

namespace recsys_t2s::database {

    DatabaseStatus StudentIndex::Init() {
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

    optional_with_status<common::ID> StudentIndex::SearchIndexByExternalID(const common::ID& external_id) {

        try {

            Session session = database::Database::Instance()->CreateSession();

            Statement select_statement(session);

            std::string ext_id_str = external_id.AsString();

            common::ID::id_type inner_id;
            select_statement << SELECT_CHECK_BY_EXTERNAL_ID_REQUEST, use(ext_id_str), into(inner_id);

            size_t selected_count = select_statement.execute();

            if ( selected_count != 0 ) {
                return std::make_pair(DatabaseStatus::OK, inner_id);
            }

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() }, std::make_optional<common::ID>());
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() }, std::make_optional<common::ID>());
        }

        return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_NOT_EXISTS, "Student index not exists." }, std::make_optional<common::ID>());
    }

    optional_with_status<StudentIndex> StudentIndex::AddStudentIndex(const recsys_t2s::database::Student &student) {

        StudentIndex index;
        try {
            {
                auto [status, index_id] = StudentIndex::SearchIndexByExternalID(student.GetExternalID());

                if (status == DatabaseStatus::OK) {
                    return UpdateStudentIndex(student);
                } else if ( status != DatabaseStatus::ERROR_NOT_EXISTS ) {
                    return std::make_pair(status, std::make_optional<StudentIndex>());
                }
            }

            index.m_Vector = functions::StudentToVector(student);
            index.m_ID = common::ID::None;

            auto* data = reinterpret_cast<unsigned char*>(index.m_Vector.data());
            Poco::Data::BLOB blob(data, index.m_Vector.size() * sizeof(float));

            std::string external_id = student.GetExternalID().AsString();
            int db_id = -1;

            Session session = Database::Instance()->CreateSession();

            Statement statement(session);
            statement << INSERT_STUDENT_INDEX_REQUEST, use(external_id), use(blob), into(db_id);

            size_t inserted_rows = statement.execute();
            index.m_ID = Database::SelectLastInsertedID();

            std::cout << "Inserted into index rows: " << inserted_rows << std::endl;
            std::cout << "Check ID after insert: " << index.m_ID << std::endl;

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() }, std::make_optional<StudentIndex>());
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() }, std::make_optional<StudentIndex>());
        }

        return std::make_pair(DatabaseStatus::OK, index);
    }

    optional_with_status<StudentIndex> StudentIndex::UpdateStudentIndex(const recsys_t2s::database::Student &student, bool is_guaranted_exists) {
        StudentIndex new_index;
        try {
            if ( !is_guaranted_exists ) {
                auto [status, index_id] = StudentIndex::SearchIndexByExternalID(student.GetExternalID());

                if (status != DatabaseStatus::OK) {
                    return std::make_pair(status, std::make_optional<StudentIndex>());
                }

            }

            new_index.m_Vector = functions::StudentToVector(student);
            new_index.m_ID = common::ID::None;

            auto* data = reinterpret_cast<unsigned char*>(new_index.m_Vector.data());
            Poco::Data::BLOB blob(data, new_index.m_Vector.size() * sizeof(float));

            std::string external_id = student.GetExternalID().AsString();

            Session session = Database::Instance()->CreateSession();

            Statement statement(session);
            statement << UPDATE_STUDENT_INDEX_REQUEST, use(external_id), use(blob), use(external_id);

            size_t inserted_rows = statement.execute();

            new_index.m_ID = Database::SelectLastInsertedID();

            std::cout << "Updated into index rows: " << inserted_rows << std::endl;
            std::cout << "Check ID after insert: " << new_index.m_ID << std::endl;

        } catch ( Poco::Data::MySQL::ConnectionException& e ) {
            std::cerr << "Connection to database error: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_CONNECTION, e.displayText() }, std::make_optional<StudentIndex>());
        } catch ( Poco::Data::MySQL::StatementException& e ) {
            std::cerr << "Statement exception while init to database: " << e.displayText() << std::endl;
            return std::make_pair(DatabaseStatus{ DatabaseStatus::ERROR_STATEMENT, e.displayText() }, std::make_optional<StudentIndex>());
        }

        return std::make_pair(DatabaseStatus::OK, new_index);
    }

} // namespace recsys_t2s::database