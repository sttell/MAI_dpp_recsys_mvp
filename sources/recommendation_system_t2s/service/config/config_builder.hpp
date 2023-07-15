#ifndef RECOMMENDATION_SYSTEM_CONFIG_BUILDER_HPP
#define RECOMMENDATION_SYSTEM_CONFIG_BUILDER_HPP

#define DefineConfigField(field_type, field_name, field_key)                                                        \
    private:                                                                                                        \
        std::shared_ptr<field_type> m_##field_name{ nullptr };                                                      \
        const char* m_Key##field_name{ field_key };                                                                 \
    public:                                                                                                         \
        inline std::shared_ptr<field_type> Get##field_name() const noexcept { return m_##field_name; }              \
        inline void Set##field_name(field_type value) noexcept { *(m_##field_name.get()) = std::move(value);  }


#define RegisterSubconfig(subconfig_type, field_name, field_key)                                                    \
private:                                                                                                            \
    std::shared_ptr<subconfig_type> m_##field_name{ nullptr };                                                      \
    const char* m_Key##field_name{ field_key };                                                                     \
public:                                                                                                             \
    const std::shared_ptr<subconfig_type> Get##field_name() const noexcept { return m_##field_name; };


#endif //RECOMMENDATION_SYSTEM_CONFIG_BUILDER_HPP
