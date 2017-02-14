#include <boost/regex.hpp>

#include <sstream>

//#include <mongo/util/net/hostandport.h>

#include "../config.h"

#include "Log.h"
#include "BaseCore.h"
#include "base64.h"
#include "Config.h"
#include "CpuStat.h"

#define MAXCOUNT 1000

BaseCore::BaseCore()
{
    time_service_started_ = boost::posix_time::second_clock::local_time();
}

BaseCore::~BaseCore()
{
}


/** Возвращает расширенные данные о состоянии службы
 */
std::string BaseCore::Status(const std::string &server_name)
{
    std::stringstream out;

    // Время последнего обращения к статистике
    static boost::posix_time::ptime last_time_accessed;

    boost::posix_time::time_duration d;

    // Вычисляем количество запросов в секунду
    if (last_time_accessed.is_not_a_date_time())
        last_time_accessed = time_service_started_;
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    int millisecs_since_last_access =
        (now - last_time_accessed).total_milliseconds();
    int millisecs_since_start =
        (now - time_service_started_).total_milliseconds();
    int requests_per_second_current = 0;
    int requests_per_second_average = 0;

    if (millisecs_since_last_access)
        requests_per_second_current =
            (request_processed_ - last_time_request_processed) * 1000 /
            millisecs_since_last_access;
    if (millisecs_since_start)
        requests_per_second_average = request_processed_ * 1000 /
                                      millisecs_since_start;

    last_time_accessed = now;
    last_time_request_processed = request_processed_;

    out << "<html>";
    out << "<head>";
    out << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>";
    out << "<style type=\"text/css\">";
    out << "body { font-family: Arial, Helvetica, sans-serif; }";
    out << "h1, h2, h3 {font-family: \"georgia\", serif; font-weight: 400;}";
    out << "table { border-collapse: collapse; border: 1px solid gray; }";
    out << "td { border: 1px dotted gray; padding: 5px; font-size: 10pt; }";
    out << "th {border: 1px solid gray; padding: 8px; font-size: 10pt; }";
    out << "</style>";
    out << "</head>";
    out << "<body>";
    out << "<h1>Состояние службы Yottos GetMyAd Worker Logger</h1>";
    out << "<table>";
    out << "<tr>";
    out << "<td>Обработано запросов:</td><td><b>" << request_processed_;
    out << "</b> (" << requests_per_second_current << "/сек, ";
    out << " в среднем " << requests_per_second_average << "/сек) ";
    out << "</td></tr>";
    out << "<tr><td>Общее кол-во показов:</td><td><b>" << offer_processed_;
    out << "</b> (" << social_processed_ << " из них социальная реклама) "<< "</td></tr>";
    out << "<tr><td>Общее кол-во ретаргетинговых показов:</td><td><b>" << retargeting_processed_<< "</b></td></tr>";
    out << "<tr><td>Имя сервера: </td> <td>" << (server_name.empty() ? server_name : "неизвестно") <<"</td></tr>";
    out << "<tr><td>IP сервера: </td> <td>" <<Config::Instance()->server_ip_ <<"</td></tr>";
    out << "<tr><td>Текущее время: </td> <td>"<<boost::posix_time::second_clock::local_time()<<"</td></tr>";
    out << "<tr><td>Время запуска:</td> <td>" << time_service_started_ <<"</td></tr>";
    out << "<tr><td>Количество ниток: </td> <td>" << Config::Instance()->server_children_<< "</td></tr>";
    out << "<tr><td>CPU user: </td> <td>" << CpuStat::cpu_user << "</td></tr>";
    out << "<tr><td>CPU sys: </td> <td>" << CpuStat::cpu_sys << "</td></tr>";
    out << "<tr><td>RAM: </td> <td>" << CpuStat::rss << "</td></tr>";
    out << "<tr><td>База данных журналирования: </td> <td>" << cfg->mongo_log_db_;
    out << "</br>replica set = ";
    if (cfg->mongo_log_url_.empty())
        out << " no ";
    else
        out << cfg->mongo_log_url_;
    out << "</td></tr>";
    out << "</table>";
    out << "</body>";
    out << "</html>";

    return out.str();
}
