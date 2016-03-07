ACLOCAL_AMFLAGS = -I m4

SRC_ALL=src/base64.cpp \
		src/Core.cpp \
		src/GeoRerions.cpp \
		src/Log.cpp \
		src/BaseCore.cpp \
		src/DataBase.cpp \
		src/HistoryManager.cpp \
		src/Informer.cpp \
		src/main.cpp \
		src/Server.cpp \
		src/Campaign.cpp \
		src/DB.cpp \
		src/Offer.cpp \
		src/CgiService.cpp \
		src/EBranch.cpp \
		src/json.cpp \
		src/ParamParse.cpp \
		src/UrlParser.cpp \
		src/Config.cpp \
		src/KompexSQLiteDatabase.cpp \
		src/Params.cpp \
		src/Cookie.cpp \
		src/GeoIPTools.cpp \
		src/KompexSQLiteStatement.cpp \
		src/ParentDB.cpp \
		src/BoostHelpers.cpp \
		src/TempTable.cpp \
		src/Core_DataBase.cpp \
		src/CpuStat.cpp 

CMN_CPPFLAGS=-std=c++11 -Iinclude
CMN_LDADD = $(BOOST_DATE_TIME_LIBS) $(BOOST_FILESYSTEM_LIBS) $(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_REGEX_LIBS)

if DEBUG
bin_PROGRAMS = workerd-log-dev
workerd_dev_SOURCES=$(SRC_ALL)
workerd_dev_CPPFLAGS=$(CMN_CPPFLAGS)
workerd_dev_LDADD=$(CMN_LDADD)
AM_CFLAGS = -g -DDEBUG -Wall -pipe
AM_CXXFLAGS = -g -DDEBUG -Wall -pipe
MAKEINIT=0
else
bin_PROGRAMS = workerd-log
workerd_SOURCES=$(SRC_ALL)
workerd_CPPFLAGS=$(CMN_CPPFLAGS)
workerd_LDADD=$(CMN_LDADD)
AM_CFLAGS = -m64 -Ofast -march=native -funroll-loops -pipe
AM_CXXFLAGS = -m64 -Ofast -march=native -funroll-loops -pipe
MAKEINIT=1
endif

DEBDEP=bash (>= 2.05a-11), awk, procps (>= 1:2.0.7-8), sed (>= 3.02-8), grep (>= 2.4.2-3), coreutils (>= 5.0-5), libtinyxml2.6.2 (>=2.6.2), libfcgi0ldbl (>=2.4.0), geoip-database (>=20130213-1), libboost-system1.49.0 (>=1.49.0), libboost-regex1.49.0 (>=1.49.0), libboost-thread1.49.0 (>=1.49.0), libboost-filesystem1.49.0 (>=1.49.0), libboost-date-time1.49.0 (>=1.49.0)

debian_BUILD = $(bin_PROGRAMS)-$(VERSION)

dist_doc_DATA=README

deb: $(bin_PROGRAMS)
	rm -rf $(debian_BUILD)
	$(MKDIR_P) $(debian_BUILD)/DEBIAN
	echo "Package: $(bin_PROGRAMS)">  $(debian_BUILD)/DEBIAN/control
	echo "Version: $(VERSION)" >> $(debian_BUILD)/DEBIAN/control
	echo "Section: base" >> $(debian_BUILD)/DEBIAN/control
	echo "Priority: optional" >> $(debian_BUILD)/DEBIAN/control
	echo "Architecture: all" >> $(debian_BUILD)/DEBIAN/control
	echo "Depends: $(DEBDEP)" >> $(debian_BUILD)/DEBIAN/control
	echo "Maintainer: Alex D <wvdial@gmail.com>" >> $(debian_BUILD)/DEBIAN/control
	echo "Description:  multithread demon, to return html(inframe) with offers. Runs on background and listen unix socket(config.xml) ready to http requests from nginx." >> $(debian_BUILD)/DEBIAN/control
	@DOLLAR_SIGN@(call install_f,$(debian_BUILD))
	for p in `find $(debian_BUILD) -type f -print | grep -v DEBIAN`; do \
	md5sum $$p | $(SED) 's/$(debian_BUILD)\///'>>$(debian_BUILD)/DEBIAN/md5sum; \
	done
	dpkg-deb --build $(debian_BUILD)
	rm -rf $(debian_BUILD)
