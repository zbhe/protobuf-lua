#include <string>
#include <stdio.h>
#include "lua.hpp"
#include "lauxlib.h"
#include "luapblib.hpp"
#include "pbmanager.hpp"
#include "wrapmessage.hpp"
#include "udata.hpp"

namespace pb{

using namespace ::google::protobuf;

const static char* MESSAGE_META = "__luapb_message__";
const static int MESSAGE_UDID = GetUDID();

static int message_gc(lua_State* L)
{
	WrapMessage* MsgPtr = (WrapMessage*)luaL_checkudataEx(L, -1, MESSAGE_UDID);
	MsgPtr->DestroyMessage();
	DESTROY_UDATA(L, WrapMessage, MESSAGE_UDID);
	return 0;
}

static int message_encode(lua_State* L)
{
	WrapMessage* MsgPtr = (WrapMessage*)luaL_checkudataEx(L, 1, MESSAGE_UDID);
	std::string MsgCode = MsgPtr->Encode(L, -1);
	lua_pushlstring(L, MsgCode.c_str(), MsgCode.size());
	return 1;
}
static int message_decode(lua_State* L)
{
	WrapMessage* MsgPtr = (WrapMessage*)luaL_checkudataEx(L, 1, MESSAGE_UDID);
	size_t Len;
	const char* Str = luaL_checklstring(L, -1, &Len);
	return MsgPtr->Decode(L, std::string(Str, Len));
}

static const struct luaL_Reg message[] = {
	{ "__gc", message_gc},
	{ "encode", message_encode},
	{ "decode", message_decode},
	{ NULL, NULL },
};


static int add_path(lua_State* L)
{
	if (LUA_TSTRING != lua_type(L, 2)){
		return luaL_error(L, "path must to be string");
	}
	PbManager::GetManager().AddProtoPath(luaL_checkstring(L, 2));
	return 0;
}

static int import_proto(lua_State* L)
{
	if (LUA_TSTRING != lua_type(L, 2)){
		return luaL_error(L, "path must to be string");
	}
	if( ! PbManager::GetManager().ImportProtoFile(luaL_checkstring(L, 2)) ){
		luaL_error(L, "import failed");
	}
	return 0;
}

static int get_message(lua_State* L)
{
	if (LUA_TSTRING != lua_type(L, 2)){
		return luaL_error(L, "message name must to be string");
	}
	Message* MsgPtr = PbManager::GetManager().MakeMessage(luaL_checkstring(L, 2));
	if( nullptr == MsgPtr ){
		return 0;
	}
	WrapMessage* RetPtr = nullptr;
	CREATE_UDATA(L, RetPtr, WrapMessage, MESSAGE_UDID, MESSAGE_META, MsgPtr);
	return 1;
}

static const struct luaL_Reg libmeth[] = {
	{"add_proto_path", add_path},
	{"import_proto", import_proto},
	{"get_message", get_message},
	{NULL, NULL},
};

};//end namespace

extern "C"
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)
__declspec(dllexport)
#endif
int luaopen_lpb(lua_State* L)
{
	RegisterType(L, pb::MESSAGE_META, pb::message);
	//luaL_register(L, "rpc", pb::libmeth);
	luaL_newlib(L, pb::libmeth);
	return 1;
}
