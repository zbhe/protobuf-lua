LUA_INC = ../third/lua-5.3.5/include
PB_INC = ../third/protobuf/include
CC = g++
CPPFLAGS = -g -O2 -fPIC -Wall -std=c++14 -I $(LUA_INC) -I $(PB_INC)
SHARED = -shared
#SHARED = -dynamiclib -fPIC #mac

SRC= luapblib.cpp msgtotbl.cpp pbmanager.cpp wrapmessage.cpp udata.cpp
OBJS=$(subst .cpp,.o,$(SRC))

PB_LIB = ../third/protobuf/lib

all : lpb.so

lpb.so : $(OBJS)
	$(CC) $(CFLAGS) $(SHARED) -o$@ $^ -L$(PB_LIB) -lprotobuf -lpthread

clean :
	rm -f *.o
	rm -f lpb.so

test : 
	lua test.lua
