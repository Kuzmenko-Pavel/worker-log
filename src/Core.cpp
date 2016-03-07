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

int HistoryManager::request_processed_ = 0;
int HistoryManager::offer_processed_ = 0;
int HistoryManager::social_processed_ = 0;

Core::Core()
{
    tid = pthread_self();

    hm = new HistoryManager();

    std::clog<<"["<<tid<<"]core start"<<std::endl;
}
//-------------------------------------------------------------------------------------------------------------------
Core::~Core()
{
    delete hm;
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

    if(!getInformer(params->informer_id_))
    {
        std::clog<<"there is no informer id: "<<prms->getInformerId()<<std::endl;
        std::clog<<" ip:"<<params->getIP();
        std::clog<<" country:"<<params->getCountry();
        std::clog<<" region:"<<params->getRegion();
        std::clog<<" cookie:"<<params->getCookieId();
        std::clog<<" context:"<<params->getContext();
        std::clog<<" search:"<<params->getSearch();
        std::clog<<" informer id:"<<params->informer_id_;
        std::clog<<" location:"<<params->getLocation();
        return Config::Instance()->template_error_;
    }

    hm->startGetUserHistory(params, informer);
    
    if (!params->async_)
    {
        //get campaign list
        getCampaign(params);

        endCampaignTime = boost::posix_time::microsec_clock::local_time();

        if (informer->plase_branch)
        {
            hm->place_get = true;
            hm->place_find = getOffers(items, params, false);
            if(hm->place_get  && !hm->place_find )
            {
                hm->place_clean = true;
                #ifdef DEBUG
                    printf("%s\n","place_clean");
                #endif // DEBUG
            }

        }
#ifndef DUMMY
        if (informer->retargeting_branch)
        {
            hm->retargeting_get = true;
            hm->retargeting_find = getRetargeting(items, params);
            if(hm->retargeting_get  && !hm->retargeting_find)
            {
                hm->retargeting_clean = true;
                #ifdef DEBUG
                    printf("%s\n","retargeting_clean");
                #endif // DEBUG
            }
            hm->retargeting_account_get = true;
            hm->retargeting_account_find = getRetargetingAccount(items, params);
            if(hm->retargeting_account_get  && !hm->retargeting_account_find)
            {
                hm->retargeting_account_clean = true;
                #ifdef DEBUG
                    printf("%s\n","retargeting_account_clean");
                #endif // DEBUG
            }
        }
#endif//DUMMY

        if( items.size() == 0 )
        {
            if(informer->nonrelevant == "social")
            {
                hm->social_get = true;
                hm->social_find = getOffers(items, params, true);
                hm->place_clean = true;
                if( items.size() == 0 )
                {
                    std::clog<<" ip:"<<params->getIP();
                    std::clog<<" country:"<<params->getCountry();
                    std::clog<<" region:"<<params->getRegion();
                    std::clog<<" cookie:"<<params->getCookieId();
                    std::clog<<" context:"<<params->getContext();
                    std::clog<<" search:"<<params->getSearch();
                    std::clog<<" informer id:"<<params->informer_id_;
                    std::clog<<" location:"<<params->getLocation();
                }
                else
                {
                    RISAlgorithm(items);
                }
            }
        }
        else
        {
            RISAlgorithm(items);
        }

    }

    resultHtml();

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
std::string Core::UserCode(Params *prms)
{
    startCoreTime = boost::posix_time::microsec_clock::local_time();

    params = prms;

    if(!getInformer(params->informer_id_))
    {
        std::clog<<"there is no informer id: "<<prms->getInformerId()<<std::endl;
        std::clog<<" ip:"<<params->getIP();
        std::clog<<" country:"<<params->getCountry();
        std::clog<<" region:"<<params->getRegion();
        std::clog<<" cookie:"<<params->getCookieId();
        std::clog<<" context:"<<params->getContext();
        std::clog<<" search:"<<params->getSearch();
        std::clog<<" informer id:"<<params->informer_id_;
        std::clog<<" location:"<<params->getLocation();
        return Config::Instance()->template_error_;
    }
    else
    {
        std::string html;
        html +="<html><head><META http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\">html, body {padding: 0; margin: 0; border: 0;}</style></head><body>";
        html += informer->user_code;
        html +="</body></html>";
        clearTmp();
        return html;
    }

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

    if(cfg->logOutPutSize)
        std::clog<<" out:"<<vResult.size();

    if(cfg->logIP)
        std::clog<<" ip:"<<params->getIP();

    if(cfg->logCountry)
        std::clog<<" country:"<<params->getCountry();

    if(cfg->logRegion)
        std::clog<<" region:"<<params->getRegion();

    if(cfg->logCookie)
        std::clog<<" cookie:"<<params->getCookieId();

    if(cfg->logContext)
        std::clog<<" context:"<<params->getContext();

    if(cfg->logSearch)
        std::clog<<" search:"<<params->getSearch();

    if(cfg->logInformerId)
        std::clog<<" informer id:"<<informer->id;

    if(cfg->logLocation)
        std::clog<<" location:"<<params->getLocation();

    if(cfg->logOutPutOfferIds || cfg->logRetargetingOfferIds)
    {
        std::clog<<" key:"<<params->getUserKey();
        if(hm->place_clean)
        {
            std::clog<<"[clean],";
        }
        std::clog<<" output ids:[";
        for(auto it = vResult.begin(); it != vResult.end(); ++it)
        {
            std::clog<<" "<<(*it)->id<<" "<<(*it)->id_int
            <<" hits:"<<(*it)->uniqueHits
            <<" rate:"<<(*it)->rating
            <<" cam:"<<(*it)->campaign_id
            <<" branch:"<<(*it)->getBranch();
        }
        std::clog<<"]";
    }
}
//-------------------------------------------------------------------------------------------------------------------
void Core::ProcessSaveResults()
{
    request_processed_++;

    log();

    if (!params->test_mode_ && informer)
    {

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
                                        append("inf_int", informer->id).
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
                                        append("inf_int", informer->id).
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
    }

    #ifdef DEBUG
        printf("Save  %s %d %d  \n", __func__, params->storage_, params->async_);
    #endif // DEBUG
    if (!params->storage_ && informer && !params->async_)
    {
        hm->updateUserHistory(vResult);
    }
    //clear tmp values: informer & temp table
    clearTmp();

    for (Offer::it o = items.begin(); o != items.end(); ++o)
    {
        if(o->second)
            delete o->second;
    }
    //clear all offers map
    items.clear();
    //clear output offers vector
    vRetargetingResult.clear();
    vResult.clear();
    OutPutCampaignRetargetingSet.clear();
    OutPutOfferRetargetingSet.clear();
    OutPutCampaignSet.clear();
    OutPutOfferSet.clear();

    if(cfg->toLog())
        std::clog<<std::endl;
}
//-------------------------------------------------------------------------------------------------------------------
std::string Core::OffersToHtml(Offer::Vector &items, const std::string &url)
{
    std::string informer_html;
    // Получаем HTML-код информера для отображение тизера
    informer_html =
        boost::str(boost::format(cfg->template_teaser_)
                   % informer->teasersCss
                   % OffersToJson(items)
                   % informer->capacity
                   % url
                   % params->location_
                   % informer->title
                   % informer->domain
                   % informer->account
                   % params->context_
                   % params->search_
                   % cfg->redirect_script_
                   % informer->headerHtml
                   % informer->footerHtml
                   % informer->html_notification
                );
    if (params->test_mode_)
    {
        informer_html = informer_html + "<!--test-->";
    }

    return informer_html;
}
//-------------------------------------------------------------------------------------------------------------------
std::string Core::OffersToJson(Offer::Vector &items)
{
    std::stringstream json;
    json << "{item:[";
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if (it != items.begin())
            json << ",";

        (*it)->redirect_url = base64_encode(boost::str(
                        boost::format("id=%s\ninf=%s\ntoken=%s\nurl=%s\nserver=%s\nloc=%s")
                        % (*it)->id
                        % params->informer_id_
                        % (*it)->token
                        % (*it)->url
                        % cfg->server_ip_
                        % params->location_
                    ));

        json << (*it)->toJson();
    }

    json << "]";
    if(informer->nonrelevant == "social")
    {
        json << ", nonrelevant: 'social'";
    }
    else
    {
        json << ", nonrelevant: 'usercode'";
    }
    json << boost::str(boost::format(", inf: \"%s\" ") % params->informer_id_) ;
    if(hm->place_clean)
    {
        json << ", clean:true";
    }
    else
    {
        json << ", clean:false";
    }
#ifndef DUMMY
    if(hm->retargeting_clean)
    {
        json << ", clean_retargeting:true";
    }
    else
    {
        json << ", clean_retargeting:false";
    }
    if(hm->retargeting_account_clean)
    {
        json << ", clean_account_retargeting:true";
    }
    else
    {
        json << ", clean_account_retargeting:false";
    }
#else
    json << ", clean_retargeting:false";
    json << ", clean_account_retargeting:false";
#endif
    json << "}";
    return json.str();
}
//-------------------------------------------------------------------------------------------------------------------
void Core::resultHtml()
{
    if(!vResult.empty())
    {
        if (params->json_)
            retHtml = OffersToJson(vResult);
        else
            retHtml = OffersToHtml(vResult, params->getUrl());
    }
    else
    {
        if (params->async_)
        {
            retHtml = OffersToHtml(vResult, params->getUrl());
        }
        else
        {
            if (params->json_)
            {
                retHtml = OffersToJson(vResult);
            }
            else
            {
                //fast logical bag fix
                if (!params->storage_ && informer && !params->async_)
                {
                    hm->updateUserHistory(vResult);
                }
                retHtml.clear();
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------
void Core::RISAlgorithm(const Offer::Map &items)
{
    #ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
        printf("%s\n","------------------------------------------------------------------");
        printf("RIS offers %lu \n",items.size());
    #endif // DEBUG
    Offer::MapRate result;
    Offer::MapRate resultRetargeting;
    std::map<const unsigned long, int> retargeting_view_offers = params->getRetargetingViewOffers();
    unsigned int loopCount;

    if( items.size() == 0)
    {
        std::clog<<"["<<tid<<"]"<<typeid(this).name()<<"::"<<__func__<< "error items size: 0"<<std::endl;
        #ifdef DEBUG
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            printf("Time %s taken return items.size() == 0: %lld \n", __func__,  microseconds);
            printf("%s\n","------------------------------------------------------------------");
        #endif // DEBUG
        return;
    }

    //sort by rating
    for(auto i = items.begin(); i != items.end(); i++)
    {
        if((*i).second)
        {
            if(hm->place_find || hm->retargeting_find)
            {
                if(!(*i).second->social)
                {
                    if((*i).second->retargeting)
                    {
                        resultRetargeting.insert(Offer::PairRate((*i).second->rating, (*i).second));
                    }
                    else
                    {
                        result.insert(Offer::PairRate((*i).second->rating, (*i).second));
                    }
                }
            }
        }
    }
    for(auto i = items.begin(); i != items.end(); i++)
    {
        if((*i).second)
        {
            if((*i).second->social)
            {
                hm->place_clean = true;
                result.insert(Offer::PairRate((*i).second->rating, (*i).second));
            }
        }
    }

    //teaser by unique id and company
    unsigned int passage;
    passage = 0;
    unsigned int unique_by_campaign;
    while ((passage < informer->capacity) && (vResult.size() < informer->capacity))
    {
        for(auto p = result.begin(); p != result.end(); ++p)
        {
            unique_by_campaign = (*p).second->unique_by_campaign + passage;
            if(OutPutCampaignSet.count((*p).second->campaign_id) < unique_by_campaign && OutPutOfferSet.count((*p).second->id_int) == 0)
            {
                if(vResult.size() >= informer->capacity)
                    break;

                vResult.push_back((*p).second);
                OutPutOfferSet.insert((*p).second->id_int);
                OutPutCampaignSet.insert((*p).second->campaign_id);

            }
        }
        passage++;
    }

    //teaser by unique id
    for(auto p = result.begin(); p != result.end(); ++p)
    {
        if(OutPutOfferSet.count((*p).second->id_int) == 0)
        {
            if(vResult.size() >= informer->capacity)
                break;

            vResult.push_back((*p).second);
            OutPutOfferSet.insert((*p).second->id_int);
            OutPutCampaignSet.insert((*p).second->campaign_id);
        }
    }

#ifndef DUMMY
/*-------------Retar----------------------*/
    unsigned int passage_r;
    passage_r = 0;
    while (passage_r <= retargeting_view_offers.size())
    {
      for(auto p = resultRetargeting.begin(); p != resultRetargeting.end(); ++p)
        {
            std::map<const unsigned long, int>::iterator it= retargeting_view_offers.find((*p).second->id_int);
            if( it != retargeting_view_offers.end() )
            {
                if(it->second > passage_r)
                {
                    continue;
                }
            }
            if(OutPutCampaignRetargetingSet.count((*p).second->campaign_id) < (*p).second->unique_by_campaign
                    && OutPutOfferRetargetingSet.count((*p).second->id_int) == 0 && !(*p).second->is_recommended)
            { 
                if(vRetargetingResult.size() >= informer->retargeting_capacity)
                    break;
                
                (*p).second->branch = EBranchL::L31;
                vRetargetingResult.push_back((*p).second);
                OutPutOfferRetargetingSet.insert((*p).second->id_int);
                OutPutCampaignRetargetingSet.insert((*p).second->campaign_id);

            }
            if ((*p).second->Recommended != "")
            {
              if ((*p).second->brending)
              {
                  for(auto pr = resultRetargeting.begin(); pr != resultRetargeting.end(); ++pr)
                    {
                        if(OutPutOfferRetargetingSet.count((*pr).second->id_int) == 0 && (*pr).second->is_recommended && (*p).second->campaign_id == (*pr).second->campaign_id )
                        {
                            if(vRetargetingResult.size() >= informer->retargeting_capacity)
                                break;
                            
                            (*pr).second->branch = EBranchL::L32;
                            vRetargetingResult.push_back((*pr).second);
                            OutPutOfferRetargetingSet.insert((*pr).second->id_int);
                            OutPutCampaignRetargetingSet.insert((*pr).second->campaign_id);

                        }
                    }
                }
            }
        }
        passage_r++;
    }

        //add teaser when teaser unique id
        for(auto p = resultRetargeting.begin(); p!=resultRetargeting.end(); ++p)
        {
            if(OutPutCampaignRetargetingSet.count((*p).second->campaign_id) < (*p).second->unique_by_campaign
                    && OutPutOfferRetargetingSet.count((*p).second->id_int) == 0 && !(*p).second->is_recommended)
            {
                if(vRetargetingResult.size() >= informer->retargeting_capacity)
                    break;
                
                (*p).second->branch = EBranchL::L31;
                vRetargetingResult.push_back((*p).second);
                OutPutOfferRetargetingSet.insert((*p).second->id_int);

            }
            if ((*p).second->Recommended != "")
            {
              for(auto pr = resultRetargeting.begin(); pr != resultRetargeting.end(); ++pr)
                {
                    if(OutPutCampaignRetargetingSet.count((*pr).second->campaign_id) < (*pr).second->unique_by_campaign
                            && OutPutOfferRetargetingSet.count((*pr).second->id_int) == 0 && (*pr).second->is_recommended)
                    {
                        if(vRetargetingResult.size() >= informer->retargeting_capacity)
                            break;
                        
                        (*pr).second->branch = EBranchL::L32;
                        vRetargetingResult.push_back((*pr).second);
                        OutPutOfferRetargetingSet.insert((*pr).second->id_int);

                    }
                }

            }
        }

    if(vRetargetingResult.size())
    {
        #ifdef DEBUG
            printf("vRetargetingResult.size() %lu\n",vRetargetingResult.size());
        #endif // DEBUG
        vResult.insert(vResult.begin(),vRetargetingResult.begin(),vRetargetingResult.end());
    }
#endif // DUMMY
/*-------------Retar----------------------*/

    //expand to return size
    #ifdef DEBUG
        printf("Time %s taken return RIS offers %lu: \n", __func__,  vResult.size());
    #endif // DEBUG
    
    loopCount = vResult.size();
    while(loopCount < informer->capacity)
    {
        for(auto p = resultRetargeting.begin(); p != resultRetargeting.end(); ++p)
        {
            if(vResult.size() < informer->capacity)
            {
                #ifdef DEBUG
                    printf("%s %lu %lu \n","retargeting_clean in appender", vResult.size(), informer->capacity);
                #endif // DEBUG
                hm->retargeting_clean = true;
                vResult.push_back((*p).second);
            }
        }
        for(auto p = result.begin(); p != result.end(); ++p)
        {
            if(vResult.size() < informer->capacity)
            {
                #ifdef DEBUG
                    printf("%s\n","place_clean in appender");
                #endif // DEBUG
                hm->place_clean = true;
                vResult.push_back((*p).second);
            }
        }
        loopCount++;
    }
        if(vResult.size() > informer->capacity)
        {
            vResult.erase(vResult.begin() + informer->capacity, vResult.end());
        }
    //Offer load
    for(auto p = vResult.begin(); p != vResult.end(); ++p)
    {
        (*p)->load();
        (*p)->gen();
    }
    #ifdef DEBUG
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("Time %s taken return RIS offers %lu: %lld \n", __func__,  vResult.size(), microseconds);
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
}
