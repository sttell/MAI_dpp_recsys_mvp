#include "recsys_database.hpp"

#include <sstream>
#include <iostream>

#include "database.hpp"

#include <Poco/Data/Transaction.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Transaction;

#define STUDENTS_TABLE_NAME "Students"
#define CREATE_STUDENTS_TABLE_REQUEST                                              \
    "CREATE TABLE IF NOT EXISTS `" STUDENTS_TABLE_NAME "` "                        \
    "(`id` "                  "INT "         "NOT NULL " "AUTO_INCREMENT,"  \
    "`external_id` "          "INT "         "NOT NULL,"                    \
    "`program_id` "           "INT "         "NOT NULL,"                    \
    "`team_id` "              "INT "         "NOT NULL,"                                                                               \
    "`descriptor` "           "TEXT "        "NOT NULL,"                    \
    "`institute_id` "         "INT "         "NOT NULL,"                                                                      \
    "`education_level` "      "INT "         "NOT NULL,"                                                                      \
    "`education_course` "     "INT "         "NOT NULL,"                                                                      \
    "`academic_group` "       "CHAR(32) "    "NOT NULL,"    \
    "`age` "                  "INT "         "NOT NULL,"  \
    "`is_it_student` "        "INT "         "NOT NULL,"    \
    "`add_datetime` "         "DATETIME "    "DEFAULT CURRENT_TIMESTAMP, "  \
    "`last_update_datetime` " "DATETIME "    "DEFAULT CURRENT_TIMESTAMP, "  \
    "PRIMARY KEY (`id`), "                                                \
    "KEY `ext_id` (`external_id`)"                                     \
    ");"

#define TEAMS_TABLE_NAME "Teams"
#define CREATE_TEAM_TABLE_REQUEST                                              \
    "CREATE TABLE IF NOT EXISTS `" TEAMS_TABLE_NAME "` "                        \
    "(`id` "                    "INT "         "NOT NULL " "AUTO_INCREMENT,"  \
    "`external_id` "            "INT "         "NOT NULL,"                    \
    "`team_lead_id` "           "INT "         "NOT NULL,"                    \
    "`descriptor` "             "TEXT "        "NOT NULL,"                               \
    "`add_datetime` "           "DATETIME "    "DEFAULT CURRENT_TIMESTAMP, "  \
    "`last_update_datetime` "   "DATETIME "    "DEFAULT CURRENT_TIMESTAMP, "  \
    "PRIMARY KEY (`id`), "                                                \
    "KEY `ext_id` (`external_id`)"                                     \
    ");"

#define INSERT_STUDENT_REQUEST               \
    "INSERT INTO " STUDENTS_TABLE_NAME " "            \
    "(external_id, program_id, team_id, descriptor, institute_id, education_level, education_course, academic_group, age, is_it_student) "                      \
    "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)"

#define SELECT_CHECK_STUDENT_BY_EXTERNAL_ID_REQUEST \
    "SELECT id "\
    "FROM " STUDENTS_TABLE_NAME " " \
    "WHERE external_id=?"

#define SELECT_CHECK_TEAM_BY_EXTERNAL_ID_REQUEST \
    "SELECT id "\
    "FROM " TEAMS_TABLE_NAME " " \
    "WHERE external_id=?"

#define SELECT_TEAM_BY_EXTERNAL_ID_REQUEST \
    "SELECT id, team_lead_id, descriptor "\
    "FROM " TEAMS_TABLE_NAME " " \
    "WHERE external_id=?"

#define SELECT_STUDENT_BY_EXTERNAL_ID_REQUEST \
    "SELECT id, program_id, team_id, descriptor, institute_id, education_level, education_course, academic_group, age, is_it_student "\
    "FROM " STUDENTS_TABLE_NAME " " \
    "WHERE external_id=?"

#define UPDATE_TEAM_BY_EXTERNAL_ID_REQUEST \
    "UPDATE " TEAMS_TABLE_NAME " SET "     \
    "team_lead_id=?, last_update_datetime=NOW() " \
    "WHERE external_id=?"

namespace recsys_t2s::database {

