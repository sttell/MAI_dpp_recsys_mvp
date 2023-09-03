/**
 * @file http_server.hpp
 * @brief Файл описывает основной класс HTTPServer.
 * @details Наследуется от Poco::Util::ServerApplication и реализует его интерфейс.
 */
#ifndef RECOMMENDATION_SYSTEM_HTTP_SERVER_HPP
#define RECOMMENDATION_SYSTEM_HTTP_SERVER_HPP

#include "Poco/Util/ServerApplication.h"

#include "config/server_config.hpp"

namespace recsys_t2s {

    /**
     * @brief Пространство имен скрывающее детали реализации
     */
    namespace impl {

        /**
         * @brief Alias для базового класса HTTP сервера.
         */
        using ServerApplicationBase = Poco::Util::ServerApplication;

    } // namespace impl

    /**
     * @brief Класс реализует базовый интерфейс класса HTTP сервера.
     * Является основным объектом в приложении. Курирует верхнеуровневую логику.
     */
    class HTTPWebServer : public impl::ServerApplicationBase {
    public:

        HTTPWebServer();

        ~HTTPWebServer() override = default;

    protected:

        /**
         * @brief Основной метод инициализации начального состояния сервиса.
         * @param t_self - объект приложения подлежащий инициализации.
         */
        void initialize( Application& t_self ) override;

        int main( [[maybe_unused]] const std::vector<std::string>& t_args ) override;

    private:
        bool m_IsHelpRequested{ false };
        std::shared_ptr<config::Config> m_Config{ nullptr };
    };

} // namespace search_service


#endif //RECOMMENDATION_SYSTEM_HTTP_SERVER_HPP
