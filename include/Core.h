#ifndef CORE_H
#define CORE_H

#include <set>

#include <boost/date_time.hpp>

#include "Offer.h"
#include "Params.h"


/// Класс, который связывает воедино все части системы.
class Core
{
public:
    Core();
    ~Core();

    /** \brief  Обработка запроса на показ рекламы.
     * Самый главный метод. Возвращает HTML-строку, которую нужно вернуть
     * пользователю.
     */
    std::string Process(Params *params);
    /** \brief save process results to mongodb log */
    void ProcessSaveResults();

private:
    boost::posix_time::ptime
    ///start and ent time core process
    startCoreTime, endCoreTime, endCampaignTime;
    ///core thread id
    pthread_t tid;
    ///parameters to process: from http GET
    Params *params;
    ///return string
    std::string retHtml;
    ///result offers vector
    Offer::Vector vResult;
    /** \brief logging Core result in syslog */
    void log();
};

#endif // CORE_H
