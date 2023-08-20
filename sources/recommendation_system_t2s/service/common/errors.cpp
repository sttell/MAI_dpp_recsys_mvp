#include "errors.hpp"

#define IMPLEMENT_DEFAULT_CONSTRUCTORS(classname, base_class) \
    classname::classname(const std::string& description) noexcept : base_class(description) {} \
    classname::classname(const char* description) noexcept : base_class(description) {}

namespace common {
namespace exceptions {

    IMPLEMENT_DEFAULT_CONSTRUCTORS(WrongPathError, std::runtime_error)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(UnexpectedFileFormatError, WrongPathError)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(FileNotFoundError, WrongPathError)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(ConfigNotFoundError, FileNotFoundError)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(InternalError, std::runtime_error)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(UnexpectedFactoryType, InternalError)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(UnexpectedHandlerType, UnexpectedFactoryType)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(BadRequest, std::runtime_error)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(BadURI, BadRequest)

    IMPLEMENT_DEFAULT_CONSTRUCTORS(BadSettingValue, BadRequest)

} // namespace exceptions
} // namespace common