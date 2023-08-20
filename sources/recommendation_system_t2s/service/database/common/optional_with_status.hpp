#ifndef RECOMMENDATION_SYSTEM_OPTIONAL_WITH_STATUS_HPP
#define RECOMMENDATION_SYSTEM_OPTIONAL_WITH_STATUS_HPP

#include <optional>
#include "database_status.hpp"

#define OPT_WITH_STATUS(status, value) std::make_pair(status, value)
#define OPT_WITH_STATUS_OK(value_to_make_opt) std::make_pair(DatabaseStatus{DatabaseStatus::OK, "OK"}, std::make_optional<decltype(value_to_make_opt)>(value_to_make_opt))
#define OPT_WITH_STATUS_ERR(err_type, message) std::make_pair(DatabaseStatus{DatabaseStatus::err_type, message}, std::nullopt)

namespace recsys_t2s::database {

    template<typename T>
    using optional_with_status = std::pair<DatabaseStatus, std::optional<T>>;

} // namespace recsys_t2s::database

#endif //RECOMMENDATION_SYSTEM_OPTIONAL_WITH_STATUS_HPP
