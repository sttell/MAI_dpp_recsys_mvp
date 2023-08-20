#ifndef RECOMMENDATION_SYSTEM_INT_SETTING_HPP
#define RECOMMENDATION_SYSTEM_INT_SETTING_HPP

#include <numeric>
#include <limits>

#include "common/errors.hpp"

namespace recsys_t2s::common {

    class IntSetting {
    public:
        IntSetting() = default;
        IntSetting(int value) : m_Value(value), m_IsNone(false) {};
        IntSetting(int value, int min_value, int max_value) : m_Value(value), m_MinValue(min_value), m_MaxValue(max_value), m_IsNone(false) {};
        void SetValue(const int value) {
            if ( !IsValidValue(value) ) {
                std::string message{"Integer value out of range: value - "};
                message += std::to_string(value);
                message += ". Expected range - [" + std::to_string(m_MinValue) + ", " + std::to_string(m_MaxValue) + "]";
                throw ::common::exceptions::BadSettingValue(message);
            }
            m_Value = value;
            m_IsNone = false;
        }

        int GetValue() const noexcept {
            return m_Value;
        };

        bool IsNone() const noexcept{
            return m_IsNone;
        };

        bool operator==(const IntSetting& rhs) { return (m_IsNone) ? (rhs.m_IsNone) : (m_Value == rhs.m_Value); }
        bool operator!=(const IntSetting& rhs) { return (m_IsNone) ? (!rhs.m_IsNone) : (m_Value != rhs.m_Value); }

    private:

        bool IsValidValue(int value) const noexcept {
            return m_MinValue <= value && m_MaxValue >= value;
        }

    private:
        int m_Value{ 0 };
        int m_MinValue{ std::numeric_limits<int>::min() };
        int m_MaxValue{ std::numeric_limits<int>::max() };

        bool m_IsNone{true};
    };

} // namespace recsys_t2s::common

#endif //RECOMMENDATION_SYSTEM_INT_SETTING_HPP
