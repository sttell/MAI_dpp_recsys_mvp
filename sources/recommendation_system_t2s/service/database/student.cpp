#include "student.hpp"

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
        if ( student.GetAge().GetValue() < 0 ) {
            return std::make_pair(false, "Error while indexing: Student have incorrect age.");
        }

        return std::make_pair(true, std::make_optional<std::string>());
    }

} // namespace functions

namespace recsys_t2s::database {


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
        m_Descriptor[ DescriptorField::STUDENT_AGE ]    = static_cast<DescriptorType>(GetAge().GetValue()) / 100.f;
        m_Descriptor[ DescriptorField::STUDENT_IS_IT ]  = static_cast<DescriptorType>(GetIsItStudent().GetValue());

        return std::make_pair(true, std::make_optional<std::string>());
    }

} // namespace recsys_t2s::database