import os
import json
import random

rnd = random.Random()
rnd.seed(100)


def rnd_value(iterable):
    return iterable[rnd.randint(0, len(iterable) - 1)]


students = []

students_count = 1000

program_ids = [i for i in range(1, 6)]
institutes = [i for i in range(1, 14)]
education_levels = [i for i in range(1, 4)]
ages = [i for i in range(17, 30)]

it_statuses = [True, False]

print('Program IDs :', program_ids)
print('Institutes  :', institutes)
print('Education Lv:', education_levels)
print('Ages        :', ages)
print('Is Statuses :', it_statuses)

rnd = random.Random()

for i in range(students_count):

    institute = rnd_value(institutes)
    edu_level = rnd_value(education_levels)

    if edu_level == 1:
        group_fst_letter = "B"
        edu_course = rnd.randint(2, 4)
    if edu_level == 2:
        group_fst_letter = "S"
        edu_course = rnd.randint(2, 6)
    elif edu_level == 3:
        edu_course = rnd.randint(1, 2)
        group_fst_letter = "M"

    academic_g = group_fst_letter + str(institute) + "-20" + "-" + str(rnd.randint(100, 110))

    is_have_team = bool(rnd.randint(0, 1))

    students.append({
        "student_id": i,
        "program_id": rnd_value(program_ids),
        "institute": institute,
        "education_level": edu_level,
        "education_course": edu_course,
        "academic_group": academic_g,
        "age": rnd_value(ages) + edu_course,
        "it_status": rnd_value(it_statuses)
    })

    if is_have_team:
        students[-1]['team_id'] = rnd.randint(0, 50)

file = open('regular_students_data.json', 'w')
json.dump(students, file, indent=4)
file.close()