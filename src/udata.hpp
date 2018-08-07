#pragma once

#include <memory>
#include <string>
#include <sstream>
#include "lua.hpp"
#include "lstate.h"

#define SET_UDATA_METATABLE(L, MT) \
	{\
		int Top = lua_gettop(L);\
		luaL_getmetatable(L, MT);\
		lua_setmetatable(L, Top);\
	}\

#define NEW_ENV(L) \
	{\
		lua_newtable(L);\
		lua_setuservalue(L, -2);\
	}\

#define CREATE_UDATA(L, PtrName, ClsName, UDID, MT, ...) \
	{\
		PtrName = (ClsName *)lua_newuserdataEx(L, UDID, sizeof(ClsName));\
		new (PtrName) ClsName(__VA_ARGS__);\
		SET_UDATA_METATABLE(L, MT)\
	}\

#define CREATE_UDATA_WITH_ENV(L, PtrName, ClsName, UDID, MT, ...) \
	{\
		CREATE_UDATA(L, PtrName, ClsName, UDID, MT, ##__VA_ARGS__);\
		NEW_ENV(L)\
	}\

#define DESTROY_UDATA(L, ClsName, UDID) \
	{ \
		ClsName * UdataToBeDestroyed = (ClsName *)luaL_checkudataEx(L, -1, UDID); \
		UdataToBeDestroyed->~ClsName(); \
	} \


void _tostring_help(std::ostringstream& ss){}
template<typename T, typename... Params> void _tostring_help(std::ostringstream& ss, T&& First, Params&&... Last)
{
	ss << First;
	_tostring_help(ss, std::forward<Params>(Last)...);
}
template<typename... Params> std::string tostring(Params&&... Args)
{
	std::ostringstream OStr;
	_tostring_help(OStr, std::forward<Params>(Args)...);
	return OStr.str();
}
std::string string_format(const char * fmt, ...);
void RegisterType(lua_State * L, const char * typeMetaName, const struct luaL_Reg meths[]);
int GetUDID();
void *lua_newuserdataEx (lua_State *L, int udata_id, size_t size);
void *luaL_checkudataEx (lua_State *L, int ud, int udata_id);

