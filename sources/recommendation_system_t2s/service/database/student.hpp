#ifndef RECOMMENDATION_SYSTEM_STUDENT_HPP
#define RECOMMENDATION_SYSTEM_STUDENT_HPP

#include <string>
#include <vector>
#include <optional>

#include "../common/id.hpp"
#include "../common/institute_id.hpp"
#include "../common/education_level.hpp"
#include "../common/education_course.hpp"

#include "database_status.hpp"

namespace recsys_t2s::database {

    template<typename T>
    using optional_with_status = std::pair<DatabaseStatus, std::optional<T>>;

    class Student {
        friend class Database;
    public:
        Student() = default;
        Student(Student&& user) = default;
        Student(const Student& user) = default;
        Student& operator=(Student&& user) = default;
        Student& operator=(const Student& user) = default;

        common::ID GetID() const noexcept { return m_ID; };
        common::ID GetExternalID() const noexcept { return m_ExternalID; };
        common::ID GetProgramID() const noexcept { return m_ProgramID; };
        common::ID GetTeamID() const noexcept { return m_TeamID; };
        common::ID GetIndexID() const noexcept { return m_IndexID; };
        common::InstituteID GetInstituteId() const noexcept { return m_InstituteId; };
        common::EducationLevel GetEducationLevel() const noexcept { return m_EducationLevel; };
        common::EducationCourse GetEducationCourse() const noexcept { return m_EducationCourse; };
        std::string GetAcademicGroup() const noexcept { return m_AcademicGroup; };
        int GetAge() const noexcept { return m_Age; };
        bool GetIsHaveTeam() const noexcept { return m_IsHaveTeam; };
        bool GetIsItStudent() const noexcept { return m_IsItStudent; };

        common::ID& ID() noexcept { return m_ID; }
        common::ID& ExternalID() noexcept { return  m_ExternalID; }
        common::ID& ProgramID() noexcept { return m_ProgramID; }
        common::ID& TeamID() noexcept { return m_TeamID; }
        common::ID& IndexID() noexcept { return m_IndexID; }
        common::InstituteID& InstituteId() noexcept { return m_InstituteId; }
        common::EducationLevel& EducationLevel() noexcept { return m_EducationLevel; }
        common::EducationCourse& EducationCourse() noexcept { return m_EducationCourse; }
        std::string& AcademicGroup() noexcept { return m_AcademicGroup; }
        int& Age() noexcept { return m_Age; }
        bool& IsHaveTeam() noexcept { return m_IsHaveTeam; }
        bool& IsItStudent() noexcept { return m_IsItStudent; }

        static DatabaseStatus Init();

        static optional_with_status<bool>        CheckIfExistsByExternalID(const common::ID& external_id);
        static optional_with_status<Student>     SearchByID(const common::ID& id);
        static optional_with_status<Student>     SearchByExternalID(const common::ID& external_id);
        static optional_with_status<common::ID>  DeleteByExternalID(const common::ID& external_id);
        static optional_with_status<common::ID>  UpdateStudent(const Student& updated);

        DatabaseStatus InsertToDatabase();

    private:
        common::ID m_ID{ common::ID::None };
        common::ID m_ExternalID{ common::ID::None };
        common::ID m_ProgramID{ common::ID::None };
        common::ID m_TeamID{ common::ID::None };
        common::ID m_IndexID{ common::ID::None };
        common::InstituteID m_InstituteId{ common::InstituteID::None };
        common::EducationLevel m_EducationLevel{ common::EducationLevel::None };
        common::EducationCourse m_EducationCourse{ common::EducationCourse::None };
        std::string m_AcademicGroup;

        int m_Age{ 0 };

        bool m_IsHaveTeam{ false };
        bool m_IsItStudent{ false };

    };

} // namespace recsys_t2s::database

#endif //RECOMMENDATION_SYSTEM_STUDENT_HPP
