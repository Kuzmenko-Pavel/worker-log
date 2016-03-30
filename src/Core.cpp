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
#include "DB.h"
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
    mongo::DB db("log");
    std::tm dt_tm;
    dt_tm = boost::posix_time::to_tm(params->time_);
    mongo::Date_t dt( (mktime(&dt_tm)) * 1000LLU);
    std::string inf = params->params_["informer_id"];
    long long inf_int = params->params_["informer_id_int"];
    std::string ip = params->params_["ip"];
    std::string cookie = params->params_["cookie"];
    std::string country = params->params_["country"];
    std::string region = params->params_["region"];
    std::string request = params->params_["request"];
    printf("%s\n","/////////////////////////////////////////////////////////////////////////");
    bool garanted = !params->offers_.empty();
    try
    {
        mongo::BSONObj record_block = mongo::BSONObjBuilder().genOID().
                                    append("dt", dt).
                                    append("inf", inf).
                                    append("inf_int", inf_int).
                                    append("ip", ip).
                                    append("cookie", cookie).
                                    append("garanted", garanted).
                                    append("country", country).
                                    append("region", region).
                                    obj();
        if (request == "initial")
        {
            db.insert(cfg->mongo_log_collection_block_, record_block, true);
            printf("%s\n",params->offers_.dump().c_str());
        }
    }
    catch (mongo::DBException &ex)
    {
        Log::err("DBException: insert into log db: %s", ex.what());
    }

    printf("%s\n","/////////////////////////////////////////////////////////////////////////");
    /* 
    mongo::BSONObj keywords = mongo::BSONObjBuilder().
    append("search", params->string_param("search")).
    append("context", params->string_param("context")).
    obj();

        for(auto i = vResult.begin(); i != vResult.end(); ++i)
        {

            mongo::BSONObj record = mongo::BSONObjBuilder().genOID().
                                    append("dt", dt).
                                    append("id", (*i)->id).
                                    append("id_int",(long long)(*i)->id_int).
                                    append("title", (*i)->title).
                                    append("inf", params->string_param("search")).
                                    append("inf_int", params->long_param("search")).
                                    append("ip", params->string_param("search")).
                                    append("cookie", params->cookie_id_).
                                    append("social", (*i)->social).
                                    append("token", (*i)->token).
                                    append("type", Offer::typeToString((*i)->type)).
                                    append("isOnClick", (*i)->isOnClick).
                                    append("campaignId", (*i)->campaign_guid).
                                    append("account_id", (*i)->account_id).
                                    append("campaignId_int", (long long)(*i)->campaign_id).
                                    append("campaignTitle", (*i)->campaign_title).
                                    append("project", (*i)->project).
                                    append("country", (params->string_param("ip").empty()?"NOT FOUND":params->string_param("ip").c_str())).
                                    append("region", (params->string_param("ip").empty()?"NOT FOUND":params->string_param("ip").c_str())).
                                    append("retargeting", (*i)->retargeting).
                                    append("keywords", keywords).
                                    append("branch", (*i)->getBranch()).
                                    append("conformity", "place").//(*i)->conformity).
                                    append("matching", (*i)->matching).
                                    obj();

            db.insert(cfg->mongo_log_collection_impression_, record, true);


            if(!(*i)->retargeting)
            {
              offer_processed_++;
            }
            else
            {
              retargeting_processed_++;
            }

            if ((*i)->social) social_processed_ ++;
        }
        bool garanted = (vResult.size() != 0);
        
    */
}
