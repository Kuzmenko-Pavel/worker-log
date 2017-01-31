ACLOCAL_AMFLAGS = -I m4

SRC_ALL=src/base64.cpp \
		src/Core.cpp \
		src/Log.cpp \
		src/BaseCore.cpp \
		src/main.cpp \
		src/Server.cpp \
		src/Offer.cpp \
		src/CgiService.cpp \
		src/EBranch.cpp \
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
bin_PROGRAMS = worker-log-dev
worker_log_dev_SOURCES=$(SRC_ALL)
worker_log_dev_CPPFLAGS=$(CMN_CPPFLAGS)
worker_log_dev_LDADD=$(CMN_LDADD)
else
bin_PROGRAMS = worker-log
worker_log_SOURCES=$(SRC_ALL)
worker_log_CPPFLAGS=$(CMN_CPPFLAGS)
worker_log_LDADD=$(CMN_LDADD)
endif


if DEBUG
AM_CFLAGS = -g -DDEBUG -Wall -pipe
AM_CXXFLAGS = -g -DDEBUG -Wall -pipe
MAKEINIT=0
else
AM_CFLAGS = -m64 -Ofast -march=native -funroll-loops -pipe
AM_CXXFLAGS = -m64 -Ofast -march=native -funroll-loops -pipe
MAKEINIT=1
endif
