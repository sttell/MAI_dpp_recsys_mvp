#ifndef RECOMMENDATION_SYSTEM_STUDENT_INDEXER_HPP
#define RECOMMENDATION_SYSTEM_STUDENT_INDEXER_HPP

#include "common/id.hpp"
#include "common/institute_id.hpp"
#include "common/education_level.hpp"
#include "common/education_course.hpp"
#include <vector>

#define ADD_FIELD(type, name, default_value) \
    private:                                         \
        type m_##name{ default_value };  \
    public:                                      \
        type Get##name() const noexcept { return m_##name; } \
        void Set##name(const type& value) { m_##name = value; }

namespace recsys_t2s::database {

    using StudentDescriptor = std::array<float, 19>;

    class StudentIndex {
    public:

        [[nodiscard]] StudentDescriptor CreateDescriptor() const;


        ADD_FIELD(common::ID, ExternalID, common::ID::None);
        ADD_FIELD(common::ID, ProgramID, common::ID::None);
        ADD_FIELD(common::ID, TeamID, common::ID::None);
        ADD_FIELD(common::ID, IndexID, common::ID::None);
        ADD_FIELD(common::InstituteID, InstituteId, common::InstituteID::None);
        ADD_FIELD(common::EducationLevel, EducationLevel, common::EducationLevel::None);
        ADD_FIELD(common::EducationCourse, EducationCourse, common::EducationCourse::None);
        ADD_FIELD(std::string, AcademicGroup, {});
        ADD_FIELD(int, Age, 0);
        ADD_FIELD(bool, IsItStudent, false);

    };

} // namespace recsys_t2s::database


#endif //RECOMMENDATION_SYSTEM_STUDENT_INDEXER_HPP
