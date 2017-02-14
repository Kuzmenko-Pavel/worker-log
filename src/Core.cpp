#include <bsoncxx/json.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include "../config.h"
#include "Config.h"
#include "Core.h"
#include "base64.h"
#include "json.h"

Core::Core()
{
    std::clog<<"["<<tid<<"]core start"<<std::endl;
}
//-------------------------------------------------------------------------------------------------------------------
Core::~Core()
{
}
//-------------------------------------------------------------------------------------------------------------------
std::string Core::Process(Params *prms)
{
    #ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
        printf("%s\n","/////////////////////////////////////////////////////////////////////////");
    #endif // DEBUG
    startCoreTime = boost::posix_time::microsec_clock::local_time();

    params = prms;

    endCoreTime = boost::posix_time::microsec_clock::local_time();
    #ifdef DEBUG
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("Time %s taken: %lld \n", __func__,  microseconds);
        printf("%s\n","/////////////////////////////////////////////////////////////////////////");
    #endif // DEBUG
    retJson["status"] = "OK";
    return retJson.dump();
}
//-------------------------------------------------------------------------------------------------------------------
void Core::ProcessSaveResults(mongocxx::client &client)
{
    request_processed_++;
    boost::posix_time::ptime time_ = boost::posix_time::second_clock::local_time();
    boost::posix_time::ptime utime_ = boost::posix_time::second_clock::universal_time();
    std::tm pt_tm = boost::posix_time::to_tm(time_ + (time_ - utime_));
    std::time_t time = mktime(&pt_tm);
    std::string inf = params->params_["informer_id"];
    long inf_int = params->params_["informer_id_int"];
    std::string ip = params->params_["ip"];
    std::string cookie = params->params_["cookie"];
    std::string country = params->params_["country"];
    std::string region = params->params_["region"];
    std::string request = params->params_["request"];
    bool test = params->isTestMode();
    printf("%s\n","/////////////////////////////////////////////////////////////////////////");
    std::vector<bsoncxx::document::value> documents;
    for (nlohmann::json::iterator it = params->offers_.begin(); it != params->offers_.end(); ++it)
    {
            documents.push_back(bsoncxx::builder::stream::document{} << "dt" << time 
				<< "id" << (*it)["guid"].get<std::string>()
				<< "id_int" << (*it)["id"].get<long>()
				<< "title" << (*it)["title"].get<std::string>()
				<< "inf" << inf
				<< "inf_int" << inf_int 
				<< "ip" << ip
				<< "cookie" << cookie
				<< "social" << (*it)["campaign_social"].get<bool>()
				<< "token" << (*it)["token"].get<std::string>()
				<< "type" << "teaser"
				<< "isOnClick" << true
				<< "campaignId" << (*it)["campaign_guid"].get<std::string>()
				<< "account_id" << (*it)["campaign_account"].get<std::string>()
				<< "campaignId_int" << (*it)["campaign_id"].get<long>()
				<< "campaignTitle" << (*it)["campaign_title"].get<std::string>()
				<< "project" << (*it)["campaign_project"].get<std::string>()
				<< "country" << country
				<< "region" << region
				<< "retargeting" << (*it)["retargeting"].get<std::string>()
				<< "keywords" << bsoncxx::builder::stream::open_document
 				<< "search" << ""
                                << "context" << ""
				<< bsoncxx::builder::stream::close_document
				<< "branch" << (*it)["branch"].get<std::string>()
				<< "conformity" << "place"
				<< "matching" << ""
				<< "test" << test
				<< "request" << request
				<< bsoncxx::builder::stream::finalize);

            if(!(*it)["retargeting"])
            {
              offer_processed_++;
            }
            else
            {
              retargeting_processed_++;
            }

            if ((*it)["campaign_social"]) social_processed_ ++;
            
    } 
    client[cfg->mongo_log_db_][cfg->mongo_log_collection_impression_].insert_many(documents);
    printf("%s\n","/////////////////////////////////////////////////////////////////////////");
}
