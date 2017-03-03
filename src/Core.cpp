#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/client.hpp>
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
    params = prms;
    retJson["status"] = "OK";
    return retJson.dump();
}
//-------------------------------------------------------------------------------------------------------------------
void Core::ProcessSaveResults(mongocxx::client &client)
{
    request_processed_++;
    boost::posix_time::ptime time_ = boost::posix_time::second_clock::local_time();
    boost::posix_time::ptime utime_ = boost::posix_time::second_clock::universal_time();
    auto dur = time_ - utime_;
    auto dt = std::chrono::system_clock::now() + std::chrono::hours(dur.hours());
    std::string inf = params->params_["informer_id"].get<std::string>();
    long inf_int = params->params_["informer_id_int"].get<long long>();
    std::string ip = params->params_["ip"].get<std::string>();
    std::string cookie = params->params_["cookie"].get<std::string>();
    std::string country = params->params_["country"].get<std::string>();
    std::string region = params->params_["region"].get<std::string>();
    std::string request = params->params_["request"].get<std::string>();
    std::string conformity = "place";

    bool test = params->isTestMode();
    std::vector<bsoncxx::document::value> documents;
    for(auto &&it : params->offers_)
    {
            auto builder = bsoncxx::builder::stream::document{};
            bsoncxx::document::value doc_value = builder
                << "dt" << bsoncxx::types::b_date(dt)
    			<< "id" << it["guid"].get<std::string>()
    			<< "id_int" << it["id"].get<long>()
    			<< "title" << it["title"].get<std::string>()
    			<< "inf" << inf
    			<< "inf_int" << inf_int 
    			<< "ip" << ip
    			<< "cookie" << cookie
    			<< "social" << it["campaign_social"].get<bool>()
    			<< "token" << it["token"].get<std::string>()
    			<< "type" << "teaser"
    			<< "isOnClick" << true
    			<< "campaignId" << it["campaign_guid"].get<std::string>()
    			<< "account_id" << it["campaign_account"].get<std::string>()
    			<< "campaignId_int" << it["campaign_id"].get<long>()
    			<< "campaignTitle" << it["campaign_title"].get<std::string>()
    			<< "project" << it["campaign_project"].get<std::string>()
    			<< "country" << country
    			<< "region" << region
        		<< "retargeting" << it["retargeting"].get<std::string>()
    			<< "keywords" << bsoncxx::builder::stream::open_document << "search" << "" << "context" << "" << bsoncxx::builder::stream::close_document
      	    	<< "branch" << it["branch"].get<std::string>()
    			<< "conformity" << conformity
    			<< "matching" << ""
    			<< "test" << test
    			<< "request" << request
                << bsoncxx::builder::stream::finalize;
            documents.push_back(doc_value);
            if(!it["retargeting"])
            {
              offer_processed_++;
            }
            else
            {
              retargeting_processed_++;
            }

            if (it["campaign_social"]) social_processed_ ++;
            
    }
    auto db = client.database(cfg->mongo_log_db_);
    auto coll = db.collection(cfg->mongo_log_collection_impression_);

    if(client)
    {
        coll.insert_many(documents);
    }
    return;
}
