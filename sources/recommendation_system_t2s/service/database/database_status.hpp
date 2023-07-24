#ifndef RECOMMENDATION_SYSTEM_DATABASE_STATUS_HPP
#define RECOMMENDATION_SYSTEM_DATABASE_STATUS_HPP

#include <cstdint>
#include <string>

namespace recsys_t2s::database {

    class DatabaseStatus {
    public:
        enum Type : uint8_t {
            None = 0,
            OK,
            ERROR_OBJECT_EXISTS,
            ERROR_STATEMENT,
            ERROR_CONNECTION,
            ERROR_NOT_EXISTS,
            _count_,
        };

        DatabaseStatus() = default;

        DatabaseStatus( Type type ) : m_Type(type), m_Message("") { /* Empty */ };

        DatabaseStatus( Type type, const std::string& message ) : m_Type(type), m_Message(message) { /* Empty */ }

        constexpr operator uint8_t() const noexcept { return static_cast<uint8_t>(m_Type); }

        constexpr operator Type() const noexcept { return m_Type; }

        constexpr bool operator==( const DatabaseStatus::Type& rhs ) { return m_Type == rhs; }

        constexpr bool operator==( const DatabaseStatus& rhs ) { return m_Type == rhs.m_Type; }

        constexpr bool operator!=( const DatabaseStatus& rhs ) { return m_Type != rhs.m_Type; }

        constexpr bool operator!=( const DatabaseStatus::Type& rhs ) { return m_Type != rhs; }

        constexpr bool operator<( const DatabaseStatus& rhs ) { return m_Type < rhs.m_Type; }

        std::string GetMessage() const noexcept { return m_Message; }

    private:
        Type m_Type;
        std::string m_Message;
    };

} // namespace recsys_t2s::database


#endif //RECOMMENDATION_SYSTEM_DATABASE_STATUS_HPP
