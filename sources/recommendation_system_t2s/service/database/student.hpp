#ifndef RECOMMENDATION_SYSTEM_STUDENT_HPP
#define RECOMMENDATION_SYSTEM_STUDENT_HPP

#include <string>
#include <vector>
#include <optional>

#include "common/id.hpp"
#include "common/institute_id.hpp"
#include "common/education_level.hpp"
#include "common/education_course.hpp"
#include "common/string_setting.hpp"
#include "common/int_setting.hpp"
#include "common/bool_setting.hpp"
#include "common/object_constructor.hpp"
#include "common/descriptor.hpp"


namespace recsys_t2s::database {


    class Student {
        friend class Database;
    public:
        Student() = default;
        Student(Student&& user) = default;
        Student(const Student& user) = default;
        Student& operator=(Student&& user) = default;
        Student& operator=(const Student& user) = default;

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
        ADD_FIELD(common::StringSetting, AcademicGroup, common::StringSetting());
        ADD_FIELD(common::IntSetting, Age, common::IntSetting());
        ADD_FIELD(common::BoolSetting, IsItStudent, common::BoolSetting());

    };

} // namespace recsys_t2s::database

#endif //RECOMMENDATION_SYSTEM_STUDENT_HPP
