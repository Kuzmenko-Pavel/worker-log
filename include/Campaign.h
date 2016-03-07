#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include <string>

#include "KompexSQLiteDatabase.h"

//typedef long long			sphinx_int64_t;
//typedef unsigned long long	sphinx_uint64_t;

enum class showCoverage : std::int8_t { all = 1, allowed = 2, thematic = 3 };

/**
  \brief  Класс, описывающий рекламную кампанию
*/
class Campaign
{
public:
    long long id;
    std::string guid;
    std::string title;
    std::string project;
    bool social;
    showCoverage type;
    int offersCount;

    Campaign();
    Campaign(long long id);
    virtual ~Campaign();

    static std::string getName(long long campaign_id);
    static void info(std::vector<Campaign*> &res, std::string t);
    static showCoverage typeConv(const std::string &t);
    void setType(const int &t);
    std::string getType();
};

#endif // CAMPAIGN_H
