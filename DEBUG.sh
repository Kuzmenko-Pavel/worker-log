#!/bin/bash
#tar zxvf install/mongodb-fix.tar.gz -C /
#apt-get install automake libtool libtinyxml2-dev libtinyxml-dev g++ libgeoip-dev
git log --oneline --decorate --color > ChangeLog
libtoolize --install --copy --force --automake
aclocal -I m4 --install
autoconf
autoheader
automake --add-missing 
./configure --enable-debug
make clean
make
