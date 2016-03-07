#include <sstream>

#include "Offer.h"
#include "Log.h"
#include "Config.h"
#include "json.h"
#include "../config.h"

Offer::Offer(unsigned long long id_int,
             unsigned long long campaign_id,
             int type_int,
             float rating,
             int uniqueHits,
             int height,
             int width,
             bool isOnClick,
             bool social,
             std::string account_id,
             int unique_by_campaign,
             std::string Recommended,
             std::string retid,
             std::string retargeting_type,
             bool retargeting,
             bool brending,
             bool is_recommended,
             bool html_notification
             ):
    id_int(id_int),
    campaign_id(campaign_id),
    type(typeFromInt(type_int)),
#ifndef DUMMY
    branch(EBranchL::L30),
#else
    branch(EBranchL::L0),
#endif
    rating(rating),
    uniqueHits(uniqueHits),
    height(height),
    width(width),
    isOnClick(isOnClick),
    social(social),
    account_id(account_id),
    unique_by_campaign((unsigned)unique_by_campaign),
    Recommended(Recommended),
    retid(retid),
    retargeting_type(retargeting_type),
    retargeting(retargeting),
    brending(brending),
    is_recommended(is_recommended),
    html_notification(html_notification)
{
}

Offer::~Offer()
{
}


bool Offer::setBranch(const EBranchT tbranch)
{
    switch(tbranch)
    {
    case EBranchT::T1:
        switch(type)
        {
        case Type::banner:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L7;
                return true;
            case false:
                branch = EBranchL::L2;
                rating = 1000 * rating;
                return true;
            }
            break;
        case Type::teazer:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L17;
                return true;
            case false:
                branch = EBranchL::L12;
                rating = 1000 * rating;
                return true;
            }
            break;
        default:
            return false;
        }
        break;
    case EBranchT::T2:
        switch(type)
        {
        case Type::banner:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L8;
                return true;
            case false:
                branch = EBranchL::L3;
                rating = 1000 * rating;
                return true;
            }
            break;
        case Type::teazer:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L18;
                return true;
            case false:
                branch = EBranchL::L13;
                return true;
            }
            break;
        default:
            return false;
        }
        break;
    case EBranchT::T3:
        switch(type)
        {
        case Type::banner:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L4;
                rating = 1000 * rating;
                return true;
            case false:
                branch = EBranchL::L3;
                rating = 1000 * rating;
                return true;
            }
            break;
        case Type::teazer:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L19;
                return true;
            case false:
                branch = EBranchL::L14;
                return true;
            }
            break;
        default:
            return false;
        }
        break;
    case EBranchT::T4:
        switch(type)
        {
        case Type::banner:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L10;
                return true;
            case false:
                branch = EBranchL::L5;
                rating = 1000 * rating;
                return true;
            }
            break;
        case Type::teazer:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L20;
                return true;
            case false:
                branch = EBranchL::L15;
                return true;
            }
            break;
        default:
            return false;
        }
        break;
    case EBranchT::T5:
        switch(type)
        {
        case Type::banner:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L11;
                return true;
            case false:
                branch = EBranchL::L6;
                rating = 1000 * rating;
                return true;
            }
            break;
        case Type::teazer:
            switch(isOnClick)
            {
            case true:
                branch = EBranchL::L21;
                return true;
            case false:
                branch = EBranchL::L16;
                return true;
            }
            break;
        default:
            return false;
        }
        break;
    case EBranchT::TMAX:
        return false;
    }

    return false;
}

