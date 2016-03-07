#include "Core_DataBase.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <vector>
#include <map>
#include <chrono>
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "Config.h"
#include "Offer.h"
#include "../config.h"

#define CMD_SIZE 2621440

Core_DataBase::Core_DataBase():
    len(CMD_SIZE)
{
    cmd = new char[len];
    tmpTable = new TempTable(cmd, len);
}

Core_DataBase::~Core_DataBase()
{
    delete tmpTable;
    delete []cmd;
}

bool Core_DataBase::clearTmp()
{
    if(informer)
        delete informer;

    return tmpTable->clear();
}

//-------------------------------------------------------------------------------------------------------------------
bool Core_DataBase::getOffers(Offer::Map &items, Params *_params, bool get_social)
{
    bool result = false;
    #ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
    Kompex::SQLiteStatement *pStmt;
    params = _params;
    std::string offerSqlStr = "\
                    SELECT %s\
                    FROM Offer AS ofrs\
                    LEFT JOIN Offer2Rating AS oret ON ofrs.id=oret.id\
                    LEFT JOIN Informer2OfferRating AS iret ON iret.id_inf=%lld AND ofrs.id=iret.id_ofr\
                    %s %s %s ;\
                   ";
    std::string select_field ="\
        ofrs.id,\
        ofrs.campaignId,\
        ofrs.type,\
        ifnull(iret.rating,oret.rating) AS rating,\
        ofrs.UnicImpressionLot,\
        ofrs.height,\
        ofrs.width,\
        ofrs.isOnClick,\
        ofrs.social,\
        ofrs.account, \
        ofrs.offer_by_campaign_unique,\
        '',\
        '',\
        0,\
        ofrs.brending, \
        0,\
        ofrs.html_notification\
        ";
    std::string social = get_social ? "1" : "0";
    std::string cost = "";
    std::string gender = "";
    if (params->cost_.empty())
    {
        cost = " and cost = 0";
    }
    else
    {
        cost = " and cost IN (0," + params->cost_ + ")";
    }
    if (params->gender_.empty())
    {
        gender = " and gender = 0";
    }
    else
    {
        gender = " and gender IN (0," + params->gender_ + ")";
    }
    std::string where_offers = "WHERE ofrs.campaignId IN (SELECT id FROM "+ tmpTable->str() +" WHERE retargeting = 0 "+ cost + gender  +" ) AND ofrs.social = "+ social +"  AND ofrs.id NOT IN ( SELECT [offerId] FROM Session where id=" + params->getUserKey() +" and uniqueHits <= 0)"; 
    if (!params->getExcludedOffersString().empty())
    {
        where_offers = "WHERE ofrs.campaignId IN (SELECT id FROM "+ tmpTable->str() +" WHERE retargeting = 0 "+ cost + gender  +" ) AND ofrs.social = "+ social +"  AND ofrs.id NOT IN ("+ params->getExcludedOffersString() +")";
    }
    std::string order_offers = "ORDER BY rating DESC"; 
    std::string limit_offers = "LIMIT " + std::to_string(informer->capacity * 10); 

    bzero(cmd,sizeof(cmd));
    sqlite3_snprintf(len, cmd, offerSqlStr.c_str(),
                     select_field.c_str(),
                     informer->id,
                     where_offers.c_str(),
                     order_offers.c_str(),
                     limit_offers.c_str());
    try
    {
        pStmt = new Kompex::SQLiteStatement(cfg->pDb->pDatabase);

        try
        {
            pStmt->Sql(cmd);

            while(pStmt->FetchRow())
            {

                if(items.count(pStmt->GetColumnInt64(0)) > 0)
                {
                    continue;
                }

                Offer *off = new Offer(pStmt->GetColumnInt64(0),     //id
                                       pStmt->GetColumnInt64(1),    //campaignId
                                       pStmt->GetColumnInt(2),      //type
                                       pStmt->GetColumnDouble(3),   //rating
                                       pStmt->GetColumnInt(4),    //uniqueHits
                                       pStmt->GetColumnInt(5),     //height
                                       pStmt->GetColumnInt(6),     //width
                                       pStmt->GetColumnBool(7),    //isOnClick
                                       pStmt->GetColumnBool(8),    //social
                                       pStmt->GetColumnString(9),  //account_id
                                       pStmt->GetColumnInt(10),      //offer_by_campaign_unique
                                       pStmt->GetColumnString(11),   //recomendet
                                       pStmt->GetColumnString(12),   //retid
                                       "",
                                       pStmt->GetColumnBool(13),    // retargeting 
                                       pStmt->GetColumnBool(14),    //brending
                                       pStmt->GetColumnBool(15),    //isrecomendet
                                       pStmt->GetColumnBool(16)    //notification
                                      );
                items.insert(Offer::Pair(off->id_int,off));
                result = true;
            }
        }
        catch(Kompex::SQLiteException &ex)
        {
            std::clog<<"["<<pthread_self()<<"]"<<__func__<<" error: "
                     <<ex.GetString()
                     <<" \n"
                     <<cmd
                     <<params->get_.c_str()
                     <<params->post_.c_str()
                     <<std::endl;
        }

        pStmt->FreeQuery();
        delete pStmt;

    }
    catch(std::exception const &ex)
    {
        std::clog<<"["<<pthread_self()<<"]"<<__func__<<" error: "
                 <<ex.what()
                 <<" \n"
                 <<cmd
                 <<std::endl;
    }
    //clear if there is(are) banner
    #ifdef DEBUG
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("Time %s taken: %lld \n", __func__,  microseconds);
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
    return result;
}
//-------------------------------------------------------------------------------------------------------------------
void Core_DataBase::getCampaign(Params *_params)
{
    #ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
    Kompex::SQLiteStatement *pStmt;
    params = _params;
    std::string D = "cast(strftime('%w','now','localtime') AS INT)";
    std::string H = "cast(strftime('%H','now','localtime') AS INT)";
    std::string M = "cast(strftime('%M','now','localtime') AS INT)";
    if (!params->D_.empty())
    {
        D = params->D_;
    }
    if (!params->M_.empty())
    {
        M = params->M_;
    }

    if (!params->H_.empty())
    {
        H = params->H_;
    }
    bzero(cmd,sizeof(cmd));
    std::string social = "";
    if (informer->nonrelevant != "social")
    {
        social = "WHERE ca.social=0";
    }
    if (informer->blocked)
    {
        social = "WHERE ca.social=1";
    }
    sqlite3_snprintf(len, cmd, cfg->campaingSqlStr.c_str(),
                         tmpTable->c_str(),
                         params->getCountry().c_str(),
                         params->getRegion().c_str(),
                         params->getDevice().c_str(),
                         informer->domainId,
                         informer->domainId,
                         informer->accountId,
                         informer->id,
                         informer->domainId,
                         informer->domainId,
                         informer->accountId,
                         informer->accountId,
                         informer->id,
                         informer->id,
                         D.c_str(),
                         H.c_str(),
                         M.c_str(),
                         H.c_str(),
                         D.c_str(),
                         H.c_str(),
                         M.c_str(),
                         H.c_str(),
                         social.c_str()
                         );


    try
    {
        pStmt = new Kompex::SQLiteStatement(cfg->pDb->pDatabase);

        pStmt->SqlStatement(cmd);

        pStmt->FreeQuery();
        delete pStmt;
    }
    catch(Kompex::SQLiteException &ex)
    {
        std::clog<<"["<<pthread_self()<<"] error: "<<__func__
                 <<ex.GetString()
                 <<" \n"
                 <<cmd
                 <<params->get_.c_str()
                 <<params->post_.c_str()
                 <<std::endl;
    }
    #ifdef DEBUG
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("Time %s taken: %lld \n", __func__,  microseconds);
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
}
//-------------------------------------------------------------------------------------------------------------------
bool Core_DataBase::getInformer(const std::string informer_id)
{
    #ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
    bool ret = false;
    Kompex::SQLiteStatement *pStmt;

    informer = nullptr;

    bzero(cmd,sizeof(cmd));
    sqlite3_snprintf(CMD_SIZE, cmd, cfg->informerSqlStr.c_str(), informer_id.c_str());

    try
    {
        pStmt = new Kompex::SQLiteStatement(cfg->pDb->pDatabase);

        pStmt->Sql(cmd);

        while(pStmt->FetchRow())
        {
            informer =  new Informer(pStmt->GetColumnInt64(0),
                                     pStmt->GetColumnString(1),
                                     pStmt->GetColumnInt(2),
                                     pStmt->GetColumnString(3),
                                     pStmt->GetColumnString(4),
                                     pStmt->GetColumnString(5),
                                     pStmt->GetColumnString(6),
                                     pStmt->GetColumnInt64(7),
                                     pStmt->GetColumnString(8),
                                     pStmt->GetColumnInt64(9),
                                     pStmt->GetColumnString(10),
                                     pStmt->GetColumnDouble(11),
                                     pStmt->GetColumnDouble(12),
                                     pStmt->GetColumnDouble(13),
                                     pStmt->GetColumnDouble(14),
                                     cfg->range_category_,
                                     pStmt->GetColumnInt(15),
                                     pStmt->GetColumnBool(16),
                                     pStmt->GetColumnString(17),
                                     pStmt->GetColumnString(18),
                                     pStmt->GetColumnBool(19),
                                     pStmt->GetColumnBool(20),
                                     pStmt->GetColumnBool(21)
                                    );
            ret = true;
            break;
        }

        pStmt->FreeQuery();

        delete pStmt;
    }
    catch(Kompex::SQLiteException &ex)
    {
        std::clog<<"["<<pthread_self()<<"] error: "<<__func__
                 <<ex.GetString()
                 <<" \n"
                 <<cmd
                 <<std::endl;
    }
    #ifdef DEBUG
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("Time %s taken: %lld \n", __func__,  microseconds);
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG

    return ret;
}
//----------------------------Retargeting---------------------------------------
bool Core_DataBase::getRetargeting(Offer::Map &items, Params *_params)
{
    bool result = false;
    #ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
    Kompex::SQLiteStatement *pStmt;
    params = _params;
    Offer::Map itemsR;
    std::map<const unsigned long,bool> retargeting_exclude_offers_map = params->getRetargetingExcludedOffersMap();
    std::vector<std::string> VRecommended;
    std::vector<std::string> temp_recommended;
    std::string offerSqlStr = "\
                    SELECT %s\
                    FROM OfferR AS ofrs\
                    %s %s %s ;\
                   ";
    std::string where_offers = "";
    std::string order_offers = ""; 
    std::string limit_offers = "LIMIT " + std::to_string(informer->capacity * 5);
    int count = 0;
    std::string recomendet_type = "all";
    int recomendet_count = 10;
    int day = 0;
    std::vector<std::string> ret = params->getRetargetingOffers();
    if(!ret.size())
    {
        return result;
    }
    std::map<const unsigned long,int> retargeting_offers_day = params->getRetargetingOffersDayMap();
    std::string select_field ="\
        ofrs.id,\
        ofrs.campaignId,\
        ofrs.type,\
        10000000.0,\
        ofrs.UnicImpressionLot,\
        ofrs.height,\
        ofrs.width,\
        ofrs.isOnClick,\
        ofrs.social,\
        ofrs.account, \
        ofrs.offer_by_campaign_unique,\
        ofrs.Recommended,\
        ofrs.retid,\
        1,\
        ofrs.brending, \
        1,\
        ofrs.html_notification,\
        ofrs.recomendet_count, \
        ofrs.recomendet_type \
        ";
    
    where_offers += "WHERE (";
    unsigned int ic = 1;
    for (auto i=ret.rbegin(); i != ret.rend() ; ++i)
    {
        std::vector<std::string> par;
        boost::split(par, *i, boost::is_any_of("~"));
        if (!par.empty() && par.size() >= 3)
        {
            where_offers += "(ofrs.account='"+ par[2] +"' AND ofrs.target='"+ par[1] +"' AND ofrs.retid='"+ par[0] +"')";
            if (ic++ < ret.size())
            {
                where_offers += " OR ";
            }
        }

    }
    where_offers += ") AND ofrs.campaignId IN (SELECT id FROM "+ tmpTable->str() +" WHERE retargeting = 1 and retargeting_type = 'offer' ) ";

    bzero(cmd,sizeof(cmd));
    sqlite3_snprintf(len, cmd, offerSqlStr.c_str(),
                     select_field.c_str(),
                     where_offers.c_str(),
                     order_offers.c_str(),
                     limit_offers.c_str());
    try
    {

        pStmt = new Kompex::SQLiteStatement(cfg->pDb->pDatabase);

        pStmt->Sql(cmd);

        while(pStmt->FetchRow())
        {

            std::map<const unsigned long, bool>::iterator it= retargeting_exclude_offers_map.find(pStmt->GetColumnInt64(0));
            if( it != retargeting_exclude_offers_map.end() )
            {
                continue;
            }
            recomendet_type = "all";
            recomendet_count = 10;
            Offer *off = new Offer(pStmt->GetColumnInt64(0),     //id
                                   pStmt->GetColumnInt64(1),    //campaignId
                                   pStmt->GetColumnInt(2),      //type
                                   pStmt->GetColumnDouble(3) - count,  //rating
                                   pStmt->GetColumnInt(4),    //uniqueHits
                                   pStmt->GetColumnInt(5),     //height
                                   pStmt->GetColumnInt(6),     //width
                                   pStmt->GetColumnBool(7),    //isOnClick
                                   pStmt->GetColumnBool(8),    //social
                                   pStmt->GetColumnString(9),  //account_id
                                   pStmt->GetColumnInt(10),      //offer_by_campaign_unique
                                   pStmt->GetColumnString(11),   //recomendet
                                   pStmt->GetColumnString(12),   //retid
                                   "offer",                     //retargeting type
                                   pStmt->GetColumnBool(13),    // retargeting 
                                   pStmt->GetColumnBool(14),    //brending
                                   false,                       //isrecomendet
                                   pStmt->GetColumnBool(16)    //notification
                                  );
            itemsR.insert(Offer::Pair(off->id_int,off));
            std::string qr = "";
            if(off->Recommended != "")
            {
                day = 0;
                std::string::size_type sz;
                long oi = std::stol (pStmt->GetColumnString(12),&sz);
                std::map<const unsigned long, int>::iterator it= retargeting_offers_day.find(oi);
                if( it != retargeting_offers_day.end() )
                {
                    day = it->second;
                }
                recomendet_count = pStmt->GetColumnInt(17);
                recomendet_type = pStmt->GetColumnString(18);
                if ( recomendet_type == "min")
                {
                    if (recomendet_count - day > 1)
                    {
                        recomendet_count = recomendet_count - day;
                    }
                    else
                    {
                        recomendet_count = 1;
                    }
                }
                else if ( recomendet_type == "max")
                {
                    if (1 + day < recomendet_count)
                    {
                        recomendet_count = 1 + day;
                    }
                }
                else
                {
                    if (recomendet_count < 1)
                    {
                        recomendet_count = 1;
                    }
                }
                temp_recommended.clear();
                boost::split(temp_recommended, off->Recommended, boost::is_any_of(","));
                if (temp_recommended.begin()+recomendet_count < temp_recommended.end())
                {
                    temp_recommended.erase(temp_recommended.begin()+recomendet_count, temp_recommended.end());
                }
                qr = "(ofrs.account='" + off->account_id + "' AND ofrs.retid IN (" + boost::algorithm::join(temp_recommended, ", ") + " ))";
                VRecommended.push_back(qr);
            }
            result = true;
            count++;
        }

        pStmt->FreeQuery();

        delete pStmt;
    }
    catch(Kompex::SQLiteException &ex)
    {
        std::clog<<"["<<pthread_self()<<"]"<<__func__<<" error: "
                 <<ex.GetString()
                 <<" \n"
                 <<cmd
                 <<params->get_.c_str()
                 <<params->post_.c_str()
                 <<std::endl;

    }
    if (VRecommended.size() > 0)
    {
        try
        {   
            where_offers = "";
            order_offers = ""; 
            limit_offers = "LIMIT 100"; 
            where_offers = "WHERE ( " + boost::algorithm::join(VRecommended, " OR ") + ")";
            bzero(cmd,sizeof(cmd));
            sqlite3_snprintf(len, cmd, offerSqlStr.c_str(),
                             select_field.c_str(),
                             where_offers.c_str(),
                             order_offers.c_str(),
                             limit_offers.c_str());

            pStmt = new Kompex::SQLiteStatement(cfg->pDb->pDatabase);

            pStmt->Sql(cmd);

            while(pStmt->FetchRow())
            {
                std::map<const unsigned long, bool>::iterator it= retargeting_exclude_offers_map.find(pStmt->GetColumnInt64(0));
                if( it != retargeting_exclude_offers_map.end() )
                {
                    continue;
                }
                Offer *off = new Offer(pStmt->GetColumnInt64(0),     //id
                                       pStmt->GetColumnInt64(1),    //campaignId
                                       pStmt->GetColumnInt(2),      //type
                                       pStmt->GetColumnDouble(3) - count,  //rating
                                       pStmt->GetColumnInt(4),    //uniqueHits
                                       pStmt->GetColumnInt(5),     //height
                                       pStmt->GetColumnInt(6),     //width
                                       pStmt->GetColumnBool(7),    //isOnClick
                                       pStmt->GetColumnBool(8),    //social
                                       pStmt->GetColumnString(9),  //account_id
                                       pStmt->GetColumnInt(10),      //offer_by_campaign_unique
                                       "",   //recomendet
                                       pStmt->GetColumnString(12),   //retid
                                       "offer",                          //retargeting type
                                       pStmt->GetColumnBool(13),    // retargeting 
                                       false,    //brending
                                       pStmt->GetColumnBool(15),    //isrecomendet
                                       false    //notification
                                      );
                itemsR.insert(Offer::Pair(off->id_int,off));
                count++;
            }

            pStmt->FreeQuery();

            delete pStmt;
        }
        catch(Kompex::SQLiteException &ex)
        {
            std::clog<<"["<<pthread_self()<<"]"<<__func__<<" error: "
                     <<ex.GetString()
                     <<" \n"
                     <<cmd
                     <<params->get_.c_str()
                     <<params->post_.c_str()
                     <<std::endl;

        }
    }
    
    for(auto i = itemsR.begin(); i != itemsR.end(); i++)
    {
        items.insert(Offer::Pair((*i).first,(*i).second));
    }
    
    itemsR.clear();
    #ifdef DEBUG
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("Time %s taken: %lld \n", __func__,  microseconds);
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
    return result;
}


