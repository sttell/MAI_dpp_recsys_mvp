#ifndef RECOMMENDATION_SYSTEM_STRING_SETTING_HPP
#define RECOMMENDATION_SYSTEM_STRING_SETTING_HPP

#include <string>

namespace recsys_t2s::common {

    class StringSetting {
    public:
        StringSetting() = default;
        StringSetting(std::string value) : m_Value(value), m_IsNone(false) {};

        void SetValue(const std::string& value) {
            m_Value = value;
            m_IsNone = false;
        }

        std::string GetValue() const noexcept {
            return m_Value;
        };

        bool IsNone() const noexcept{
            return m_IsNone;
        };

        bool operator==(const StringSetting& rhs) { return (m_IsNone) ? (rhs.m_IsNone) : (m_Value == rhs.m_Value); }
        bool operator!=(const StringSetting& rhs) { return (m_IsNone) ? (!rhs.m_IsNone) : (m_Value != rhs.m_Value); }

    private:
        std::string m_Value;
        bool m_IsNone{true};
    };

} // namespace recsys_t2s::common

#endif //RECOMMENDATION_SYSTEM_STRING_SETTING_HPP
