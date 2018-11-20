#!/bin/sh

SHELL_DIR=`pwd`
L_GCC=arm-linux-gnueabihf-gcc
L_GXX=arm-linux-gnueabihf-g++
L_STRIP=arm-linux-gnueabihf-strip
L_TARGET=$SHELL_DIR
L_PREFIX=$SHELL_DIR/build/arm/usr

export GCC=$L_GCC
export GXX=$L_GXX

mkdir -p $L_PREFIX


#uuid install
if [ -d $SHELL_DIR/build/arm/uuid ] ;  then
    echo "uuid is already build"
else
    mkdir -p $SHELL_DIR/build/arm/uuid
    cd $SHELL_DIR/build/arm/uuid
    cp $SHELL_DIR/lib/uuid/* ./ -rf
    autoreconf -ivf 
    chmod +x ./configure
    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX --host=arm
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        rm $SHELL_DIR/build/arm/uuid -rf
        cd $SHELL_DIR
        exit 1
    fi
fi

#modbus install
#if [ -d $SHELL_DIR/build/arm/libmodbus ] ;  then
#    echo "libmodbus is already build"
#else
#    mkdir -p $SHELL_DIR/build/arm/libmodbus
#    cd $SHELL_DIR/build/arm/libmodbus
#    cp $SHELL_DIR/lib/libmodbus/* ./ -rf
#    sudo autoreconf -ivf 
#    chmod +x ./autogen.sh
#    ./autogen.sh
#    chmod +x ./configure
#    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX --host=arm
#    make && sudo make install
#    result=$?
#    if [ $result != 0 ] ; then
#        rm $SHELL_DIR/build/arm/libmodbus -rf
#        cd $SHELL_DIR
#        exit 1
#    fi
#    cd $SHELL_DIR
#fi
#
#
#
#
if [ -d $SHELL_DIR/build/arm/cares ] ;  then
    echo "cares is already build"
else
    mkdir -p $SHELL_DIR/build/arm/cares
    cd $SHELL_DIR/build/arm/cares
    cp $SHELL_DIR/lib/c-ares/* ./ -rf
    ./buildconf
    ./autoconf configure.ac
    chmod +x ./configure
    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX --host=arm
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        rm $SHELL_DIR/build/arm/cares -rf
        cd $SHELL_DIR
        exit 1
    fi
    cd $SHELL_DIR
fi
#
#
if [ -d $SHELL_DIR/build/arm/curl ] ;  then
    echo "curl is already build"
else
    mkdir -p $SHELL_DIR/build/arm/curl
    cd $SHELL_DIR/build/arm/curl
    cp $SHELL_DIR/lib/curl/* ./ -rf
    chmod +x ./configure
    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX -disable-shared --enable-static --without-libidn --without-ssl --without-librtmp --without-gnutls --without-nss --without-libssh2 --without-zlib --without-winidn --disable-rtsp --disable-ldap --disable-ldaps --disable-ipv6 --host=arm 
	make clean
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        rm $SHELL_DIR/build/arm/curl -rf
        cd $SHELL_DIR
        exit 1
    fi
    cd $SHELL_DIR
fi

#if [ -d $SHELL_DIR/build/arm/libevent ] ;  then
#    echo "curl is already build"
#else
#    mkdir -p $SHELL_DIR/build/arm/libevent
#    cd $SHELL_DIR/build/arm/libevent
#    cp $SHELL_DIR/lib/libevent/* ./ -rf
#    chmod +x ./configure
#    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX --host=arm 
#	make clean
#    make && sudo make install
#    result=$?
#    if [ $result != 0 ] ; then
#        rm $SHELL_DIR/build/arm/libevent -rf
#        cd $SHELL_DIR
#        exit 1
#    fi
#    cd $SHELL_DIR
#fi
#

if [ -d $SHELL_DIR/build/arm/openssl ] ;  then
   echo "openssl is already build"
else
   mkdir -p $SHELL_DIR/build/arm/openssl
   cd $SHELL_DIR/build/arm/openssl
   cp $SHELL_DIR/lib/openssl/* ./ -rf
   chmod +x ./Configure
   CC=$L_GCC CXX=$L_GXX ./Configure no-asm --prefix=$L_PREFIX linux-generic32 no-shared 
#CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX  --host=arm -disable-shared --enable-static
   make  && make install
   result=$?
   if [ $result != 0 ] ; then
       rm $SHELL_DIR/build/arm/openssl -rf
       cd $SHELL_DIR
       exit 1
   fi
   cd $SHELL_DIR
fi

if [ -d $SHELL_DIR/build/arm/sqlite3 ] ;  then
    echo "sqlite3 is already build"
else
    mkdir -p $SHELL_DIR/build/arm/sqlite3
    cd $SHELL_DIR/build/arm/sqlite3
    cp $SHELL_DIR/lib/sqlite3/* ./ -rf
    sudo autoreconf -ivf 
    chmod +x ./configure
    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX  --host=arm
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        rm $SHELL_DIR/build/arm/sqlite3 -rf
        cd $SHELL_DIR
        exit 1
    fi
    cd $SHELL_DIR
fi

if [ -d $SHELL_DIR/build/arm/mosquitto ] ;  then
    echo "mosquitto is already build"
else
    mkdir -p $SHELL_DIR/build/arm/mosquitto
	cd $SHELL_DIR/build/arm/mosquitto
#cp $SHELL_DIR/lib/mosquittoarm/* ./ -rf
	cmake $SHELL_DIR/lib/mosquitto/ -DCMAKE_INSTALL_PREFIX=$L_PREFIX -DCMAKE_C_COMPILER=$L_GCC -DCMAKE_CXX_COMPILER=$L_GXX -DWITH_TLS=$L_PREFIX -DUUID_HEADER=0  -DCMAKE_FIND_ROOT_PATH=/opt/gcc-linaro-arm-linux-gnueabihf-4.9
#make CC=arm-linux-gcc CXX=arm-linux-g++ CFLAGS="-I/home/edwin/workspace/gateway/build/arm/usr/include" LDFLAGS="-L/home/edwin/workspace/gateway/build/arm/usr/lib -lcares -lssl -lcrypto -luuid -lwebsockets -static"
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        #sudo rm $SHELL_DIR/build/arm/mosquitto -rf
        cd $SHELL_DIR
        exit 1
    fi
    cd $SHELL_DIR
fi

#sed -i "1c CC=arm-linux-gcc" src/Makefile

cd src/
#make clean
#make
#cp bq-gateway-bz603 ../

    
