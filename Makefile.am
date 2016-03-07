ACLOCAL_AMFLAGS = -I m4

SRC_ALL=src/base64.cpp \
		src/Core.cpp \
		src/Log.cpp \
		src/BaseCore.cpp \
		src/main.cpp \
		src/Server.cpp \
		src/DB.cpp \
		src/Offer.cpp \
		src/CgiService.cpp \
		src/EBranch.cpp \
		src/json.cpp \
		src/ParamParse.cpp \
		src/UrlParser.cpp \
		src/Config.cpp \
		src/Params.cpp \
		src/Cookie.cpp \
		src/GeoIPTools.cpp \
		src/BoostHelpers.cpp \
		src/CpuStat.cpp 


CMN_CPPFLAGS=-std=c++11 -Iinclude
CMN_LDADD = $(BOOST_DATE_TIME_LIBS) $(BOOST_FILESYSTEM_LIBS) $(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_REGEX_LIBS)


if DEBUG
bin_PROGRAMS = workerd-log-dev
workerd_dev_SOURCES=$(SRC_ALL)
workerd_dev_CPPFLAGS=$(CMN_CPPFLAGS)
workerd_dev_LDADD=$(CMN_LDADD)
else
bin_PROGRAMS = workerd-log
workerd_SOURCES=$(SRC_ALL)
workerd_CPPFLAGS=$(CMN_CPPFLAGS)
workerd_LDADD=$(CMN_LDADD)
endif

DEBDEP=bash (>= 2.05a-11), awk, procps (>= 1:2.0.7-8), sed (>= 3.02-8), grep (>= 2.4.2-3), coreutils (>= 5.0-5), libtinyxml2.6.2 (>=2.6.2), libfcgi0ldbl (>=2.4.0),  geoip-database (>=20130213-1), libboost-system1.49.0 (>=1.49.0), libboost-regex1.49.0 (>=1.49.0), libboost-thread1.49.0 (>=1.49.0), libboost-filesystem1.49.0 (>=1.49.0), libboost-date-time1.49.0 (>=1.49.0)

if DEBUG
AM_CFLAGS = -g -DDEBUG -Wall -pipe
AM_CXXFLAGS = -g -DDEBUG -Wall -pipe
else
AM_CFLAGS = -m64 -Ofast -march=native -funroll-loops -pipe
AM_CXXFLAGS = -m64 -Ofast -march=native -funroll-loops -pipe
endif


dist_doc_DATA=README
