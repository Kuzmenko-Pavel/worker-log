#ifndef DB_H
#define DB_H
#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>
#include "../config.h"
#include "Log.h"
#include <string>
#include <utility>
#include <map>

namespace mongocxx
{
	class DB
	{
	public:
	    class ConnectionOptions
	    {
		public:
		ConnectionOptions() : slave_ok(false) {}
		std::string database;
		std::string server_host;
		std::string replica_set;
		bool slave_ok;
	    };

	    class ReplicaSetConnection
	    {
		std::string replica_set_;
		std::string connection_string_;
	    public:
		ReplicaSetConnection(const std::string &replica_set,
				     const std::string &connection_string)
		    : replica_set_(replica_set),
		      connection_string_(connection_string) { }

		std::string replica_set() const
		{
		    return replica_set_;
		}
		std::string connection_string() const
		{
		    return connection_string_;
		}
	    };
	public:
	    DB(const std::string &name = std::string());
	    ~DB();
	    static void addDatabase(const std::string &name,
				    const std::string &server_host,
				    const std::string &database,
				    bool slave_ok);

	    static void addDatabase(const std::string &name,
				    const ReplicaSetConnection &connection_string,
				    const std::string &database,
				    bool slave_ok);

	    static void addDatabase(const std::string &server_host,
				    const std::string &database,
				    bool slave_ok);

	    static void addDatabase(const ReplicaSetConnection &connection_string,
				    const std::string &database, bool slave_ok);

	    std::string collect(const std::string &collection);

	    //bool createCollection(const string &coll, long long size = 0,
	    //			  bool capped = false, int max = 0, BSONObj *info = 0) {
	    //	return (*db_)->createCollection(this->collect(coll), size,
	    //				     capped, max, info);
	    //  }

	    std::string &database();

	    std::string &server_host();

	    std::string &replica_set();

	    bool slave_ok();

	    mongocxx::database db(const std::string &name = std::string());

	    static bool ConnectLogDatabase();

	    //void insert( const string &coll, BSONObj obj, bool safe = false );

	    class NotRegistered : public std::exception
	    {
	    public:
		NotRegistered(const std::string &str) : error_message(str) { }
		NotRegistered(const char *str) : error_message(str) { }
		virtual ~NotRegistered() throw() { }
		virtual const char *what() const throw()
		{
		    return error_message.c_str();
		}
	    private:
		std::string error_message;
	    };

	private:
	    mongocxx::pool *db_;
	    static bool fConnectedToLogDatabase;


	protected:
	    typedef std::map<std::string, ConnectionOptions*> ConnectionOptionsMap;
	    static ConnectionOptionsMap connection_options_map_;
	    ConnectionOptions *options_;

	    static ConnectionOptions *options_by_name(const std::string &name);

	    static void _addDatabase(const std::string &name,
				     const std::string &server_host,
				     const std::string &database,
				     const std::string &replica_set,
				     bool slave_ok);
	};

}  // end namespace mongo
#endif // DB_H
