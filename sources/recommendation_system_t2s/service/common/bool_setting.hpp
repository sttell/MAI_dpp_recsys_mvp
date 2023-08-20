#ifndef RECOMMENDATION_SYSTEM_BOOL_SETTING_HPP
#define RECOMMENDATION_SYSTEM_BOOL_SETTING_HPP

namespace recsys_t2s::common {

    class BoolSetting {
    public:
        BoolSetting() = default;
        BoolSetting(bool value) : m_Value(value), m_IsNone(false) {};

        void SetValue(const bool value) {
            m_Value = value;
            m_IsNone = false;
        }

        bool GetValue() const noexcept {
            return m_Value;
        };

        bool IsNone() const noexcept{
            return m_IsNone;
        };

        bool operator==(const BoolSetting& rhs) { return (m_IsNone) ? (rhs.m_IsNone) : (m_Value == rhs.m_Value); }
        bool operator!=(const BoolSetting& rhs) { return (m_IsNone) ? (!rhs.m_IsNone) : (m_Value != rhs.m_Value); }

    private:
        bool m_Value{ false };
        bool m_IsNone{true};
    };

} // namespace recsys_t2s::common

#endif //RECOMMENDATION_SYSTEM_BOOL_SETTING_HPP
