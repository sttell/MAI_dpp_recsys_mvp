#ifndef RECOMMENDATION_SYSTEM_SERVER_CONFIG_HPP
#define RECOMMENDATION_SYSTEM_SERVER_CONFIG_HPP

#include "config_builder.hpp"

#include <string>
#include <memory>

namespace Poco::JSON { class Object; }

namespace recsys_t2s::config {

    class NetworkConfig {
    public:
        NetworkConfig() noexcept;

        explicit NetworkConfig(Poco::JSON::Object& json_root) noexcept;

        DefineConfigField(std::string,  IpAddress, "ip_address")
        DefineConfigField(unsigned int, Port,      "port"      )

    };

    class DatabaseConfig {
    public:
        DatabaseConfig() noexcept;
        explicit DatabaseConfig(Poco::JSON::Object& json_root) noexcept;

        DefineConfigField(std::string,  HostIP,       "host_ip" )
        DefineConfigField(unsigned int, Port,         "port"    )
        DefineConfigField(std::string,  Login,        "login"   )
        DefineConfigField(std::string,  Password,     "password")
        DefineConfigField(std::string,  DatabaseName, "database")
        DefineConfigField(unsigned int, TryConnectDelay, "try_connect_delay")
        DefineConfigField(unsigned int, TryConnectCount, "try_connect_count")

    };

    class Config {
    public:
        explicit Config(const std::string &path);

        Config(const Config &) = default;
        Config &operator=(const Config &) = default;

        Config(Config &&) = default;
        Config &operator=(Config &&) = default;

        RegisterSubconfig(DatabaseConfig, DatabaseConfig, "database")
        RegisterSubconfig(NetworkConfig,  NetworkConfig,  "network" )
    };

} // namespace recsys_t2s::config

#endif // RECOMMENDATION_SYSTEM_SERVER_CONFIG_HPP
