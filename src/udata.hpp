#pragma once

#include <memory>
#include <string>
#include "lua.hpp"
#include "lstate.h"

//给栈顶的udata设置指定的metatable
#define SET_UDATA_METATABLE(L, MT) \
	{\
		int LDataFromLutil = lua_gettop(L);\
		luaL_getmetatable(L, MT);\
		lua_setmetatable(L, LDataFromLutil);\
	}\

//给栈顶的udata绑定新的env
//lua_setfenv是可能失败的，但是这个对象是我自己newuserdata出来的应该没问题
//5.2不支持fenv, 函数setupvalue, userdata用set_uservalue
#define NEW_ENV(L) \
	{\
		lua_newtable(L);\
		lua_setuservalue(L, -2);\
	}\

//创建一个udata，用ClsName初始化，把metatable设置为MT
#define CREATE_UDATA(L, PtrName, ClsName, UDID, MT, ...) \
	{\
		PtrName = (ClsName *)lua_newuserdataEx(L, UDID, sizeof(ClsName));\
		new (PtrName) ClsName(__VA_ARGS__);\
		SET_UDATA_METATABLE(L, MT)\
	}\

//创建一个udata，用ClsName初始化，把metatable设置为MT，再绑定一个新env
#define CREATE_UDATA_WITH_ENV(L, PtrName, ClsName, UDID, MT, ...) \
	{\
		CREATE_UDATA(L, PtrName, ClsName, UDID, MT, ##__VA_ARGS__);\
		NEW_ENV(L)\
	}\

//释放一个与对象关联在一起的udata，这个udata必须在栈顶
#define DESTROY_UDATA(L, ClsName, UDID) \
	{ \
		ClsName * UdataToBeDestroyed = (ClsName *)luaL_checkudataEx(L, -1, UDID); \
		UdataToBeDestroyed->~ClsName(); \
	} \

//假设栈顶是lua table：tbl，本函数做的事情是tbl[Key] = {Construct = ConstructName, Deconstruct = DeconstructName}
#define DBG_INSERT_TO_LUA(L, Key, ConstructName, DeconstructName) \
	{ \
		lua_pushstring(L, Key);\
		lua_newtable(L);\
		lua_pushstring(L, "Construct");\
		lua_pushinteger(L, ConstructName);\
		lua_settable(L, -3);\
		lua_pushstring(L, "Deconstruct");\
		lua_pushinteger(L, DeconstructName);\
		lua_settable(L, -3);\
		lua_settable(L, -3);\
	} \

class LUTILException: public std::exception
{
public:
	LUTILException(const std::string & msg) : _msg(msg) {}
	~LUTILException() throw () {}
	const char* what() const throw() {return _msg.c_str();}
private:
	std::string _msg;
};

//简单的format功能，限制了总串长度不能超过5120
const std::string string_format(const char * fmt, ...);
void RegisterType(lua_State * L, const char * typeMetaName, const struct luaL_Reg meths[]);

//userdata扩展

int GetUDID();
inline lua_State* GetMainL(const lua_State* L)
{
	return G(L)->mainthread;
}

void *lua_newuserdataEx (lua_State *L, int udata_id, size_t size);
void *luaL_checkudataEx (lua_State *L, int ud, int udata_id);
void *luaL_checkudataExWithException (lua_State *L, int ud, int udata_id) throw (LUTILException);
void *luaL_checkudatalistEx (lua_State *L, int ud, const int udata_id_list[], size_t list_size, const char * summary_tname);
void *luaL_checkudatalistExWithException (lua_State *L, int ud, const int udata_id_list[], size_t list_size, const char * summary_tname);

