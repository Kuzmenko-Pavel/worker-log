#include "DB.h"
#include "Config.h"

mongocxx::DB::ConnectionOptionsMap mongocxx::DB::connection_options_map_;

mongocxx::DB::DB(const std::string &name)
{
    options_ = options_by_name(name);
    if (!options_){
        throw NotRegistered("Database " +
                            (name.empty()? "(default)" : name) +
                            " is not registered! Use addDatabase() first.");
	}
    if (options_->replica_set.empty())
    {
    	db_ = new mongocxx::pool(mongocxx::uri{});
    }
    else
    {
    	db_ = new mongocxx::pool(mongocxx::uri{});
    }
}

mongocxx::DB::~DB()
{
    delete db_;
}

void mongocxx::DB::addDatabase(const std::string &name,
                            const std::string &server_host,
                            const std::string &database,
                            bool slave_ok)
{
    _addDatabase(name, server_host, database, "", slave_ok);
}

/** Регистрирует подключение \a name к набору реплик баз данных
(Replica Set). */
void mongocxx::DB::addDatabase(const std::string &name,
                            const ReplicaSetConnection &connection_string,
                            const std::string &database,
                            bool slave_ok)
{
    _addDatabase(name, connection_string.connection_string(), database,
                 connection_string.replica_set(), slave_ok);
}

/** Регистрирует базу данных "по умолчанию". */
void mongocxx::DB::addDatabase(const std::string &server_host,
                            const std::string &database,
                            bool slave_ok)
{
    _addDatabase("", server_host, database, "", slave_ok);
}

/** Регистрирует базу данных "по умолчанию", подключение осуществляется
к набору реплик (Replica Set). */
void mongocxx::DB::addDatabase(const ReplicaSetConnection &connection_string,
                            const std::string &database, bool slave_ok)
{
    _addDatabase("", connection_string.connection_string(), database,
                 connection_string.replica_set(), slave_ok);
}
/** Название используемой базы данных */
std::string &mongocxx::DB::database()
{
    return options_->database;
}

/** Адрес сервера MongoDB */
std::string &mongocxx::DB::server_host()
{
    return options_->server_host;
}

/** Название набора реплик (replica set) */
std::string &mongocxx::DB::replica_set()
{
    return options_->replica_set;
}

/** Возвращает true, если допускается read-only подключение к slave серверу в кластере */
bool mongocxx::DB::slave_ok()
{
    return options_->slave_ok;
}

/** Возвращает соединение к базе данных.
Может использоваться для операций, не предусмотренных обёрткой.
 */
mongocxx::database mongocxx::DB::db(const std::string &name)
{
    auto c = (*db_).acquire();
    return (*c)['s'];
}


mongocxx::DB::ConnectionOptions *mongocxx::DB::options_by_name(const std::string &name)
{
    ConnectionOptionsMap::const_iterator it =
        connection_options_map_.find(name);
    if (it != connection_options_map_.end())
        return it->second;
    else
        return 0;
}

/** Добавляет настройки подключения */
void mongocxx::DB::_addDatabase(const std::string &name,
                             const std::string &server_host,
                             const std::string &database,
                             const std::string &replica_set,
                             bool slave_ok)
{
    ConnectionOptions *options = options_by_name(name);
    if (!options)
        options = new ConnectionOptions;
    else
    {
        Log::warn("Database %s is already registered. Old connection will be overwritten.",
                  (name.empty()? "(default)" : name).c_str());
    };

    options->database = database;
    options->server_host = server_host;
    options->replica_set = replica_set;
    options->slave_ok = slave_ok;
    connection_options_map_[name] = options;
}

/** Создаёт подключения к базе данных логирования.
 *
 * Настройки читаются конструктором класса из переменных окружения среды.*/

bool mongocxx::DB::ConnectLogDatabase()
{

    for(auto h = cfg->mongo_log_host_.begin(); h != cfg->mongo_log_host_.end(); ++h)
    {
        Log::info("Connecting to: %s",(*h).c_str());
        try
        {
            if (cfg->mongo_log_set_.empty())
                mongocxx::DB::addDatabase( "log",
                                        *h,
                                        cfg->mongo_log_db_,
                                        cfg->mongo_log_slave_ok_);
            else
                mongocxx::DB::addDatabase( "log",
                                        mongocxx::DB::ReplicaSetConnection(
                                            cfg->mongo_log_set_,
                                            *h),
                                        cfg->mongo_log_db_,
                                        cfg->mongo_log_slave_ok_);

            /** Подготовка базы данных MongoDB.*/
            //mongocxx::DB POOL("log");
            //mongocxx::database &cl = POOL.db("log");
            //db.createCollection("log.impressions", 700*1000000, true, 1000000);
        }
        catch (std::exception const &ex)
        {
            Log::err("Error connecting to mongo: %s", ex.what());
        }
    }

    return true;
}
