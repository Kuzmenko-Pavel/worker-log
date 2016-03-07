#include "Informer.h"

Informer::Informer(long id) :
    id(id)
{
}

Informer::Informer(long id, const std::string &title, unsigned int capacity, const std::string &bannersCss,
                   const std::string &teasersCss,
                   const std::string &headerHtml,
                   const std::string &footerHtml,
                   long domainId, const std::string &domain, long accountId, const std::string &account,
                   double range_short_term, double range_long_term,
                   double range_context, double range_search, double range_category,
                   int retargeting_capacity, bool blocked, const std::string &nonrelevant, const std::string &user_code, bool html_notification, bool plase_branch, bool retargeting_branch):
    id(id),
    title(title),
    capacity(capacity),
    bannersCss(bannersCss),
    teasersCss(teasersCss),
    headerHtml(headerHtml),
    footerHtml(footerHtml),
    domainId(domainId),
    domain(domain),
    accountId(accountId),
    account(account),
    retargeting_capacity(retargeting_capacity),
    range_short_term(range_short_term),
    range_long_term(range_long_term),
    range_context(range_context),
    range_search(range_search),
    range_category(range_category),
    blocked(blocked),
    nonrelevant(nonrelevant),
    user_code(user_code),
    html_notification(html_notification),
    plase_branch(plase_branch),
    retargeting_branch(retargeting_branch)

{
}

Informer::~Informer()
{
}

bool Informer::operator==(const Informer &other) const
{
    return this->id == other.id;
}

bool Informer::operator<(const Informer &other) const
{
    return this->id < other.id;
}
