
# Project message prefix that used by project_message macro for shows debug info.
set(PROJECT_MESSAGE_PREFIX "Unknown")

macro(project_message message_level message)
    message(${message_level} "[${PROJECT_MESSAGE_PREFIX}]: ${message}")
endmacro()