    DatabaseStatus RecSysDatabase::Init() {
        START_STATEMENT_SECTION
        Session session = database::Database::Instance()->CreateSession();

        Statement create_statement(session);
        create_statement << CREATE_STUDENTS_TABLE_REQUEST, now;

        create_statement.execute();

        END_STATEMENT_SECTION_WITH_STATUS

        START_STATEMENT_SECTION
            Session session = database::Database::Instance()->CreateSession();

            Statement create_statement(session);
            create_statement << CREATE_TEAM_TABLE_REQUEST, now;

            create_statement.execute();

        END_STATEMENT_SECTION_WITH_STATUS

        return DatabaseStatus{ DatabaseStatus::OK };
    }

    optional_with_status<bool> RecSysDatabase::CheckIfExistsStudentByExternalID(const common::ID &id) {

        START_STATEMENT_SECTION

            Session session = database::Database::Instance()->CreateSession();

            Statement select_statement(session);

            std::string in_external_id = id.AsString();
            common::ID::id_type out_inner_id;

            select_statement << SELECT_CHECK_STUDENT_BY_EXTERNAL_ID_REQUEST, into(out_inner_id), use(in_external_id);

            size_t selected_count = select_statement.execute();

            if ( selected_count == 0 )
                return OPT_WITH_STATUS_OK(false);

        END_STATEMENT_SECTION_WITH_OPT(bool);

        return OPT_WITH_STATUS_OK(true);

    }

    optional_with_status<Team> RecSysDatabase::SearchTeamByExternalID(const common::ID &id) {

        Team team;
        START_STATEMENT_SECTION

            Session session = database::Database::Instance()->CreateSession();
            Statement select_statement(session);

            std::string in_external_id = id.AsString();

            common::ID::id_type out_inner_id;
            common::ID::id_type out_team_lead_id;
            std::string out_descriptor_blob;

            select_statement << SELECT_TEAM_BY_EXTERNAL_ID_REQUEST,
                                into(out_inner_id),
                                into(out_team_lead_id),
                                into(out_descriptor_blob),
                                use(in_external_id);

            size_t selected_count = select_statement.execute();
            if ( selected_count == 0 )
                return OPT_WITH_STATUS_ERR(ERROR_NOT_EXISTS, "Team with ID " + in_external_id + " not exists.", Team);

            team.SetID(out_inner_id);
            team.SetExternalID(id);
            team.SetTeamLeadID(out_team_lead_id);
            team.SetDescriptor(HexBlobToDescriptor(out_descriptor_blob));

        END_STATEMENT_SECTION_WITH_OPT(Team)

            Session session = database::Database::Instance()->CreateSession();
            Statement select_statement(session);

            std::set<common::ID> student_indices;

            std::string in_team_id = team.GetExternalID().AsString();
            common::ID::id_type out_id;

            select_statement << "SELECT external_id FROM " STUDENTS_TABLE_NAME " WHERE team_id=?",
                                into(out_id),
                                use(in_team_id),
                                range(0, 1);

            while( !select_statement.done() ) {
                if ( select_statement.execute() ) {
                    student_indices.insert(out_id);
                }
            }

            team.SetStudentExternalIndices(student_indices);
            DatabaseStatus load_students_status = team.LoadStudents();

            if ( load_students_status != DatabaseStatus::OK )
                return OPT_WITH_STATUS(load_students_status, std::nullopt);

            auto [is_updated, reason] = team.UpdateDescriptor();
            if ( !is_updated )
                return OPT_WITH_STATUS_ERR(ERROR_CREATE_DESCRIPTOR, reason.value(), Team);

        START_STATEMENT_SECTION

        END_STATEMENT_SECTION_WITH_OPT(Team)

        return OPT_WITH_STATUS_OK(team);
    }

