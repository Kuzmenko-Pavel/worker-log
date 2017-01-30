#!/bin/bash
#tar zxvf install/mongodb-fix.tar.gz -C /
#apt-get install automake libtool libtinyxml2-dev libtinyxml-dev
git log --oneline --decorate --color > ChangeLog
git log --format='%aN <%aE>' | sort -f | uniq > AUTHORS
libtoolize --install --copy --force --automake
aclocal -I m4 --install
autoconf
autoheader
automake --add-missing 
./configure --enable-debug
make clean
make
