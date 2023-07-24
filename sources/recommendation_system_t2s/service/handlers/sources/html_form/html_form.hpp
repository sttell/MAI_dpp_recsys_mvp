#ifndef RECOMMENDATION_SYSTEM_HTML_FORM_HPP
#define RECOMMENDATION_SYSTEM_HTML_FORM_HPP

#include "Poco/Net/HTMLForm.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Array.h"

#include <string>

using HTMLFormBase = Poco::Net::HTMLForm;

namespace utils {

    bool IsValidInteger( const std::string& string_representation, bool is_unsigned = false );

    unsigned long long StringToUnsignedLongLong(const std::string& string_value) noexcept;

    long long StringToSignedLongLong(const std::string& string_value) noexcept;

    std::optional<Poco::JSON::Array> StringToJsonArray(const std::string& string_value);

} // namespace [ functions ]

namespace recsys_t2s::handlers::impl {

    class HTMLForm : public HTMLFormBase {

        using HTTPRequest = Poco::Net::HTTPRequest;
        using PartHandler = Poco::Net::PartHandler;

    public:

        HTMLForm();

        explicit HTMLForm(const std::string& encoding);

        HTMLForm(const HTTPRequest& request, std::istream& requestBody, PartHandler& handler);

        HTMLForm(const HTTPRequest& request, std::istream& requestBody);

        explicit HTMLForm(const HTTPRequest& request);

    public:

        template<typename T>
        bool isConvertable(const std::string& field_key) const;

        template<typename T>
        std::optional<T> getValue(const std::string& field_key) const;

    };


    template<typename T>
    bool HTMLForm::isConvertable( const std::string& field_key ) const {
        try {
            auto tmp = this->getValue<T>(field_key);
        } catch (...) {
            return false;
        }
        return true;
    }

    template<typename T>
    std::optional<T> HTMLForm::getValue( const std::string& field_key ) const {

        if ( !this->has(field_key) ) {
            return std::make_optional<T>();
        }

        if constexpr ( std::is_convertible_v<T, std::string> ) {
            return std::make_optional(this->get(field_key));
        }

        const std::string string_value = this->get(field_key);

        if constexpr ( std::is_same_v<T, bool> ) {

            std::string lower_string;
            std::transform(string_value.begin(), string_value.end(), lower_string.begin(),
                [](char sym) -> char {
                    return static_cast<char>(std::tolower(sym));
                }
            );

            if ( lower_string == "true" || lower_string == "1" ) {
                return std::make_optional(true);
            }
            if ( lower_string == "false" || lower_string == "0" ) {
                return std::make_optional(false);
            }

            return std::make_optional<bool>();
        }

        // For integer types
        if constexpr( std::is_integral_v<T> ) {

            if constexpr( std::is_unsigned_v<T> ) {

                if ( !utils::IsValidInteger(string_value, true) )
                    return std::make_optional<T>();
                return std::make_optional(static_cast<T>(utils::StringToUnsignedLongLong(string_value)));

            } else {

                if ( !utils::IsValidInteger(string_value, false) )
                    return std::make_optional<T>();
                return std::make_optional(static_cast<T>(utils::StringToSignedLongLong(string_value)));

            }

        }

        if constexpr( std::is_same_v<T, Poco::JSON::Array> ) {

            return utils::StringToJsonArray(string_value);

        }

    }

} // namespace recsys_t2s::handlers::impl

#endif // RECOMMENDATION_SYSTEM_HTML_FORM_HPP
