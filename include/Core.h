#ifndef CORE_H
#define CORE_H

#include <set>

#include <boost/date_time.hpp>
#include <mongocxx/client.hpp>

#include "Offer.h"
#include "Params.h"
#include "json.h"


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
    void ProcessSaveResults(mongocxx::client &client);

private:
    boost::posix_time::ptime
    ///start and ent time core process
    startCoreTime, endCoreTime, endCampaignTime;
    ///core thread id
    pthread_t tid;
    ///parameters to process: from http GET
    Params *params;
    ///return string
    nlohmann::json retJson;
};

#endif // CORE_H
