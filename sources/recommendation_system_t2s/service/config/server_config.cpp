#include "server_config.hpp"

#include "Poco/JSON/Parser.h"

#include <vector>
#include <cinttypes>
#include <fstream>

#include "path_validate.hpp"

namespace {

    constexpr const char*  kDefaultIP   = "0.0.0.0";
    constexpr unsigned int kDefaultPort = 8080;
    constexpr const char*  kDefaultDB_Host = "127.0.0.1";
    constexpr unsigned int kDefaultDB_Port = 3360;
    constexpr unsigned int kDefaultDB_TryConnectCount = 10;
    constexpr unsigned int kDefaultDB_TryConnectDelay = 1000;
    constexpr const char*  kDefaultDB_Login = "admin";
    constexpr const char*  kDefaultDB_Password = "admin";
    constexpr const char*  kDefaultDB_Database = "archdb";


} // namespace [ Constants ]

namespace {

    template <typename ExpectedType>
    void JsonGetValue(Poco::JSON::Object& node, const char* key, ExpectedType& value) {
        if ( node.has(key) ) {
            auto str_representation = node.get(key).toString();
            if constexpr ( std::is_constructible_v<ExpectedType, std::string> ) {
                value = str_representation;
            }
            if constexpr ( std::is_integral_v<ExpectedType> ) {
                std::istringstream iss(str_representation);
                if constexpr ( std::is_unsigned_v<ExpectedType> ) {
                    uint64_t integral_value;
                    iss >> integral_value;
                    value = static_cast<ExpectedType>(integral_value);
                }
                if constexpr ( !std::is_unsigned_v<ExpectedType> ) {
                    int64_t integral_value;
                    iss >> integral_value;
                    value = static_cast<ExpectedType>(integral_value);
                }
            }
        }
    }

    template<typename T>
    inline std::shared_ptr<T> GetSharedFromNode(Poco::JSON::Object& json_root, const char* key) {
        return std::make_shared<T>(json_root.getValue<T>(key));
    }

    template<typename T>
    inline void InitSubconfigIfHasKey(Poco::SharedPtr<Poco::JSON::Object>& root, std::shared_ptr<T>& subconfig, const char* key) {
        if ( root->has(key) ) {
            subconfig = std::make_shared<T>(*root->getObject(key));
            return;
        }
        subconfig = std::make_shared<T>();
    }

} // namespace [ Functions ]

namespace recsys_t2s::config {

    NetworkConfig::NetworkConfig() noexcept :
        m_IpAddress(std::make_shared<std::string>(kDefaultIP)),
        m_Port(std::make_shared<unsigned int>(kDefaultPort)) { /* Empty */ }

    NetworkConfig::NetworkConfig(Poco::JSON::Object &json_root) noexcept: NetworkConfig() {
        JsonGetValue(json_root, "ip_address", *m_IpAddress);
        JsonGetValue(json_root, "port", *m_Port);
    }

} // namespace recsys_t2s::config

namespace recsys_t2s::config {

    DatabaseConfig::DatabaseConfig() noexcept:
            m_HostIP(std::make_shared<std::string>(kDefaultDB_Host)),
            m_Port(std::make_shared<unsigned int>(kDefaultDB_Port)),
            m_Login(std::make_shared<std::string>(kDefaultDB_Login)),
            m_Password(std::make_shared<std::string>(kDefaultDB_Password)),
            m_DatabaseName(std::make_shared<std::string>(kDefaultDB_Database)),
            m_TryConnectDelay(std::make_shared<unsigned int>(kDefaultDB_TryConnectDelay)),
            m_TryConnectCount(std::make_shared<unsigned int>(kDefaultDB_TryConnectCount))
            { /* Empty */ }

    DatabaseConfig::DatabaseConfig(Poco::JSON::Object &json_root) noexcept: DatabaseConfig() {
        m_HostIP = GetSharedFromNode<std::string>(json_root, m_KeyHostIP);
        m_Port = GetSharedFromNode<unsigned int>(json_root, m_KeyPort);
        m_Login = GetSharedFromNode<std::string>(json_root, m_KeyLogin);
        m_Password = GetSharedFromNode<std::string>(json_root, m_KeyPassword);
        m_DatabaseName = GetSharedFromNode<std::string>(json_root, m_KeyDatabaseName);
        m_TryConnectDelay = GetSharedFromNode<unsigned int>(json_root, m_KeyTryConnectDelay);
        m_TryConnectCount = GetSharedFromNode<unsigned int>(json_root, m_KeyTryConnectCount);
    }

} // namespace recsys_t2s::config


namespace recsys_t2s::config {

    Config::Config(const std::string &path) :
        m_DatabaseConfig(nullptr),
        m_NetworkConfig(nullptr) {

        config::utils::ValidateJsonPath(path);

        std::ifstream fin(path);
        if ( !fin.is_open() ) {
            throw std::runtime_error("Error opening server config file.");
        }

        Poco::JSON::Parser parser;
        auto parsing_result = parser.parse(fin);
        fin.close();

        auto root = parsing_result.extract<Poco::JSON::Object::Ptr>();

        InitSubconfigIfHasKey(root, m_NetworkConfig, m_KeyNetworkConfig);
        InitSubconfigIfHasKey(root, m_DatabaseConfig, m_KeyDatabaseConfig);

    }

} // namespace recsys_t2s::config