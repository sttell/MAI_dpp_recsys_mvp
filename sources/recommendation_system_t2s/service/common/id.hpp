#ifndef RECOMMENDATION_SYSTEM_ID_HPP
#define RECOMMENDATION_SYSTEM_ID_HPP

#include <cstdint>
#include <limits>
#include <string>

namespace recsys_t2s::common {

    class ID {
    public:

        using id_type = int64_t;

        static constexpr id_type None{ -1 };

        ID() = default;

        constexpr ID( id_type value ) : m_ID(value), m_IsNone(value == ID::None) { /* Empty */ };

        constexpr operator id_type() const noexcept { return m_ID; }

        constexpr bool operator==( const ID& rhs ) { return (m_IsNone) ? false : (m_ID == rhs); }

        constexpr bool operator!=( const ID& rhs ) { return (m_IsNone) ? false : (m_ID != rhs); }

        constexpr bool operator<( const ID& rhs ) { return (m_IsNone) ? false : (m_ID < rhs); }

        [[nodiscard]] std::string AsString() const {
            return std::to_string(m_ID);
        }

    private:
        id_type m_ID;
        bool m_IsNone{ false };
    };


} // namespace recsys_t2s::common

#endif //RECOMMENDATION_SYSTEM_ID_HPP