    DatabaseStatus RecSysDatabase::InsertStudent(const recsys_t2s::database::Student &student) {
        {
            auto [status, is_exists] = RecSysDatabase::CheckIfExistsStudentByExternalID(student.GetExternalID());

            if ( status != DatabaseStatus::OK )
                return status;

            if ( is_exists.value() )
                return {
                    DatabaseStatus::ERROR_NOT_EXISTS,
                    "Student with ID " + student.GetExternalID().AsString() + " exists."
                };

        }

        auto [ is_created, opt_message ] = student.UpdateDescriptor();
        if (!is_created)
            return {DatabaseStatus::ERROR_CREATE_DESCRIPTOR, opt_message.value()};

        std::optional<Team> opt_student_team;
        if ( student.GetTeamID() != common::ID::None ) {
            auto [status, opt_team] = RecSysDatabase::SearchTeamByExternalID(student.GetTeamID());

            if ( status == DatabaseStatus::OK ) {
                opt_student_team = opt_team;
            } else if ( status != DatabaseStatus::ERROR_NOT_EXISTS ) {
                return status;
            }
        }
        if ( opt_student_team.has_value() ) {
            opt_student_team->AppendExistsStudent(student.GetExternalID(), student.GetDescriptor());
        }

        common::ID inserted_id;
        START_STATEMENT_SECTION

            Session session = Database::Instance()->CreateSession();
            Transaction transaction(session);

            std::string in_external_id = student.GetExternalID().AsString();
            std::string in_program_id = student.GetProgramID().AsString();
            std::string in_team_id = student.GetTeamID().AsString();
            std::string in_descriptor = DescriptorToHexBlob(student.GetDescriptor());
            std::string in_institute_id = student.GetInstituteId().AsString();
            std::string in_education_level = student.GetEducationLevel().AsString();
            std::string in_education_course = student.GetEducationCourse().AsString();
            std::string in_academic_group = student.GetAcademicGroup().GetValue();
            std::string in_age = std::to_string(student.GetAge().GetValue());
            std::string in_is_it = std::to_string(student.GetIsItStudent().GetValue());

            std::string insert_student_statement{ "INSERT INTO " STUDENTS_TABLE_NAME " (external_id, program_id, team_id, descriptor, institute_id, education_level, education_course, academic_group, age, is_it_student) " };
            insert_student_statement += "VALUES(" + in_external_id + ", ";
            insert_student_statement += in_program_id + ", ";
            insert_student_statement += in_team_id + ", ";
            insert_student_statement += in_descriptor + ", ";
            insert_student_statement += in_institute_id + ", ";
            insert_student_statement += in_education_level + ", ";
            insert_student_statement += in_education_course + ", ";
            insert_student_statement += "\"" + in_academic_group + "\", ";
            insert_student_statement += in_age + ", ";
            insert_student_statement += in_is_it + ")";

            transaction.execute(insert_student_statement, false);

            if ( opt_student_team.has_value() ) {

                std::string in_team_descriptor = DescriptorToHexBlob(opt_student_team->GetDescriptor());

                std::string update_team_statement{ "UPDATE " TEAMS_TABLE_NAME " SET " };
                update_team_statement += "descriptor=" + in_team_descriptor + ", ";
                update_team_statement += "last_update_datetime=NOW() WHERE external_id=" + student.GetTeamID().AsString();

                transaction.execute(update_team_statement);

            }

            transaction.commit();

        END_STATEMENT_SECTION_WITH_STATUS

        return DatabaseStatus::OK;
    }

    optional_with_status<bool> RecSysDatabase::CheckIfExistsTeamByExternalID(const common::ID &id) {
        START_STATEMENT_SECTION

            Session session = database::Database::Instance()->CreateSession();

            Statement select_statement(session);

            std::string in_external_id = id.AsString();
            common::ID::id_type out_inner_id;

            select_statement << SELECT_CHECK_TEAM_BY_EXTERNAL_ID_REQUEST, into(out_inner_id), use(in_external_id);

            size_t selected_count = select_statement.execute();

            if ( selected_count == 0 )
                return OPT_WITH_STATUS_OK(false);

        END_STATEMENT_SECTION_WITH_OPT(bool);

        return OPT_WITH_STATUS_OK(true);
    }

    DatabaseStatus RecSysDatabase::InsertTeam(const recsys_t2s::database::Team &team) {

        DatabaseStatus load_students_status = team.LoadStudents();
        if ( load_students_status != DatabaseStatus::OK )
            return load_students_status;


        auto [ is_created, opt_message ] = team.UpdateDescriptor();
        if (!is_created)
            return {
                DatabaseStatus::ERROR_CREATE_DESCRIPTOR,
                opt_message.value()
            };

        common::ID inserted_id;
        START_STATEMENT_SECTION

            Session session = Database::Instance()->CreateSession();
            Transaction transaction(session);

            std::string in_external_id = team.GetExternalID().AsString();
            std::string in_team_lead_id = team.GetTeamLeadID().AsString();
            std::string in_descriptor = DescriptorToHexBlob(team.GetDescriptor());


            std::string insert_team_statement{ "INSERT INTO " TEAMS_TABLE_NAME " (external_id, team_lead_id, descriptor) " };
            insert_team_statement += "VALUES(";
            insert_team_statement += in_external_id + ", ";
            insert_team_statement += in_team_lead_id + ", ";
            insert_team_statement += in_descriptor + ")";

            transaction.execute(insert_team_statement, false);

            for ( auto& student : team.GetStudents() ) {

                std::string update_student_statement{ "UPDATE " STUDENTS_TABLE_NAME " SET " };
                update_student_statement += "team_id=" + team.GetExternalID().AsString() + ", last_update_datetime=NOW() ";
                update_student_statement += "WHERE external_id=" + student.GetExternalID().AsString();

                transaction.execute(update_student_statement);
            }

            transaction.commit();

        END_STATEMENT_SECTION_WITH_STATUS

        return DatabaseStatus::OK;

    }

