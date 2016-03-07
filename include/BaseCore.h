#ifndef BASECORE_H
#define BASECORE_H

#include "../config.h"

#include <boost/date_time.hpp>
#include <boost/circular_buffer.hpp>

/// Класс, который связывает воедино все части системы.
class BaseCore
{
public:
    /** \brief  Создаёт ядро.
     *
     * Производит все необходимые инициализации:
     *
     * - Загружает все сущности из базы данных;
     * - Подключается к RabbitMQ и создаёт очереди сообщений;
     * - При необходимости создаёт локальную базу данных MongoDB с нужными
     *   параметрами.
     */
    BaseCore();

    /** Пытается красиво закрыть очереди RabbitMQ, но при работе с FastCGI
     *  никогда не вызывается (как правило, процессы просто снимаются).
     */
    ~BaseCore();

    /** \brief  Загружает все сущности, которые используются при показе
     *          рекламы. */

    /** \brief  Выводит состояние службы и некоторую статистику */
    std::string Status(const std::string &);

private:

 /// Время запуска службы
    boost::posix_time::ptime time_service_started_,time_mq_check_;
};


#endif // CORE_H
