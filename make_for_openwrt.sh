#!/bin/sh

SHELL_DIR=`pwd`
L_GCC=mipsel-openwrt-linux-gcc
L_GXX=mipsel-openwrt-linux-g++
L_STRIP=mipsel-openwrt-linux-uclibc-strip
L_TARGET=$SHELL_DIR
L_PREFIX=$SHELL_DIR/build/mipsel/usr
L_HOST=arm
export GCC=$L_GCC
export GXX=$L_GXX

mkdir -p $L_PREFIX


#uuid install
if [ -d $SHELL_DIR/build/mips/uuid ] ;  then
    echo "uuid is already build"
else
    mkdir -p $SHELL_DIR/build/mips/uuid
    cd $SHELL_DIR/build/mips/uuid
    cp $SHELL_DIR/lib/uuid/* ./ -rf
    autoreconf -ivf 
    chmod +x ./configure
    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX --host=$L_HOST
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        rm $SHELL_DIR/build/mips/uuid -rf
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
if [ -d $SHELL_DIR/build/mips/cares ] ;  then
    echo "cares is already build"
else
    mkdir -p $SHELL_DIR/build/mips/cares
    cd $SHELL_DIR/build/mips/cares
    cp $SHELL_DIR/lib/c-ares/* ./ -rf
    ./buildconf
    ./autoconf configure.ac
    chmod +x ./configure
    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX --host=arm
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        rm $SHELL_DIR/build/mips/cares -rf
        cd $SHELL_DIR
        exit 1
    fi
    cd $SHELL_DIR
fi
#
#
if [ -d $SHELL_DIR/build/mips/curl ] ;  then
    echo "curl is already build"
else
    mkdir -p $SHELL_DIR/build/mips/curl
    cd $SHELL_DIR/build/mips/curl
    cp $SHELL_DIR/lib/curl/* ./ -rf
    chmod +x ./configure
    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX -disable-shared --enable-static --without-libidn --without-ssl --without-librtmp --without-gnutls --without-nss --without-libssh2 --without-zlib --without-winidn --disable-rtsp --disable-ldap --disable-ldaps --disable-ipv6 --host=arm 
	make clean
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        rm $SHELL_DIR/build/mips/curl -rf
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
#if [ -d $SHELL_DIR/build/arm/openssl ] ;  then
#    echo "openssl is already build"
#else
#   mkdir -p $SHELL_DIR/build/arm/openssl
#   cd $SHELL_DIR/build/arm/openssl
#   cp $SHELL_DIR/lib/openssl/* ./ -rf
#   chmod +x ./Configure
#   CC=$L_GCC CXX=$L_GXX ./Configure --prefix=$L_PREFIX linux-generic32
#   make  && make install
#   result=$?
#   if [ $result != 0 ] ; then
#       rm $SHELL_DIR/build/arm/openssl -rf
#       cd $SHELL_DIR
#       exit 1
#   fi
#   cd $SHELL_DIR
#fi
#
if [ -d $SHELL_DIR/build/mips/openssl ] ;  then
   echo "openssl is already build"
else
   mkdir -p $SHELL_DIR/build/mips/openssl
   cd $SHELL_DIR/build/mips/openssl
   cp $SHELL_DIR/lib/openssl/* ./ -rf
   chmod +x ./Configure
   CC=$L_GCC CXX=$L_GXX ./Configure --prefix=$L_PREFIX linux-generic32 no-async
   make  && make install
   result=$?
   if [ $result != 0 ] ; then
       rm $SHELL_DIR/build/mips/openssl -rf
       cd $SHELL_DIR
       exit 1
   fi
   cd $SHELL_DIR
fi

if [ -d $SHELL_DIR/build/mips/sqlite3 ] ;  then
    echo "sqlite3 is already build"
else
    mkdir -p $SHELL_DIR/build/mips/sqlite3
    cd $SHELL_DIR/build/mips/sqlite3
    cp $SHELL_DIR/lib/sqlite3/* ./ -rf
    autoreconf -ivf 
    chmod +x ./configure
    CC=$L_GCC CXX=$L_GXX ./configure --prefix=$L_PREFIX  --host=arm
    make && make install
    result=$?
    if [ $result != 0 ] ; then
        rm $SHELL_DIR/build/mips/sqlite3 -rf
        cd $SHELL_DIR
        exit 1
    fi
    cd $SHELL_DIR
fi

if [ -d $SHELL_DIR/build/mips/mosquitto ] ;  then
    echo "mosquitto is already build"
else
    mkdir -p $SHELL_DIR/build/mips/mosquitto
	cd $SHELL_DIR/build/mips/mosquitto
#cp $SHELL_DIR/lib/mosquittoarm/* ./ -rf
	cmake $SHELL_DIR/lib/mosquitto/ -DCMAKE_INSTALL_PREFIX=$L_PREFIX -DCMAKE_C_COMPILER=$L_GCC -DCMAKE_CXX_COMPILER=$L_GXX -DWITH_TLS=$L_PREFIX -DUUID_HEADER=0  -DCMAKE_FIND_ROOT_PATH=/home/kevin/workspace/openwrt_ap2/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2
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

    