    optional_with_status<Student> RecSysDatabase::SearchStudentByExternalID(const common::ID &id) {

        Student student;

        START_STATEMENT_SECTION

            Session session = Database::Instance()->CreateSession();
            Statement statement(session);

            std::string in_external_id = id.AsString();

            common::ID::id_type out_id;
            common::ID::id_type out_program_id;
            common::ID::id_type out_team_id;
            std::string out_descriptor;
            common::ID::id_type out_institute_id;
            int out_education_level_id;
            int out_education_course_id;
            std::string out_academic_group;
            int out_age;
            bool out_is_it_student;

            statement << SELECT_STUDENT_BY_EXTERNAL_ID_REQUEST,
                         into(out_id),
                         into(out_program_id),
                         into(out_team_id),
                         into(out_descriptor),
                         into(out_institute_id),
                         into(out_education_level_id),
                         into(out_education_course_id),
                         into(out_academic_group),
                         into(out_age),
                         into(out_is_it_student),
                         use(in_external_id);

            size_t selected_cnt = statement.execute();

            if ( selected_cnt == 0 ) {
                return OPT_WITH_STATUS_ERR(
                        ERROR_NOT_EXISTS,
                        "Student with ID " + id.AsString() + " not exists.",
                        Student
                );
            }

            student.SetID(out_id);
            student.SetExternalID(id);
            student.SetProgramID(out_program_id);
            student.SetTeamID(out_team_id);
            student.SetDescriptor(HexBlobToDescriptor(out_descriptor));
            student.SetInstituteId(out_institute_id);
            student.SetEducationLevel(out_education_level_id);
            student.SetEducationCourse(out_education_course_id);
            student.SetAcademicGroup(out_academic_group);
            student.SetAge(out_age);
            student.SetIsItStudent(out_is_it_student);

        END_STATEMENT_SECTION_WITH_OPT(Student)

        return OPT_WITH_STATUS_OK(student);
    }

    DatabaseStatus RecSysDatabase::DeleteStudentByExternalID(const common::ID &id) {

        auto [ search_student_status, opt_student ] = SearchStudentByExternalID(id);
        if ( search_student_status != DatabaseStatus::OK )
            return search_student_status;

        std::optional<Team> opt_team;
        if ( opt_student->GetTeamID() != common::ID::None ) {
            auto [ search_team_status, opt_ret_team ] = SearchTeamByExternalID(opt_student->GetTeamID());
            if ( search_team_status == DatabaseStatus::ERROR_NOT_EXISTS ) {
                opt_student->SetTeamID(common::ID::None);
            } else if ( search_team_status != DatabaseStatus::OK )
                return search_team_status;

            opt_team = opt_ret_team;
        }

        common::ID deleted_id;
        START_STATEMENT_SECTION

            Session session = database::Database::Instance()->CreateSession();
            Transaction transaction(session);

            std::string delete_statement = "DELETE FROM " STUDENTS_TABLE_NAME " WHERE external_id=" + id.AsString();
            transaction.execute(delete_statement, false);

            if ( opt_team.has_value() && opt_team->GetTeamLeadID() == id) {
                opt_team->RemoveExistsStudent(opt_student->GetExternalID(), opt_student->GetDescriptor());

                std::string update_team_lead_statement = "UPDATE " TEAMS_TABLE_NAME " SET team_lead_id=";
                update_team_lead_statement += std::to_string(common::ID::None) + ", last_update_datetime=NOW() ";
                update_team_lead_statement += "WHERE external_id=" + opt_team->GetExternalID().AsString();

                transaction.execute(update_team_lead_statement, false);
            }
            transaction.commit();

            deleted_id = opt_student->GetID();

        END_STATEMENT_SECTION_WITH_STATUS

        return DatabaseStatus::OK;
    }
    DatabaseStatus RecSysDatabase::DeleteTeamByExternalID(const common::ID &id) {
        auto [search_team_status, opt_team] = SearchTeamByExternalID(id);
        if ( search_team_status != DatabaseStatus::OK )
            return search_team_status;

        START_STATEMENT_SECTION

            Session session = Database::Instance()->CreateSession();
            Transaction transaction(session);

            std::string delete_team_request = "DELETE FROM " TEAMS_TABLE_NAME " WHERE external_id=" + id.AsString();
            transaction.execute(delete_team_request, false);

            std::string change_student_team_request = "UPDATE " STUDENTS_TABLE_NAME " SET ";
            change_student_team_request += "team_id=" + std::to_string(common::ID::None) + ", last_update_datetime=NOW() ";
            change_student_team_request += "WHERE team_id=" + opt_team->GetExternalID().AsString();
            transaction.execute(change_student_team_request, false);

            transaction.commit();

        END_STATEMENT_SECTION_WITH_STATUS

        return DatabaseStatus::OK;
    }

