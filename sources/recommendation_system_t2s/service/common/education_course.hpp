#ifndef RECOMMENDATION_SYSTEM_EDUCATIONCOURSE_HPP
#define RECOMMENDATION_SYSTEM_EDUCATIONCOURSE_HPP

#include <cstdint>
#include <limits>

namespace recsys_t2s::common {

    class EducationCourse {

        using course_t = int8_t;

    public:
        static constexpr course_t None{ -1 };

        EducationCourse() = default;

        constexpr EducationCourse( course_t value ) : m_Course(value), m_IsNone(value == EducationCourse::None)
        { /* Empty */ };

        constexpr operator course_t() const noexcept { return m_Course; }

        constexpr bool operator==( const EducationCourse& rhs ) { return !(m_IsNone) && (m_Course == rhs); }

        constexpr bool operator==( const EducationCourse::course_t& rhs ) { return !(m_IsNone) && (m_Course == rhs); }

        constexpr bool operator!=( const EducationCourse& rhs ) { return !(m_IsNone) && (m_Course != rhs); }

        constexpr bool operator<( const EducationCourse& rhs ) { return !(m_IsNone) && (m_Course < rhs); }

        std::string AsString() const {
            return std::to_string(m_Course);
        }

    private:
        course_t m_Course;
        bool m_IsNone{ false };
    };


} // namespace recsys_t2s::common

#endif //RECOMMENDATION_SYSTEM_EDUCATIONCOURSE_HPP
