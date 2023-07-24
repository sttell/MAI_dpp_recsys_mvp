#include "html_form.hpp"

#include <cstdlib>
#include <iostream>

namespace utils {

    /**
     *
     * Test cases:
     *      ''
     *      '+'
     *      '-'
     *      '0'
     *      '01'
     *      '+1'
     *      '+01'
     *      '-01'
     *      '132'
     *      'asd'
     *      '-a'
     *
     * @param t_string_representation
     * @param t_is_unsigned
     * @return
     */
    bool IsValidInteger( const std::string& t_string_representation, bool t_is_unsigned ) {

        if ( t_string_representation.empty() )
            return false;

        // Check first zero
        int digits_start_idx{ 0 };
        if ( t_is_unsigned ) {
            if ( t_string_representation.size() > 1 && t_string_representation.at(0) == '0' ) {
                return false;
            }
        } else {

            const char first_symbol = t_string_representation.at(0);
            const bool is_first_symbol_digit = first_symbol >= '0' && first_symbol <= '9';
            const bool is_first_symbol_sign  = first_symbol == '-' || first_symbol == '+';

            if ( !is_first_symbol_digit && !is_first_symbol_sign )
                return false;
            if ( is_first_symbol_sign && t_string_representation.size() == 1 )
                return false;

            if ( is_first_symbol_sign )
                digits_start_idx++;

        }

        for ( size_t i = digits_start_idx; i < t_string_representation.size(); ++i ) {
            const char sym = t_string_representation.at(i);
            if ( sym < '0' || sym > '9' ) {
                return false;
            }
        }

        return true;
    }

    unsigned long long StringToUnsignedLongLong(const std::string& t_string_value) noexcept {
        const char* p_start = t_string_value.c_str();
        char* p_end;
        return strtoull(p_start, &p_end, 10);
    }

    std::optional<Poco::JSON::Array> StringToJsonArray(const std::string& t_string_value) {

        std::string json = "{\"tmp_key\": " + t_string_value + "}";

        Poco::JSON::Parser parser;

        Poco::Dynamic::Var result;
        try {
            result = parser.parse(t_string_value);
        } catch (const Poco::JSON::JSONException& e) {
            std::cerr << "Error: " << e.displayText() << std::endl;
        }

        Poco::JSON::Array::Ptr pArray;
        try {
            pArray = result.extract<Poco::JSON::Array::Ptr>();
        } catch (const Poco::BadCastException& e) {
            std::cerr << "Error: Cannot convertion json object to array" << std::endl;
        }

        if ( pArray == nullptr ) {
            return std::make_optional<Poco::JSON::Array>();
        }

        return std::make_optional<Poco::JSON::Array>(*pArray);
    }

    long long StringToSignedLongLong(const std::string& t_string_value) noexcept {
        const char* p_start = t_string_value.c_str();
        char* p_end;
        return strtoll(p_start, &p_end, 10);
    }

} // namespace [ functions ]

namespace recsys_t2s::handlers::impl {

    HTMLForm::HTMLForm() : HTMLFormBase() { /* Empty */ }

    HTMLForm::HTMLForm(const std::string& t_encoding) : HTMLFormBase(t_encoding) { /* Empty */ }

    HTMLForm::HTMLForm(const HTTPRequest& t_request, std::istream& t_requestBody, PartHandler& t_handler) :
        HTMLFormBase(t_request, t_requestBody, t_handler) { /* Empty */ }

    HTMLForm::HTMLForm(const HTTPRequest& t_request, std::istream& t_requestBody) :
        HTMLFormBase(t_request, t_requestBody) { /* Empty */ }

    HTMLForm::HTMLForm(const HTTPRequest& t_request) :
        HTMLFormBase(t_request) { /* Empty */ }


} // namespace recsys_t2s::handlers::impl