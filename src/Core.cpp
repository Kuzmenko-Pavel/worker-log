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

    return retHtml;
}
//-------------------------------------------------------------------------------------------------------------------
void Core::log()
{
    if(cfg->toLog())
    {
        std::clog<<"["<<tid<<"]";
    }
    if(cfg->logCoreTime)
    {
        std::clog<<" core time:"<< boost::posix_time::to_simple_string(endCoreTime - startCoreTime);
    }

    if(cfg->logIP)
        std::clog<<" ip:"<<params->getIP();

    if(cfg->logCountry)
        std::clog<<" country:"<<params->getCountry();

    if(cfg->logRegion)
        std::clog<<" region:"<<params->getRegion();

    if(cfg->logCookie)
        std::clog<<" cookie:"<<params->getCookieId();
}
//-------------------------------------------------------------------------------------------------------------------
void Core::ProcessSaveResults()
{
    request_processed_++;

    log();
    mongo::BSONObj keywords = mongo::BSONObjBuilder().
    append("search", params->getSearch()).
    append("context", params->getContext()).
    obj();
    try
    {
        mongo::DB db("log");
        std::tm dt_tm;
        dt_tm = boost::posix_time::to_tm(params->time_);
        mongo::Date_t dt( (mktime(&dt_tm)) * 1000LLU);

        for(auto i = vResult.begin(); i != vResult.end(); ++i)
        {

            mongo::BSONObj record = mongo::BSONObjBuilder().genOID().
                                    append("dt", dt).
                                    append("id", (*i)->id).
                                    append("id_int",(long long)(*i)->id_int).
                                    append("title", (*i)->title).
                                    append("inf", params->informer_id_).
                                    append("inf_int", "").
                                    append("ip", params->ip_).
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
                                    append("country", (params->getCountry().empty()?"NOT FOUND":params->getCountry().c_str())).
                                    append("region", (params->getRegion().empty()?"NOT FOUND":params->getRegion().c_str())).
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
        
        mongo::BSONObj record_block = mongo::BSONObjBuilder().genOID().
                                    append("dt", dt).
                                    append("inf", params->informer_id_).
                                    append("inf_int", "").
                                    append("ip", params->ip_).
                                    append("cookie", params->cookie_id_).
                                    append("garanted", garanted).
                                    append("country", (params->getCountry().empty()?"NOT FOUND":params->getCountry().c_str())).
                                    append("region", (params->getRegion().empty()?"NOT FOUND":params->getRegion().c_str())).
                                    obj();
        db.insert(cfg->mongo_log_collection_block_, record_block, true);
    }
    catch (mongo::DBException &ex)
    {
        Log::err("DBException: insert into log db: %s", ex.what());
    }

    
    vResult.clear();
}
