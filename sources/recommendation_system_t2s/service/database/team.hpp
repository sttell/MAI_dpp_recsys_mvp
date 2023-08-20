#ifndef RECOMMENDATION_SYSTEM_TEAM_HPP
#define RECOMMENDATION_SYSTEM_TEAM_HPP

#include <string>
#include <vector>
#include <optional>
#include <set>
#include <list>

#include "../common/id.hpp"
#include "../common/institute_id.hpp"
#include "../common/education_level.hpp"
#include "../common/education_course.hpp"

#include "common/optional_with_status.hpp"
#include "common/object_constructor.hpp"
#include "common/descriptor.hpp"

#include "student.hpp"

namespace recsys_t2s::database {


    class Team {
        friend class Database;
    public:
        Team() = default;
        Team(Team&& user) = default;
        Team(const Team& user) = default;
        Team& operator=(Team&& user) = default;
        Team& operator=(const Team& user) = default;

        void AppendExistsStudent(const common::ID& id, const Descriptor& student_desc);
        void RemoveExistsStudent(const common::ID& id, const Descriptor& student_desc);

        DatabaseStatus LoadStudents(bool error_if_not_exists=false) const;
        std::pair<bool, std::optional<std::string>> UpdateDescriptor() const;

        ADD_FIELD(common::ID, ID, common::ID::None);
        ADD_FIELD(common::ID, ExternalID, common::ID::None);
        ADD_FIELD(common::ID, TeamLeadID, common::ID::None);
        ADD_MUTABLE_FIELD(Descriptor, Descriptor, Descriptor{ 0.f });
        ADD_MUTABLE_FIELD(std::set<common::ID>, StudentExternalIndices, std::set<common::ID>{});
        ADD_MUTABLE_FIELD(std::list<database::Student>, Students, std::list<database::Student>{});

    };

} // namespace recsys_t2s::database

#endif //RECOMMENDATION_SYSTEM_TEAM_HPP
