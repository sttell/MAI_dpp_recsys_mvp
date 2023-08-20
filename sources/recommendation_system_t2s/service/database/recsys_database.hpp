#ifndef RECOMMENDATION_SYSTEM_RECSYS_DATABASE_HPP
#define RECOMMENDATION_SYSTEM_RECSYS_DATABASE_HPP


#include <string>
#include <vector>
#include <optional>

#include "../common/id.hpp"
#include "../common/institute_id.hpp"
#include "../common/education_level.hpp"
#include "../common/education_course.hpp"
#include "common/object_constructor.hpp"
#include "common/descriptor.hpp"

#include "common/optional_with_status.hpp"

#include "student.hpp"
#include "team.hpp"

namespace recsys_t2s::database {


    class RecSysDatabase {

        friend class Database;

    public:
        static DatabaseStatus Init();

        static DatabaseStatus InsertStudent(const Student& student);
        static optional_with_status<common::ID> InsertTeam(const Team& team)
        ;
        static optional_with_status<bool> CheckIfExistsStudentByExternalID(const common::ID& id);
        static optional_with_status<bool> CheckIfExistsTeamByExternalID(const common::ID& id);

        static DatabaseStatus DeleteStudentByExternalID(const common::ID& id);
        static DatabaseStatus DeleteTeamByExternalID(const common::ID& id);

        static optional_with_status<Student> SearchStudentByExternalID(const common::ID& id);
        static optional_with_status<Team> SearchTeamByExternalID(const common::ID& id);

        static optional_with_status<common::ID> UpdateStudent(Student new_student);
        static optional_with_status<common::ID> UpdateTeam(Team new_team);

    };


} // namespace recsys_t2s::database


#endif //RECOMMENDATION_SYSTEM_RECSYS_DATABASE_HPP
