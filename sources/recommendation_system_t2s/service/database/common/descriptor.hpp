#ifndef RECOMMENDATION_SYSTEM_DESCRIPTOR_HPP
#define RECOMMENDATION_SYSTEM_DESCRIPTOR_HPP

#include <array>
#include <sstream>

namespace recsys_t2s::database {

    using DescriptorType = float;

    class DescriptorField {
    public:
        enum : size_t {
            INSTITUTE_1 = 0,
            INSTITUTE_2,
            INSTITUTE_3,
            INSTITUTE_4,
            INSTITUTE_5,
            INSTITUTE_6,
            INSTITUTE_7,
            INSTITUTE_8,
            INSTITUTE_9,
            INSTITUTE_I10,
            INSTITUTE_T11,
            INSTITUTE_T12,
            INSTITUTE_C,
            GRADUATE_BACHELOR,
            GRADUATE_MAGISTRACY,
            GRADUATE_SPECIALIST,
            STUDENT_COURSE,
            STUDENT_AGE,
            STUDENT_IS_IT,

            _count_
        } Value;

        constexpr DescriptorField(size_t value) : Value(static_cast<decltype(Value)>(value)) {};

        constexpr operator size_t() { return static_cast<size_t>(Value); }

    };

    static constexpr size_t DESCRIPTOR_LENGTH = DescriptorField::_count_;
    static constexpr size_t DESCRIPTOR_BYTES = DescriptorField::_count_ * sizeof(DescriptorType);

    static constexpr size_t DESCRIPTOR_INSTITUTES_FIELDS_START_ID = DescriptorField::INSTITUTE_1;
    static constexpr size_t DESCRIPTOR_INSTITUTES_FIELDS_END_ID = DescriptorField::INSTITUTE_C;
    static constexpr size_t DESCRIPTOR_GRADUATE_FIELDS_START_ID = DescriptorField::GRADUATE_BACHELOR;
    static constexpr size_t DESCRIPTOR_GRADUATE_FIELDS_END_ID = DescriptorField::GRADUATE_SPECIALIST;


    using Descriptor = std::array<DescriptorType, DescriptorField::_count_>;

    std::string DescriptorToHexBlob(const Descriptor& descriptor, bool is_append_brackets=true);
    Descriptor HexBlobToDescriptor(const std::string& blob);

    float DistanceBetweenDescriptors(const Descriptor& left, const Descriptor& right);

} // namespace recsys_t2s::database

#endif //RECOMMENDATION_SYSTEM_DESCRIPTOR_HPP
