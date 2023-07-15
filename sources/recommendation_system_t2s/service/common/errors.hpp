#ifndef RECOMMENDATION_SYSTEM_ERRORS_HPP
#define RECOMMENDATION_SYSTEM_ERRORS_HPP

#include <stdexcept>
#include <string>

#define REGISTER_EXCEPTION_TYPE(classname, base_class)               \
    class classname : public base_class {                            \
    public:                                                          \
        explicit classname(const std::string& description) noexcept; \
        explicit classname(const char* description) noexcept;        \
    }

namespace common {
namespace exceptions {

    REGISTER_EXCEPTION_TYPE(WrongPathError, std::runtime_error);

    REGISTER_EXCEPTION_TYPE(UnexpectedFileFormatError, WrongPathError);

    REGISTER_EXCEPTION_TYPE(FileNotFoundError, WrongPathError);

    REGISTER_EXCEPTION_TYPE(ConfigNotFoundError, FileNotFoundError);

    REGISTER_EXCEPTION_TYPE(InternalError, std::runtime_error);

    REGISTER_EXCEPTION_TYPE(UnexpectedFactoryType, InternalError);

    REGISTER_EXCEPTION_TYPE(UnexpectedHandlerType, UnexpectedFactoryType);

    REGISTER_EXCEPTION_TYPE(BadRequest, std::runtime_error);

    REGISTER_EXCEPTION_TYPE(BadURI, BadRequest);

} // namespace exceptions
} // namespace common

#endif //RECOMMENDATION_SYSTEM_ERRORS_HPP