#include <unistd.h>

#include "TempTable.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "Config.h"

#define CMD_SIZE 2621440

TempTable::TempTable(char *cmd, size_t len):
    cmd(cmd),
    len(len)
{
    tmpTableName = "tmp" + std::to_string((long long int)getpid()) + std::to_string((long long int)pthread_self());

    Kompex::SQLiteStatement *p;
    p = new Kompex::SQLiteStatement(cfg->pDb->pDatabase);
    try
    {
        bzero(cmd,sizeof(cmd));
        sqlite3_snprintf(len, cmd, "\
             CREATE TABLE IF NOT EXISTS %s(\
             id INT8 PRIMARY KEY,\
             retargeting_type VARCHAR(10),\
             account VARCHAR(64),\
             cost SMALLINT,\
             gender SMALLINT,\
             retargeting SMALLINT\
            ) WITHOUT ROWID;\
            ", tmpTableName.c_str());
        p->SqlStatement(cmd);
    }
    catch(Kompex::SQLiteException &ex)
    {
        std::clog<<__func__<<" error: create tmp table: %s"<< ex.GetString()<<std::endl;
        exit(1);
    }
    try
    {
        
        bzero(cmd,sizeof(cmd));
        sqlite3_snprintf(len, cmd, "\
            CREATE INDEX IF NOT EXISTS idx_%s_campaign_id_retargeting_retargeting_type ON %s (id, retargeting, retargeting_type);\
            ", tmpTableName.c_str(), tmpTableName.c_str());
        p->SqlStatement(cmd);
    }
    catch(Kompex::SQLiteException &ex)
    {
        std::clog<<__func__<<" error: create tmp table: %s"<< ex.GetString()<<std::endl;
        exit(1);
    }
    delete p;
}

TempTable::~TempTable()
{
}

bool TempTable::clear()
{
    Kompex::SQLiteStatement *pStmt;

    pStmt = new Kompex::SQLiteStatement(cfg->pDb->pDatabase);
    bzero(cmd,sizeof(cmd));
    sqlite3_snprintf(len,cmd,"DELETE FROM %s;",tmpTableName.c_str());
    try
    {
        pStmt->SqlStatement(cmd);
        pStmt->FreeQuery();
    }
    catch(Kompex::SQLiteException &ex)
    {
        std::clog<<__func__<<" error: " <<ex.GetString()<<std::endl;
    }
    delete pStmt;
    return true;
}