    DatabaseStatus RecSysDatabase::UpdateTeam(recsys_t2s::database::Team new_team) {

        auto [ search_team_status, is_exists ] = CheckIfExistsTeamByExternalID(new_team.GetExternalID());
        if ( search_team_status != DatabaseStatus::OK ) {
            return search_team_status;
        }
        if ( !is_exists.value() ) {
            return {
                DatabaseStatus::ERROR_NOT_EXISTS,
                "Team with ID " + new_team.GetExternalID().AsString() + " not exists.",
            };
        }

        common::ID updated_id;
        START_STATEMENT_SECTION

            Session session = Database::Instance()->CreateSession();
            Statement statement(session);

            std::string team_lead_id = new_team.GetTeamLeadID().AsString();
            std::string external_id  = new_team.GetExternalID().AsString();

            statement << UPDATE_TEAM_BY_EXTERNAL_ID_REQUEST,
                         use(team_lead_id),
                         use(external_id);

            statement.execute();

        END_STATEMENT_SECTION_WITH_STATUS

        return DatabaseStatus::OK;
    }

    DatabaseStatus
    RecSysDatabase::UpdateStudent(recsys_t2s::database::Student new_student, const std::map<std::string, bool>& fields_in_form) {

        auto [ search_status, opt_student ] = SearchStudentByExternalID(new_student.GetExternalID());
        if ( search_status != DatabaseStatus::OK )
            return search_status;

        if ( !fields_in_form.count("program_id") ) new_student.SetProgramID(opt_student->GetProgramID());
        if ( !fields_in_form.count("team_id") ) new_student.SetTeamID(opt_student->GetTeamID());
        if ( !fields_in_form.count("institute") ) new_student.SetInstituteId(opt_student->GetInstituteId());
        if ( !fields_in_form.count("education_course") ) new_student.SetEducationCourse(opt_student->GetEducationCourse());
        if ( !fields_in_form.count("education_level") ) new_student.SetEducationLevel(opt_student->GetEducationLevel());
        if ( !fields_in_form.count("academic_group") ) new_student.SetAcademicGroup(opt_student->GetAcademicGroup());
        if ( !fields_in_form.count("age") ) new_student.SetAge(opt_student->GetAge());
        if ( !fields_in_form.count("it_status") ) new_student.SetIsItStudent(opt_student->GetIsItStudent());

        auto [is_updated, reason] = new_student.UpdateDescriptor();
        if ( !is_updated ) {
            return {DatabaseStatus::ERROR_CREATE_DESCRIPTOR, reason.value()};
        }

        // external_id, program_id, team_id, descriptor, institute_id, education_level, education_course, academic_group, age, is_it_student
        bool is_update_program_id = new_student.GetProgramID() != opt_student->GetProgramID();
        bool is_update_team_id = new_student.GetTeamID() != opt_student->GetTeamID();
        bool is_update_institute_id = new_student.GetInstituteId() != opt_student->GetInstituteId();
        bool is_update_education_level = new_student.GetEducationLevel() != opt_student->GetEducationLevel();
        bool is_update_education_course = new_student.GetEducationCourse() != opt_student->GetEducationCourse();
        bool is_update_academic_group = new_student.GetAcademicGroup() != opt_student->GetAcademicGroup();
        bool is_update_age = new_student.GetAge() != opt_student->GetAge();
        bool is_update_is_it_student = new_student.GetIsItStudent() != opt_student->GetIsItStudent();

        std::optional<Team> old_team;
        std::optional<Team> new_team;

        bool is_old_has_team = opt_student->GetTeamID() != recsys_t2s::common::ID::None;
        bool is_new_has_team = new_student.GetTeamID() != recsys_t2s::common::ID::None;

        if ( is_old_has_team && is_update_team_id ) {

            auto [ status, opt_team ] = SearchTeamByExternalID(opt_student->GetTeamID());
            if ( status != DatabaseStatus::OK )
                return status;
            old_team = opt_team;

        }
        if ( is_new_has_team && is_update_team_id ) {

            auto [ status, opt_team ] = SearchTeamByExternalID(new_student.GetTeamID());
            if ( status != DatabaseStatus::OK )
                return status;
            new_team = opt_team;

        }

        if ( is_old_has_team && old_team->GetStudents().size() == 1 ) {
            return {
                DatabaseStatus::ERROR_BAD_REQUEST,
                "Command " +
                    old_team->GetExternalID().AsString() +
                    " will remain empty. Pre-delete command " +
                    old_team->GetExternalID().AsString() + "."
            };
        }

        START_STATEMENT_SECTION

        Session session = Database::Instance()->CreateSession();
        Transaction transaction(session);

        std::string update_statement{ "UPDATE " STUDENTS_TABLE_NAME " SET " };

        std::vector<std::string> updated_fields;
        if ( is_update_program_id )
            updated_fields.emplace_back("program_id=" + new_student.GetProgramID().AsString());
        if ( is_update_team_id )
            updated_fields.emplace_back("team_id=" + new_student.GetTeamID().AsString());
        if ( is_update_institute_id )
            updated_fields.emplace_back("institute_id=" + new_student.GetInstituteId().AsString());
        if ( is_update_education_level )
            updated_fields.emplace_back("education_level=" + new_student.GetEducationLevel().AsString());
        if ( is_update_education_course )
            updated_fields.emplace_back("education_course=" + new_student.GetEducationCourse().AsString());
        if ( is_update_academic_group )
            updated_fields.emplace_back("academic_group=\"" + new_student.GetAcademicGroup().GetValue() + "\"");
        if ( is_update_age )
            updated_fields.emplace_back("age=" + std::to_string(new_student.GetAge().GetValue()));
        if ( is_update_is_it_student )
            updated_fields.emplace_back("is_it_student=" + std::to_string(new_student.GetIsItStudent().GetValue()));

        updated_fields.emplace_back("descriptor=" + DescriptorToHexBlob(new_student.GetDescriptor()));
        updated_fields.emplace_back("last_update_datetime=NOW()");

        for ( size_t i = 0; i < updated_fields.size() - 1; ++i ) {
            update_statement += updated_fields[i] + ", ";
        }

        update_statement += updated_fields.back();
        update_statement += " WHERE external_id=" + new_student.GetExternalID().AsString();

        transaction.execute(update_statement, false);

        // Вышел из старой команды
        if ( is_update_team_id && is_old_has_team ) {

            old_team->RemoveExistsStudent(new_student.GetExternalID(), opt_student->GetDescriptor());

            std::string update_team_statement{ "UPDATE " TEAMS_TABLE_NAME " SET "};
            if ( new_student.GetExternalID() == old_team->GetTeamLeadID() ) {
                update_team_statement += "team_lead_id=" + std::to_string(common::ID::None) + ", ";
            }
            update_team_statement += "descriptor=" + DescriptorToHexBlob(old_team->GetDescriptor()) + ", ";
            update_team_statement += "last_update_datetime=NOW() ";
            update_team_statement += "WHERE external_id=" + old_team->GetExternalID().AsString();

            transaction.execute(update_team_statement, false);

        }

        // Во всех случаях обновляем дескриптор в текущей команде
        if ( is_new_has_team ) {

            new_team->AppendExistsStudent(new_student.GetExternalID(), new_student.GetDescriptor());

            std::string update_team_statement{ "UPDATE " TEAMS_TABLE_NAME " SET "};
            update_team_statement += "descriptor=" + DescriptorToHexBlob(new_team->GetDescriptor()) + ", ";
            update_team_statement += "last_update_datetime=NOW() ";
            update_team_statement += "WHERE external_id=" + new_team->GetExternalID().AsString();

            transaction.execute(update_team_statement, false);

        }

        transaction.commit();

        END_STATEMENT_SECTION_WITH_STATUS

        return DatabaseStatus::OK;
    }

} // namespace recsys_t2s::database