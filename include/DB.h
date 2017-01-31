#ifndef DB_H
#define DB_H
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

mongocxx::instance instance{};
mongocxx::pool pool;
pool = mongocxx::pool{mongocxx::uri{}};

#endif // DB_H
