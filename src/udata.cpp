#include "udata.hpp"

const std::string string_format(const char * fmt, ...)
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
	luaL_register(L, NULL, meths);
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
	if (p != NULL && *((int *)p) == udata_id)
		return (void *)((char *)p + sizeof(int));
	luaL_typerror(L, ud, string_format("userdata(id:%d, got:%d)", udata_id, p ? *(int*)p : -1).c_str());  /* else error */
	return NULL;  /* to avoid warnings */
}
void *luaL_checkudataExWithException (lua_State *L, int ud, int udata_id) throw (LUTILException)
{
	void *p = lua_touserdata(L, ud);
	if (p != NULL && *((int *)p) == udata_id)
		return (void *)((char *)p + sizeof(int));
	throw LUTILException(string_format("%s expected, got %s", string_format("userdata(id:%d)", udata_id).c_str(), luaL_typename(L, ud)));
}
void *luaL_checkudatalistEx (lua_State *L, int ud, const int udata_id_list[], size_t list_size, const char * summary_tname)
{
	void *p = lua_touserdata(L, ud);
	if (p != NULL)
	{
		int CurId = *((int *)p);
		for (size_t i = 0 ; i < list_size ; i++)
			if (udata_id_list[i] == CurId)
				return (void *)((char *)p + sizeof(int));
	}
	luaL_typerror(L, ud, summary_tname);  /* else error */
	return NULL;  /* to avoid warnings */
}
void *luaL_checkudatalistExWithException (lua_State *L, int ud, const int udata_id_list[], size_t list_size, const char * summary_tname)
{
	void *p = lua_touserdata(L, ud);
	if (p != NULL)
	{
		int CurId = *((int *)p);
		for (size_t i = 0 ; i < list_size ; i++)
			if (udata_id_list[i] == CurId)
				return (void *)((char *)p + sizeof(int));
	}
	throw LUTILException(string_format("%s expected, got %s", summary_tname, luaL_typename(L, ud)));
}

