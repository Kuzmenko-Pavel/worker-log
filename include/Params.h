#ifndef PARAMS_H
#define PARAMS_H

#include <sstream>
#include <string>
#include <boost/date_time.hpp>
#include <boost/algorithm/string/join.hpp>
#include <vector>
#include <map>

#include "json.h"

/** \brief Параметры, которые определяют показ рекламы */
class Params
{
public:
    std::string cookie_id_;
    std::string post_;
    std::string get_;
    unsigned long long key_long_;
    boost::posix_time::ptime time_;
    nlohmann::json offers_;
    nlohmann::json informer_;
    nlohmann::json params_;

    Params();
    Params &parse();

    /** Установка параметров, которые определяют показ рекламы */
    Params &cookie_id(const std::string &cookie_id);
    Params &json(const std::string &json);
    Params &get(const std::string &get);
    Params &post(const std::string &post);
    
    /** Получение параметров, которые определяют показ рекламы */
    std::string getCookieId() const;
    std::string getUserKey() const;
    unsigned long long getUserKeyLong() const;
    boost::posix_time::ptime getTime() const;

private:
    nlohmann::json json_; 
};

#endif // PARAMS_H
