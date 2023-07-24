#ifndef RECOMMENDATION_SYSTEM_INSTITUTE_ID_HPP
#define RECOMMENDATION_SYSTEM_INSTITUTE_ID_HPP

#include <cstdint>
#include <string>
#include <map>
#include <algorithm>

namespace recsys_t2s::common {

    // TODO: Replace to data table
    // InstituteID StringName
    //    ID          "1"
    class InstituteID {
    public:
        enum Type : uint8_t {
            None = 0,
            I1,
            I2,
            I3,
            I4,
            I5,
            I6,
            I7,
            I8,
            I9,
            I10,
            T11,
            T12,
            C,
            _count_,
        };

        InstituteID() = default;

        constexpr InstituteID( Type type ) : m_Type(type) { /* Empty */ };

        constexpr InstituteID( uint8_t int_type ) : m_Type(static_cast<Type>(int_type)) { /* Empty */ }

        InstituteID(const std::string& string_name) : m_Type(Type::None) {

            static const std::map<std::string, Type> string_to_type{
                    { "1",   Type::I1  },
                    { "2",   Type::I2  },
                    { "3",   Type::I3  },
                    { "4",   Type::I4  },
                    { "5",   Type::I5  },
                    { "6",   Type::I6  },
                    { "7",   Type::I7  },
                    { "8",   Type::I8  },
                    { "9",   Type::I9  },
                    { "i10", Type::I10 },
                    { "t11", Type::T11 },
                    { "t12", Type::T12 },
                    { "c",   Type::C   }
            };

            if ( string_to_type.count(string_name) ) {
                m_Type = string_to_type.at(string_name);
            }

        };

        constexpr operator uint8_t() const noexcept { return static_cast<uint8_t>(m_Type); }

        constexpr operator Type() const noexcept { return m_Type; }

        constexpr bool operator==( const InstituteID::Type& rhs ) { return m_Type == rhs; }

        constexpr bool operator==( const InstituteID& rhs ) { return m_Type == rhs.m_Type; }

        constexpr bool operator!=( const InstituteID& rhs ) { return m_Type != rhs.m_Type; }

        constexpr bool operator<( const InstituteID& rhs ) { return m_Type < rhs.m_Type; }

        std::string AsString() const {
            return std::to_string(m_Type);
        }

    private:
        Type m_Type;
    };

} // namespace recsys_t2s::common

#endif //RECOMMENDATION_SYSTEM_INSTITUTE_ID_HPP
