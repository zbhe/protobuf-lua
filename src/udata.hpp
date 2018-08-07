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


class tostring
{
private:
	const std::string Sep;
	void do_tostring(std::ostringstream& ss){}
	template<typename First, typename... Last> void do_tostring(std::ostringstream& ss, First&& F, Last&&... L)
	{
		ss << F;
		if( sizeof...(L) > 0 ){
			ss << Sep;
		}
		do_tostring(ss, std::forward<Last>(L)...);
	}
public:
	tostring() = default;
	template<typename T> tostring(T&& _Sep):Sep{std::forward<T>(_Sep)}{}

	template<typename... Params> std::string operator () (Params&&... Args)
	{
		std::ostringstream OStr;
		do_tostring(OStr, std::forward<Params>(Args)...);
		return OStr.str();
	}
};

std::string string_format(const char * fmt, ...);
void RegisterType(lua_State * L, const char * typeMetaName, const struct luaL_Reg meths[]);
int GetUDID();
void *lua_newuserdataEx (lua_State *L, int udata_id, size_t size);
void *luaL_checkudataEx (lua_State *L, int ud, int udata_id);

