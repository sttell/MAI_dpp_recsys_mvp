#include "student_indexer.hpp"

namespace functions {

    using student_t = recsys_t2s::database::StudentIndex;
    using index_vector_t = recsys_t2s::database::StudentDescriptor;

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

} // namespace functions

namespace recsys_t2s::database {

    StudentDescriptor StudentIndex::CreateDescriptor() const {

        functions::ValidateBuildIndexInfo(*this);

        StudentDescriptor output_vector{ 0.f };

        constexpr auto institutes_count = static_cast<uint8_t>(recsys_t2s::common::InstituteID::Type::_count_);
        constexpr auto education_level_count = static_cast<uint8_t>(recsys_t2s::common::EducationLevel::_count_);

        const auto institute_index = static_cast<uint8_t>(GetInstituteId()) - 1;
        const auto education_level_index = institutes_count + static_cast<uint8_t>(GetEducationLevel()) - 2;

        constexpr auto student_course_index = education_level_count + institutes_count - 2;
        constexpr auto student_age_index = student_course_index + 1;
        constexpr auto student_it_status_index = student_age_index + 1;

        auto student_course = static_cast<int8_t>(GetEducationCourse());
        if ( GetEducationLevel() == recsys_t2s::common::EducationLevel::Magistracy ) {
            student_course += 4;
        }

        output_vector[ institute_index ] = 1.f;
        output_vector[ education_level_index ] = 1.f;
        output_vector[ student_course_index ] = static_cast<float>(student_course) / 6.f;
        output_vector[ student_age_index ] = static_cast<float>(GetAge()) / 100.f;
        output_vector[ student_it_status_index ] = static_cast<float>(GetIsItStudent());

        return output_vector;
    }

} // namespace recsys_t2s::database