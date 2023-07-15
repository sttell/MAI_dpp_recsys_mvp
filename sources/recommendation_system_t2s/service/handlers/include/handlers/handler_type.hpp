#ifndef RECOMMENDATION_SYSTEM_HANDLER_TYPE_HPP
#define RECOMMENDATION_SYSTEM_HANDLER_TYPE_HPP

#include <cstdint>

namespace recsys_t2s::handlers {

    class HandlerType {
    public:
        enum Type : uint8_t {
            Base = 0, 
            Student, 
            Team, 
            _count_
        };

        HandlerType() = default;

        constexpr HandlerType( Type type ) : m_Type(type) { /* Empty */ };

        constexpr operator Type() const noexcept { return m_Type; }

        constexpr bool operator==( const HandlerType::Type& rhs ) { return m_Type == rhs; }

        constexpr bool operator==( const HandlerType& rhs ) { return m_Type == rhs.m_Type; }

        constexpr bool operator!=( const HandlerType& rhs ) { return m_Type != rhs.m_Type; }

        constexpr bool operator<( const HandlerType& rhs ) { return m_Type < rhs.m_Type; }

    private:
        Type m_Type;
    };

} // namespace recsys_t2s::handlers

#endif //RECOMMENDATION_SYSTEM_HANDLER_TYPE_HPP
