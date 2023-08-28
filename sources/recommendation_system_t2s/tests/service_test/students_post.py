import pytest
import http.client
import json

def test_example():
    conn = http.client.HTTPConnection("localhost", 8081)
    payload = ''
    headers = {}
    conn.request("POST", "/student?student_id=4&program_id=2&institute=4&education_level=bachelor&academic_group=M20-80-107&age=25&it_status=false&education_course=1", payload, headers)
    res = conn.getresponse()
    data = json.loads(res.read().decode("utf-8"))

    assert data['detail'] == "Student with ID 4 exists."
    assert data['instance'] == "student_handler"
