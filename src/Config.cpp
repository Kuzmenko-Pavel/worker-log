//#include <iostream>
#include "../config.h"
#include <boost/regex.hpp>
#include <fstream>

#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <assert.h>

#include "Log.h"
#include "Config.h"
#include "BoostHelpers.h"


unsigned long request_processed_;
unsigned long last_time_request_processed;
unsigned long offer_processed_;
unsigned long social_processed_;
unsigned long retargeting_processed_;

// Global static pointer used to ensure a single instance of the class.
Config* Config::mInstance = NULL;

Config* Config::Instance()
{
    if (!mInstance)   // Only allow one instance of class to be generated.
        mInstance = new Config();

    return mInstance;
}

Config::Config()
{
    mIsInited = false;
}

bool Config::LoadConfig(const std::string fName)
{
    mFileName = fName;
    return Load();
}

void Config::exit(const std::string &mes)
{
    std::cerr<<mes<<std::endl;
    std::clog<<mes<<std::endl;
    ::exit(1);
}

bool Config::Load()
{
    TiXmlDocument *mDoc;
    TiXmlElement *mRoot, *mElem, *mel, *mels;

    std::clog<<"open config file:"<<mFileName;

    mIsInited = false;

    if((cfgFilePath = BoostHelpers::getConfigDir(mFileName)).empty())
    {
        return false;
    }

    std::clog<<" config dir:"<<cfgFilePath<<std::endl;

    if(!(mDoc = new TiXmlDocument(mFileName)))
    {
        exit("error create TiXmlDocument object");
    }


    if(!mDoc->LoadFile())
    {
        exit("load file: "+mFileName+
             " error: "+ mDoc->ErrorDesc()+
             " row: "+std::to_string(mDoc->ErrorRow())+
             " col: "+std::to_string(mDoc->ErrorCol()));
    }

    mRoot = mDoc->FirstChildElement("root");

    if(!mRoot)
    {
        exit("does not found root section in file: "+mFileName);
    }

    instanceId = atoi(mRoot->Attribute("id"));

    if( (mels = mRoot->FirstChildElement("mongo")) )
    {
        if( (mel = mels->FirstChildElement("log")) )
        {
            for(mElem = mel->FirstChildElement("host"); mElem; mElem = mElem->NextSiblingElement("host"))
            {
                mongo_log_host_.push_back(mElem->GetText());
            }

            if( (mElem = mel->FirstChildElement("db")) && (mElem->GetText()) )
            {
                mongo_log_db_ = mElem->GetText();
            }

            if( (mElem = mel->FirstChildElement("set")) && (mElem->GetText()) )
                mongo_log_set_ = mElem->GetText();

            if( (mElem = mel->FirstChildElement("slave")) && (mElem->GetText()) )
                mongo_log_slave_ok_ = strncmp(mElem->GetText(),"1",1)>=0 ? true : false;

            if( (mElem = mel->FirstChildElement("login")) && (mElem->GetText()) )
                mongo_log_login_ = mElem->GetText();

            if( (mElem = mel->FirstChildElement("passwd")) && (mElem->GetText()) )
                mongo_log_passwd_ = mElem->GetText();

            if( (mElem = mel->FirstChildElement("collection_impression")) && (mElem->GetText()) )
                mongo_log_collection_impression_ = mElem->GetText();

            if( (mElem = mel->FirstChildElement("collection_block")) && (mElem->GetText()) )
                mongo_log_collection_block_ = mElem->GetText();
        }
        else
        {
            exit("no log section in mongo in config file. exit");
        }
    }
    else
    {
        exit("no mongo section in config file. exit");
    }


    //main config
    if( (mElem = mRoot->FirstChildElement("server")) )
    {
        if( (mel = mElem->FirstChildElement("ip")) && (mel->GetText()) )
        {
            server_ip_ = mel->GetText();
        }

        if( (mel = mElem->FirstChildElement("geocity_path")) && (mel->GetText()) )
        {
            geocity_path_ = mel->GetText();

            if(!BoostHelpers::checkPath(geocity_path_, false, true))
            {
                ::exit(1);
            }
        }
        if( (mel = mElem->FirstChildElement("geocountry_path")) && (mel->GetText()) )
        {
            geocountry_path_ = mel->GetText();

            if(!BoostHelpers::checkPath(geocity_path_, false, true))
            {
                ::exit(1);
            }
        }

        if( (mel = mElem->FirstChildElement("socket_path")) && (mel->GetText()) )
        {
            server_socket_path_ = mel->GetText();

            if(BoostHelpers::checkPath(server_socket_path_, true, true))
            {
                std::clog<<"server socket path: "<<server_socket_path_<<" exists"<<std::endl;
                unlink(server_socket_path_.c_str());
            }
        }

        if( (mel = mElem->FirstChildElement("children")) && (mel->GetText()) )
        {
            server_children_ = atoi(mel->GetText());
        }

        if( (mel = mElem->FirstChildElement("lock_file")) && (mel->GetText()) )
        {
            lock_file_ = mel->GetText();

            if(!BoostHelpers::checkPath(lock_file_,true, true))
            {
                ::exit(1);
            }
        }

        if( (mel = mElem->FirstChildElement("pid_file")) && (mel->GetText()) )
        {
            pid_file_ = mel->GetText();

            if(!BoostHelpers::checkPath(pid_file_,true, true))
            {
                ::exit(1);
            }
        }


        if( (mel = mElem->FirstChildElement("user")) && (mel->GetText()) )
        {
            user_ = mel->GetText();
        }

        if( (mel = mElem->FirstChildElement("group")) && (mel->GetText()) )
        {
            group_ = mel->GetText();
        }

        if( (mel = mElem->FirstChildElement("time_update")) && (mel->GetText()) )
        {
            if((time_update_ = BoostHelpers::getSeconds(mel->GetText())) == -1)
            {
                exit("Config::Load: no time match in config.xml element: time_update");
            }
        }

        if( (mel = mElem->FirstChildElement("cookie")) )
        {
            if( (mels = mel->FirstChildElement("name")) && (mels->GetText()) )
                cookie_name_ = mels->GetText();
            else
            {
                exit("element cookie_name is not inited");
            }
            if( (mels = mel->FirstChildElement("domain")) && (mels->GetText()) )
                cookie_domain_ = mels->GetText();
            else
            {
                exit("element cookie_domain is not inited");
            }

            if( (mels = mel->FirstChildElement("path")) && (mels->GetText()) )
                cookie_path_ = mels->GetText();
            else
            {
                exit("element cookie_path is not inited");
            }
        }
    }
    else
    {
        exit("no server section in config file. exit");
    }

    if( (mElem = mRoot->FirstChildElement("log")) )
    {
        if( (mel = mElem->FirstChildElement("coretime")) && (mel->GetText()) )
        {
            logCoreTime = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("outsize")) && (mel->GetText()) )
        {
            logOutPutSize = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("ip")) && (mel->GetText()) )
        {
            logIP = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("country")) && (mel->GetText()) )
        {
            logCountry = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("region")) && (mel->GetText()) )
        {
            logRegion = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("cookie")) && (mel->GetText()) )
        {
            logCookie = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("context")) && (mel->GetText()) )
        {
            logContext = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("search")) && (mel->GetText()) )
        {
            logSearch = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("informerId")) && (mel->GetText()) )
        {
            logInformerId = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("location")) && (mel->GetText()) )
        {
            logLocation = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("sphinx")) && (mel->GetText()) )
        {
            logSphinx = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("RetargetingOfferIds")) && (mel->GetText()) )
        {
            logRetargetingOfferIds = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("OutPutOfferIds")) && (mel->GetText()) )
        {
            logOutPutOfferIds = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("monitor")) && (mel->GetText()) )
        {
            logMonitor = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }

        if( (mel = mElem->FirstChildElement("commands")) && (mel->GetText()) )
        {
            logMonitor = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }
        if( (mel = mElem->FirstChildElement("redis")) && (mel->GetText()) )
        {
            logRedis = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }
        if( (mel = mElem->FirstChildElement("logMQ")) && (mel->GetText()) )
        {
            logMQ = strncmp(mel->GetText(),"1",1)>=0 ? true : false;
        }
    }
    else
    {
        logCoreTime = logOutPutSize = logIP = true;

        logSphinx = logOutPutOfferIds = logRetargetingOfferIds =
        logLocation = logInformerId = logSearch =
        logContext = logCookie = logCountry = logRegion = logMonitor = logMQ = logRedis = false;

        std::clog<<"using default log mode: CoreTime OutPutSize IP"<<std::endl;
    }

    request_processed_ = 0;
    last_time_request_processed = 0;
    offer_processed_ = 0;
    social_processed_ = 0;
    retargeting_processed_ = 0;

    mIsInited = true;
    return mIsInited;
}