bool Core_DataBase::getRetargetingAccount(Offer::Map &items, Params *_params)
{
    bool result = false;
    #ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
    Kompex::SQLiteStatement *pStmt;
    params = _params;
    Offer::Map itemsR;
    std::vector<std::string> retargeting_accounts;
    std::map<std::string,std::pair <int,int>> map_user_accounts_;
    std::map<std::string,int> cost_accounts;
    std::map<std::string,int> gender_accounts;
    std::string offerSqlStr = "\
                    SELECT %s\
                    FROM OfferA AS ofrs\
                    %s %s %s ;\
                   ";
    std::string where_offers = "";
    std::string order_offers = ""; 
    std::string limit_offers = "LIMIT " + std::to_string(informer->capacity * 5);
    int count = 0;
    std::string select_field ="\
        ofrs.id,\
        ofrs.campaignId,\
        ofrs.type,\
        1000000.0,\
        ofrs.UnicImpressionLot,\
        ofrs.height,\
        ofrs.width,\
        ofrs.isOnClick,\
        ofrs.social,\
        ofrs.account, \
        ofrs.offer_by_campaign_unique,\
        '',\
        0,\
        1,\
        '', \
        1,\
        ofrs.html_notification\
        ";
    
    where_offers = "WHERE ofrs.campaignId IN (SELECT id FROM "+ tmpTable->str() +" WHERE retargeting = 1 and retargeting_type = 'account' ) AND ofrs.social = 0"; 
    if (!params->getAccountRetargetingExcludedOffersString().empty())
    {
        where_offers = "WHERE ofrs.campaignId IN (SELECT id FROM "+ tmpTable->str() +" WHERE retargeting = 1 and retargeting_type = 'account' )\
                        AND ofrs.social = 0  AND ofrs.id NOT IN ("+ params->getAccountRetargetingExcludedOffersString() +")";
    }

    bzero(cmd,sizeof(cmd));
    sqlite3_snprintf(len, cmd, offerSqlStr.c_str(),
                     select_field.c_str(),
                     where_offers.c_str(),
                     order_offers.c_str(),
                     limit_offers.c_str());
    try
    {

        pStmt = new Kompex::SQLiteStatement(cfg->pDb->pDatabase);

        pStmt->Sql(cmd);

        while(pStmt->FetchRow())
        {
            Offer *off = new Offer(pStmt->GetColumnInt64(0),     //id
                                   pStmt->GetColumnInt64(1),    //campaignId
                                   pStmt->GetColumnInt(2),      //type
                                   pStmt->GetColumnDouble(3) - count,  //rating
                                   pStmt->GetColumnInt(4),    //uniqueHits
                                   pStmt->GetColumnInt(5),     //height
                                   pStmt->GetColumnInt(6),     //width
                                   pStmt->GetColumnBool(7),    //isOnClick
                                   pStmt->GetColumnBool(8),    //social
                                   pStmt->GetColumnString(9),  //account_id
                                   pStmt->GetColumnInt(10),      //offer_by_campaign_unique
                                   pStmt->GetColumnString(11),   //recomendet
                                   pStmt->GetColumnString(12),   //retid
                                   "account",                     //retargeting type
                                   pStmt->GetColumnBool(13),    // retargeting 
                                   pStmt->GetColumnBool(14),    //brending
                                   false,                       //isrecomendet
                                   pStmt->GetColumnBool(16)    //notification
                                  );
            itemsR.insert(Offer::Pair(off->id_int,off));
            result = true;
            count++;
        }

        pStmt->FreeQuery();

        delete pStmt;
    }
    catch(Kompex::SQLiteException &ex)
    {
        std::clog<<"["<<pthread_self()<<"]"<<__func__<<" error: "
                 <<ex.GetString()
                 <<" \n"
                 <<cmd
                 <<params->get_.c_str()
                 <<params->post_.c_str()
                 <<std::endl;

    }
    
    for(auto i = itemsR.begin(); i != itemsR.end(); i++)
    {
        items.insert(Offer::Pair((*i).first,(*i).second));
    }
    
    itemsR.clear();
    #ifdef DEBUG
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("Time %s taken: %lld \n", __func__,  microseconds);
        printf("%s\n","------------------------------------------------------------------");
    #endif // DEBUG
    return result;
}
