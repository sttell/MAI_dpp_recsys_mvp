#ifndef RECOMMENDATION_SYSTEM_OBJECT_CONSTRUCTOR_HPP
#define RECOMMENDATION_SYSTEM_OBJECT_CONSTRUCTOR_HPP

#define ADD_FIELD(type, name, default_value) \
    private:                                         \
        type m_##name{ default_value };  \
    public:                                      \
        type Get##name() const noexcept { return m_##name; } \
        void Set##name(const type& value) { m_##name = value; }

#define ADD_MUTABLE_FIELD(type, name, default_value) \
    private:                                         \
        mutable type m_##name{ default_value };  \
    public:                                      \
        type Get##name() const noexcept { return m_##name; } \
        void Set##name(const type& value) { m_##name = value; }


#endif //RECOMMENDATION_SYSTEM_OBJECT_CONSTRUCTOR_HPP
