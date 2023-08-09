#ifndef RECOMMENDATION_SYSTEM_STUDENT_HPP
#define RECOMMENDATION_SYSTEM_STUDENT_HPP

#include <string>
#include <vector>
#include <optional>

#include "../common/id.hpp"
#include "../common/institute_id.hpp"
#include "../common/education_level.hpp"
#include "../common/education_course.hpp"

#include "student_indexer.hpp"

#include "database_status.hpp"

namespace recsys_t2s::database {

    template<typename T>
    using optional_with_status = std::pair<DatabaseStatus, std::optional<T>>;

    class Student {
        friend class Database;
    public:
        Student() = default;
        Student(Student&& user) = default;
        Student(const Student& user) = default;
        Student& operator=(Student&& user) = default;
        Student& operator=(const Student& user) = default;

        static DatabaseStatus Init();

        static optional_with_status<bool>        CheckIfExistsByExternalID(const common::ID& external_id);
        static optional_with_status<Student>     SearchByID(const common::ID& id);
        static optional_with_status<Student>     SearchByExternalID(const common::ID& external_id);
        static optional_with_status<common::ID>  DeleteByExternalID(const common::ID& external_id);
        static optional_with_status<common::ID>  UpdateStudent(const Student& updated, const StudentIndex& index);

        DatabaseStatus InsertToDatabase(const StudentIndex& index);

        ADD_FIELD(common::ID, ID, common::ID::None);
        ADD_FIELD(common::ID, ExternalID, common::ID::None);
        ADD_FIELD(common::ID, ProgramID, common::ID::None);
        ADD_FIELD(common::ID, TeamID, common::ID::None);
        ADD_FIELD(StudentDescriptor, Descriptor, {});
        ADD_FIELD(bool, IsHaveTeam, false);

    };

} // namespace recsys_t2s::database

#endif //RECOMMENDATION_SYSTEM_STUDENT_HPP
