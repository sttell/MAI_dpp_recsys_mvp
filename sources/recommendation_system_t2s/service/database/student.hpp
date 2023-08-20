#ifndef RECOMMENDATION_SYSTEM_STUDENT_HPP
#define RECOMMENDATION_SYSTEM_STUDENT_HPP

#include <string>
#include <vector>
#include <optional>

#include "../common/id.hpp"
#include "../common/institute_id.hpp"
#include "../common/education_level.hpp"
#include "../common/education_course.hpp"
#include "common/object_constructor.hpp"
#include "common/descriptor.hpp"

#include "common/optional_with_status.hpp"

namespace recsys_t2s::database {


    class Student {
        friend class Database;
    public:
        Student() = default;
        Student(Student&& user) = default;
        Student(const Student& user) = default;
        Student& operator=(Student&& user) = default;
        Student& operator=(const Student& user) = default;

        static DatabaseStatus Init();
        static optional_with_status<Student>     SearchByExternalID(const common::ID& external_id);
        static optional_with_status<common::ID>  UpdateStudent(Student& updated);

        [[nodiscard]] std::pair<bool, std::optional<std::string>> UpdateDescriptor() const;

        ADD_FIELD(common::ID, ID, common::ID::None);
        ADD_FIELD(common::ID, ExternalID, common::ID::None);
        ADD_FIELD(common::ID, ProgramID, common::ID::None);
        ADD_FIELD(common::ID, TeamID, common::ID::None);
        ADD_FIELD(common::ID, IndexInfoID, common::ID::None);
        ADD_MUTABLE_FIELD(Descriptor, Descriptor, {});
        ADD_FIELD(common::InstituteID, InstituteId, common::InstituteID::None);
        ADD_FIELD(common::EducationLevel, EducationLevel, common::EducationLevel::None);
        ADD_FIELD(common::EducationCourse, EducationCourse, common::EducationCourse::None);
        ADD_FIELD(std::string, AcademicGroup, std::string{});
        ADD_FIELD(int, Age, 0);
        ADD_FIELD(bool, IsItStudent, false);

    };

} // namespace recsys_t2s::database

#endif //RECOMMENDATION_SYSTEM_STUDENT_HPP
