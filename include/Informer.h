#ifndef INFORMER_H
#define INFORMER_H

#include <string>

typedef long long			sphinx_int64_t;
typedef unsigned long long	sphinx_uint64_t;

/**
    \brief Класс, описывающий рекламную выгрузку
*/
class Informer
{
public:
       long long id;                            //Индентификатор РБ
        std::string title;
        unsigned int capacity;                      //Количество мест под тизер
        std::string bannersCss;                 //Стиль CSS РБ для отображения банеров
        std::string teasersCss;                 //Стиль CSS РБ для отображения тизеров
        std::string headerHtml;
        std::string footerHtml;
        long domainId;
        std::string domain;
        long accountId;
        std::string account;
        unsigned retargeting_capacity;
        double range_short_term, range_long_term, range_context, range_search, range_category;

        bool blocked;                           //Статус активности РБ
        std::string nonrelevant;            //Что отображать при отсутствии платных РП
        std::string user_code;                  //Строка пользовательского кода
        bool valid;                             //Валидность блока
        bool html_notification;                 
        bool plase_branch;                      
        bool retargeting_branch;                
        int height;                             //Высота блока
        int width;                              //Ширина блока
        int height_banner;                      //Высота отображаемых банеров
        int width_banner;                       //Ширина отображаемых банеров

    Informer(long id);
    Informer(long id, const std::string &title,
            unsigned int capacity,
            const std::string &bannersCss,
            const std::string &teasersCss,
            const std::string &headerHtml,
            const std::string &footerHtml,
            long, const std::string &domain, long, const std::string &account,
            double, double, double, double, double, int, bool, const std::string &nonrelevant, const std::string &user_code, bool, bool, bool);
    virtual ~Informer();

    bool is_null() const
    {
        return id==0;
    }

    bool operator==(const Informer &other) const;
    bool operator<(const Informer &other) const;

    bool isShortTerm() const {return range_short_term > 0;}
    bool isLongTerm() const {return range_long_term > 0;}
    bool isContext() const {return range_context > 0;}
    bool isSearch() const {return range_search > 0;}
    bool isCategory() const {return range_category > 0;}
    bool sphinxProcessEnable() const {
        return range_short_term > 0 || range_long_term > 0 || range_context > 0 || range_search > 0
        || range_category > 0;}

private:

};

#endif // INFORMER_H
