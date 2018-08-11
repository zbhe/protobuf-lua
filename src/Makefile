LUA_INC = ../third/lua/include
PB_INC = ../third/protobuf/include
CC = g++
CPPFLAGS = -g -O2 -Wall -std=c++14 -I $(LUA_INC) -I $(PB_INC)
#SHARED = -shared -fPIC
SHARED = -dynamiclib -fPIC #mac

SRC= luapblib.cpp msgtotbl.cpp pbmanager.cpp wrapmessage.cpp udata.cpp
OBJS=$(subst .cpp,.o,$(SRC))

PB_LIB = ../third/protobuf/lib
LUALIB = ../third/lua/lib/liblua.a

all : libpb.so

libpb.so : $(OBJS)
	$(CC) $(CFLAGS) $(SHARED) -o$@ $^ $(LUALIB) -L$(PB_LIB) -lprotobuf

clean :
	rm -f *.o
	rm -f libpb.so

test : 
	lua test.lua
