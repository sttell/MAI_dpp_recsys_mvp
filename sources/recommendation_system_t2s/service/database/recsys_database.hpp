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

#include <map>

namespace recsys_t2s::database {


    class RecSysDatabase {

        friend class Database;

    public:
        static DatabaseStatus Init();

        static DatabaseStatus InsertStudent(const Student& student);
        static DatabaseStatus InsertTeam(const Team& team);

        static optional_with_status<bool> CheckIfExistsStudentByExternalID(const common::ID& id);
        static optional_with_status<bool> CheckIfExistsTeamByExternalID(const common::ID& id);

        static DatabaseStatus DeleteStudentByExternalID(const common::ID& id);
        static DatabaseStatus DeleteTeamByExternalID(const common::ID& id);

        static optional_with_status<Student> SearchStudentByExternalID(const common::ID& id);
        static optional_with_status<Team> SearchTeamByExternalID(const common::ID& id);

        static DatabaseStatus UpdateStudent(Student new_student, const std::map<std::string, bool>& fields_in_form);
        static DatabaseStatus UpdateTeam(const Team& new_team, const std::map<std::string, bool>& fields_in_form);

        static optional_with_status<std::vector<common::ID>> GetRecommendationListForStudent(const common::ID& student_id);
        static optional_with_status<std::vector<common::ID>> GetRecommendationListForTeam(const common::ID& team_id);

    private:
        static optional_with_status<std::vector<Student>> GetAllStudentsMatchInfo(const Team& team);
        static optional_with_status<std::vector<Team>> GetAllTeamsMatchInfo(const common::ID& student_id);


    };


} // namespace recsys_t2s::database


#endif //RECOMMENDATION_SYSTEM_RECSYS_DATABASE_HPP
