#CC=gcc
CC=arm-linux-gnueabihf-gcc
CFLAGES = -w -g -Wall -O2
LIBS=-lpthread -L../build/arm/usr/lib -lsqlite3 -luuid -ldl -lmosquitto -lcrypto -lcurl -lcares
#LIBS=-lpthread -lm -ldl -L../build/arm/usr/lib -lmosquitto -lsqlite3 -luuid -lrt -lcares -liot_sdk -liot_platform -lmbedtls -lmbedx509 -lmbedcrypto -lgcov -lcrypto -lycapic -lcurl -levent

INCS=-I./ -I./utils -I./common -I./log -I./json -I./lds -I./base -I./broadcast -I./net -I../build/arm/usr/include
TARGET = dalitek_new_gateway
DIRS = . \
./utils \
./common \
./log \
./json \
./lds \
./base \
./broadcast \
./net \

FILES = $(foreach dir, $(DIRS),$(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c,%.o,$(FILES))


$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGES) $(LIBS)

$(OBJS):%.o:%.c         
	@$(CC) $(INCS) -c  $< $(CFLAGES) -o $@

clean:
	-rm -f $(TARGET) $(OBJS)
