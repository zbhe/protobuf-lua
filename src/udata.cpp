#include "udata.hpp"

std::string string_format(const char * fmt, ...)
{
	const size_t STR_FMT_MAX_LEN = 5120;
	char buf[STR_FMT_MAX_LEN];
	va_list ap;
	va_start(ap, fmt);
	int size = vsnprintf(buf, STR_FMT_MAX_LEN, fmt, ap);
	va_end(ap);
	buf[size + 1] = '\0';
	std::string ret = buf;
	return ret;
}
void RegisterType(lua_State * L, const char * typeMetaName, const struct luaL_Reg meths[])
{
	int top = lua_gettop(L);
	luaL_newmetatable(L, typeMetaName);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	//luaL_register(L, nullptr, meths);
	luaL_setfuncs(L, meths, 0);
	lua_settop(L, top);
}

int GetUDID()
{
	static int id = 0;
	id+=1;
	return id;
}
void *lua_newuserdataEx (lua_State *L, int udata_id, size_t size)
{
	char * RealPtr = (char *)lua_newuserdata(L, size + sizeof(int));
	*((int *)RealPtr) = udata_id;
	return (void *)(RealPtr + sizeof(int));
}
void *luaL_checkudataEx (lua_State *L, int ud, int udata_id)
{
	void *p = lua_touserdata(L, ud);
	if (p != nullptr && *((int *)p) == udata_id)
		return (void *)((char *)p + sizeof(int));
	luaL_argerror(L, ud, string_format("userdata(id:%d, got:%d)", udata_id, p ? *(int*)p : -1).c_str());  /* else error */
	return nullptr;
}

