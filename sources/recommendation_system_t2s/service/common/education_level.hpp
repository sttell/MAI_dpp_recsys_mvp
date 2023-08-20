#ifndef RECOMMENDATION_SYSTEM_EDUCATION_LEVEL_HPP
#define RECOMMENDATION_SYSTEM_EDUCATION_LEVEL_HPP

#include <cstdint>
#include <string>
#include <map>
#include <algorithm>

namespace recsys_t2s::common {

    // TODO: Replace to data table
    // EduLevelID StringName
    //    ID      "bachelor"
    class EducationLevel {
    public:
        enum Type : uint8_t {
            None = 0,
            Bachelor,
            Specialty,
            Magistracy,
            _count_,
        };

        EducationLevel() = default;

        constexpr EducationLevel( Type type ) : m_Type(type) { /* Empty */ };

        constexpr EducationLevel( uint8_t type ) : m_Type(static_cast<Type>(type)) { /* Empty */ }

        EducationLevel(const std::string& string_name) : m_Type(Type::None) {

            static const std::map<std::string, Type> string_to_type{
                    { "bachelor",   Type::Bachelor  },
                    { "specialty",  Type::Specialty  },
                    { "magistracy", Type::Magistracy  }
            };

            if ( string_to_type.count(string_name) ) {
                m_Type = string_to_type.at(string_name);
            }

        };

        constexpr operator uint8_t() const noexcept { return static_cast<uint8_t>(m_Type); }

        constexpr operator Type() const noexcept { return m_Type; }

        constexpr bool operator==( const EducationLevel::Type& rhs ) { return m_Type == rhs; }

        constexpr bool operator==( const EducationLevel& rhs ) { return m_Type == rhs.m_Type; }

        constexpr bool operator!=( const EducationLevel& rhs ) { return m_Type != rhs.m_Type; }

        constexpr bool operator<( const EducationLevel& rhs ) { return m_Type < rhs.m_Type; }

        std::string AsString() const {
            return std::to_string(m_Type);
        }

    private:
        Type m_Type;
    };

} // namespace recsys_t2s::common

#endif //RECOMMENDATION_SYSTEM_EDUCATION_LEVEL_HPP
