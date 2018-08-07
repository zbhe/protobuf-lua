CC = g++
CFLAGS = -g -O2 -Wall -std=c++14
SHARED = -shared -fPIC

SRC= luapblib.cpp msgtotbl.cpp pbmanager.cpp wrapmessage.cpp udata.cpp
OBJS=$(subst .cpp,.o,$(SRC))

LUA_INC = /usr/local/include
PB_INC = /usr/local/include
PB_LIB = /usr/local/lib

all : libluaprotobuf.so

libluaprotobuf.so : $(OBJS)
	$(CC) $(CFLAGS) $(SHARED) -o$@ $^ -I$(LUA_INC) -I$(PB_INC) -L$(PB_LIB) -Wl,-Bdynamic 

clean :
	rm -f *.o
	rm -f libluaprotobuf.so

test : 
	lua test.lua