//---------------------------------------------------------------------------------------------------------------
Config::~Config()
{
    mInstance = NULL;
}
//---------------------------------------------------------------------------------------------------------------
int Config::getTime(const char *p)
{
    struct tm t;
    int ret;
    strptime(p, "%H:%M:%S", &t);
    ret = t.tm_hour * 3600;
    ret = ret + t.tm_min * 60;
    return ret + t.tm_sec;
}
//---------------------------------------------------------------------------------------------------------------
std::string Config::getFileContents(const std::string &fileName)
{
    std::ifstream in(fileName, std::ios::in | std::ios::binary);

    if(in)
    {
        std::string cnt;
        in.seekg(0, std::ios::end);
        cnt.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&cnt[0], cnt.size());
        in.close();
        return(cnt);
    }

    std::clog<<"error open file: "<<fileName<<" error number: "<<errno<<std::endl;
    return std::string();
}


void Config::minifyhtml(std::string &s) {
  boost::regex nowhitespace(
    "(?ix)"
    "(?>"           // Match all whitespans other than single space.
    "[^\\S ]\\s*"   // Either one [\t\r\n\f\v] and zero or more ws,
    "| \\s{2,}"     // or two or more consecutive-any-whitespace.
    ")"             // Note: The remaining regex consumes no text at all...
    "(?="           // Ensure we are not in a blacklist tag.
    "[^<]*+"        // Either zero or more non-"<" {normal*}
    "(?:"           // Begin {(special normal*)*} construct
    "<"             // or a < starting a non-blacklist tag.
    "(?!/?(?:textarea|pre)\\b)"
    "[^<]*+"        // more non-"<" {normal*}
    ")*+"           // Finish "unrolling-the-loop"
    "(?:"           // Begin alternation group.
    "<"             // Either a blacklist start tag.
    "(?>textarea|pre)\\b"
    "| \\z"         // or end of file.
    ")"             // End alternation group.
    ")"             // If we made it here, we are not in a blacklist tag.
  );

  boost::regex in_line("\r?\n|\r|\n");
  s = boost::regex_replace(s, nowhitespace, "");
  s = boost::regex_replace(s, in_line, "");
}
