#include "team.hpp"

#include <cassert>

#include "recsys_database.hpp"
#include <iostream>

namespace recsys_t2s::database {

    void Team::AppendExistsStudent(const common::ID &id, const recsys_t2s::database::Descriptor &student_desc) {
        for ( size_t i = 0; i < DESCRIPTOR_LENGTH; ++i ) {
            m_Descriptor[i] *= static_cast<float>(m_StudentExternalIndices.size());
            m_Descriptor[i] += student_desc[i];
            m_Descriptor[i] /= static_cast<float>(m_StudentExternalIndices.size() + 1);
        }
        m_StudentExternalIndices.insert(id);
        LoadStudents();
    }

    DatabaseStatus Team::LoadStudents() const {

        std::list<database::Student> new_students;

        // Load team lead
        {
            auto [status, opt_student] = database::RecSysDatabase::SearchStudentByExternalID(m_TeamLeadID);
            if (status != database::DatabaseStatus::OK && status != database::DatabaseStatus::ERROR_NOT_EXISTS)
                return {status.GetCode(), "Error while search Team Lead: " + status.GetMessage()};
        }

        // Load employees
        std::vector<common::ID> to_erase_ids;
        for ( const auto& student_id : m_StudentExternalIndices ) {
            auto [status, opt_student] = database::RecSysDatabase::SearchStudentByExternalID(student_id);

            if ( status == database::DatabaseStatus::ERROR_NOT_EXISTS ) {
                to_erase_ids.push_back(student_id);
                continue;
            }

            if ( status != database::DatabaseStatus::OK )
                return status;

            new_students.push_back(opt_student.value());
        }

        // Erase not exists students
        for ( const auto& id : to_erase_ids )
            m_StudentExternalIndices.erase(id);

        m_Students = new_students;

        return DatabaseStatus::OK;
    }

    std::pair<bool, std::optional<std::string>> Team::UpdateDescriptor() const {

        assert(!m_Students.empty());

        std::fill(m_Descriptor.begin(), m_Descriptor.end(), 0.f);

        /* Reduce */
        for ( auto& student : m_Students ) {
            auto current_student_descriptor = student.GetDescriptor();
            for ( size_t i = 0; i < DESCRIPTOR_LENGTH; ++i ) {
                m_Descriptor[i] += current_student_descriptor[i];
            }
        }

        /* Normalize */
        const size_t students_count = m_Students.size();
        for ( size_t i = 0; i < DESCRIPTOR_LENGTH; ++i ) {
            m_Descriptor[i] /= static_cast<float>(students_count);
        }

        return std::make_pair(true, std::make_optional<std::string>());
    }

    void Team::RemoveExistsStudent(const common::ID &id, const recsys_t2s::database::Descriptor &student_desc) {

        assert(m_StudentExternalIndices.size() > 1);
        assert(m_Students.size() == m_StudentExternalIndices.size());

        for ( size_t i = 0; i < DESCRIPTOR_LENGTH; ++i ) {
            m_Descriptor[i] *= static_cast<float>(m_StudentExternalIndices.size());
            m_Descriptor[i] -= student_desc[i];
            m_Descriptor[i] /= static_cast<float>(m_StudentExternalIndices.size() - 1);
        }
        m_StudentExternalIndices.erase(id);

        auto student_pos = std::find_if(m_Students.begin(), m_Students.end(), [id](const Student& student){ return student.GetExternalID() == id; });
        m_Students.erase(student_pos);
    }

} // namespace recsys_t2s::database
