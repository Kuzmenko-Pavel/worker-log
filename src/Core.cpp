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
//#include "DB.h"
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
void Core::ProcessSaveResults()
{
    request_processed_++;
    //mongo::DB db("log");
    boost::posix_time::ptime time_ = boost::posix_time::second_clock::local_time();
    boost::posix_time::ptime utime_ = boost::posix_time::second_clock::universal_time();
    std::tm pt_tm = boost::posix_time::to_tm(time_ + (time_ - utime_));
    std::time_t t = mktime(&pt_tm);
    //mongo::Date_t dt(t * 1000LLU);
    std::string inf = params->params_["informer_id"];
    long long inf_int = params->params_["informer_id_int"];
    std::string ip = params->params_["ip"];
    std::string cookie = params->params_["cookie"];
    std::string country = params->params_["country"];
    std::string region = params->params_["region"];
    std::string request = params->params_["request"];
    bool test = params->isTestMode();
    printf("%s\n","/////////////////////////////////////////////////////////////////////////");
    std::string id;
    long long id_int;
    std::string campaign_guid;
    long long campaign_id;
    std::string account_id;
    std::string campaign_title;
    std::string title;
    bool social;
    std::string token;
    std::string type = "teaser";
    std::string project;
    bool retargeting;
    bool isOnClick = true;
    std::string branch;
    std::string conformity = "place";
    std::string matching;
    //mongo::BSONObj keywords = mongo::BSONObjBuilder().
    //append("search", "").
    //append("context", "").
    //obj();
    nlohmann::json offer;
    for (nlohmann::json::iterator it = params->offers_.begin(); it != params->offers_.end(); ++it)
    {
            offer = *it;
            printf("%s\n",offer.dump().c_str());
            id = offer["guid"];
            id_int = offer["id"];
            campaign_guid = offer["campaign_guid"];
            campaign_id = offer["campaign_id"];
            account_id = offer["campaign_account"];
            campaign_title = offer["campaign_title"];
            title = offer["title"];
            social = offer["campaign_social"];
            token = offer["token"];
            project = offer["campaign_project"];
            retargeting = offer["retargeting"];
            branch = offer["branch"];
//            mongo::BSONObj record = mongo::BSONObjBuilder().genOID().
//                                    append("dt", dt).
//                                    append("id", id).
//                                    append("id_int",id_int).
//                                    append("title", title).
//                                    append("inf", inf).
//                                    append("inf_int", inf_int).
//                                    append("ip", ip).
//                                    append("cookie", cookie).
//                                    append("social", social).
//                                    append("token", token).
//                                    append("type", type).
//                                    append("isOnClick", isOnClick).
//                                    append("campaignId", campaign_guid).
//                                    append("account_id", account_id).
//                                    append("campaignId_int", campaign_id).
//                                    append("campaignTitle", campaign_title).
//                                    append("project", project).
//                                    append("country", country).
//                                    append("region", region).
//                                    append("retargeting", retargeting).
//                                    append("keywords", keywords).
//                                    append("branch", branch).
//                                    append("conformity", "place").//(*i)->conformity).
//                                    append("matching", matching).
//                                    append("test", test).
//                                    append("request", request).
//                                    obj();
//
//            db.insert(cfg->mongo_log_collection_impression_, record, true);


            if(!retargeting)
            {
              offer_processed_++;
            }
            else
            {
              retargeting_processed_++;
            }

            if (social) social_processed_ ++;
            
    }
    printf("%s\n","/////////////////////////////////////////////////////////////////////////");
}
