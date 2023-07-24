#ifndef RECOMMENDATION_SYSTEM_STUDENT_INDEXER_HPP
#define RECOMMENDATION_SYSTEM_STUDENT_INDEXER_HPP

#include "database.hpp"
#include "student.hpp"

#include <vector>

namespace recsys_t2s::database {

    class StudentIndex {
        friend class Database;
    public:

        static constexpr size_t index_size = 19;
        using index_vector_t = std::array<float, index_size>;

        StudentIndex() = default;
        StudentIndex(StudentIndex&& user) = default;
        StudentIndex(const StudentIndex& user) = default;
        StudentIndex& operator=(StudentIndex&& user) = default;
        StudentIndex& operator=(const StudentIndex& user) = default;

        common::ID GetID() const noexcept { return m_ID; };
        index_vector_t GetVector() const noexcept { return m_Vector; };

        common::ID& ID() noexcept { return m_ID; };
        index_vector_t& Vector() noexcept { return m_Vector; };

        static DatabaseStatus Init();

        static optional_with_status<StudentIndex> AddStudentIndex(const Student& student);
        static optional_with_status<common::ID> SearchIndexByExternalID(const common::ID& external_id);
        static optional_with_status<StudentIndex> UpdateStudentIndex(const Student& student, bool is_guaranted_exists=false);

    private:
        common::ID m_ID;
        index_vector_t m_Vector;
    };

} // namespace recsys_t2s::database


#endif //RECOMMENDATION_SYSTEM_STUDENT_INDEXER_HPP
