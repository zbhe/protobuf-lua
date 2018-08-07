#include <iostream>  // for cout
#include "wrapmessage.hpp"
#include "msgtotbl.hpp"
#include "pbmanager.hpp"
#include "udata.hpp"
namespace protocol{
using namespace std::literals::string_literals;

WrapMessage::WrapMessage(Message* MsgPtr):m_Ptr(MsgPtr),
					  m_pDesc(m_Ptr->GetDescriptor()),
					  m_pRefl(m_Ptr->GetReflection())
{
}

void WrapMessage::DestroyMessage()
{
	delete m_Ptr;
	m_Ptr = nullptr;
	m_pDesc = nullptr;
	m_pRefl = nullptr;
}

void WrapMessage::SetField(lua_State* L, const std::string& sField)
{
	assert(m_pDesc);
	assert(m_pRefl);
	const FieldDescriptor* pField = m_pDesc->FindFieldByName(sField);
	if( !pField ){
		luaL_error(L, string_format("message %s no field: %s", m_Ptr->GetTypeName().c_str(), sField.c_str()).c_str());
		return;
	}
	if( pField->is_repeated() ){
		SetRepeatedField(L, pField);
		return;
	}

	using Fd = FieldDescriptor;
	Fd::CppType eCppType = pField->cpp_type();
	switch (eCppType)
	{
	case Fd::CPPTYPE_INT32:
		m_pRefl->SetInt32(m_Ptr, pField, luaL_checkint(L, -1));
		return;
	case Fd::CPPTYPE_INT64:
		m_pRefl->SetInt64(m_Ptr, pField, luaL_checkinteger(L, -1));
		return;
	case Fd::CPPTYPE_UINT32:
		m_pRefl->SetUInt32(m_Ptr, pField, luaL_checkint(L, -1));
		return;
	case Fd::CPPTYPE_UINT64:
		m_pRefl->SetUInt64(m_Ptr, pField, luaL_checkinteger(L, -1));
		return;
	case Fd::CPPTYPE_DOUBLE:
		m_pRefl->SetDouble(m_Ptr, pField, luaL_checknumber(L, -1));
		return;
	case Fd::CPPTYPE_FLOAT:
		m_pRefl->SetFloat(m_Ptr, pField, luaL_checknumber(L, -1));
		return;
	case Fd::CPPTYPE_BOOL:
		m_pRefl->SetBool(m_Ptr, pField, lua_toboolean(L, -1));
		return;
	case Fd::CPPTYPE_ENUM:
		// Support enum name.
		m_pRefl->SetEnumValue(m_Ptr, pField, GetEnumValue(L, pField, luaL_checkstring(L, -1)));
		return;
	case Fd::CPPTYPE_STRING:
		m_pRefl->SetString(m_Ptr, pField, luaL_checkstring(L, -1));
		return;
	case Fd::CPPTYPE_MESSAGE:
		if( lua_type(L, -1) == LUA_TTABLE ){
			Message* pSubMsg = m_pRefl->MutableMessage(m_Ptr, pField);
			assert(pSubMsg);
			WrapMessage(pSubMsg).SetMsg(L, -1);
			return;
		}else{
			int Type = lua_type(L, -1);
			luaL_error(L, (pField->full_name() + " expects a table but got a " + lua_typename(L, Type)).c_str());
		}
		return;
	default:
		// Unknown field type CPPTYPE_UNKNOWN of Message.Field
		luaL_error(L, ("Unknown field type "s + pField->CppTypeName(eCppType) + " of " + pField->full_name()).c_str());
		return;
	}
}

void WrapMessage::SetMsg(lua_State*L, int Index)
{
	if(lua_type(L, Index) != LUA_TTABLE){
		luaL_error(L, "msg type error");
		return;
	}
	int SaveIndex = lua_gettop(L);
	lua_pushvalue(L, Index);
	lua_pushnil(L);
	while( lua_next(L, -2) ){
		int SaveStack = lua_gettop(L) - 1;
		if( lua_type(L, -2) != LUA_TSTRING){
			lua_settop(L, SaveStack);
			continue;
		}
		const std::string& FieldName = lua_tostring(L, -2);
		SetField(L, FieldName);
		lua_settop(L, SaveStack);
	}
	lua_settop(L, SaveIndex);
}

// Support map. luaTable is an array or a map in lua table.
void WrapMessage::SetRepeatedField(lua_State* L, const FieldDescriptor* pField)
{
	assert(pField->is_repeated());
	if(lua_type(L, -1) != LUA_TTABLE){
		int Type = lua_type(L, -1);
		luaL_error(L, (pField->full_name() + " expects a table bug got " + lua_typename(L, Type)).c_str());
		return;
	}
	if( pField->is_map() ){
		/*unsupport by tony 2017-11-17 20:22:06
		SetRepeatedMapField(field, luaTable);
		*/
		return;
	}
	// non-map
	lua_pushvalue(L, -1);
	lua_pushnil(L);
	using Fd = FieldDescriptor;
	Fd::CppType eCppType = pField->cpp_type();
	while( lua_next(L, -2) ){
		int SaveStack = lua_gettop(L);
		switch (eCppType)
		{
		case Fd::CPPTYPE_INT32:
			m_pRefl->AddInt32(m_Ptr, pField, luaL_checkint(L, -1));
			break;
		case Fd::CPPTYPE_INT64:
			m_pRefl->AddInt64(m_Ptr, pField, luaL_checkinteger(L, -1));
			break;
		case Fd::CPPTYPE_UINT32:
			m_pRefl->AddUInt32(m_Ptr, pField, luaL_checkint(L, -1));
			break;
		case Fd::CPPTYPE_UINT64:
			m_pRefl->AddUInt64(m_Ptr, pField, luaL_checkinteger(L, -1));
			break;
		case Fd::CPPTYPE_DOUBLE:
			m_pRefl->AddDouble(m_Ptr, pField, luaL_checknumber(L, -1));
			break;
		case Fd::CPPTYPE_FLOAT:
			m_pRefl->AddFloat(m_Ptr, pField, luaL_checknumber(L, -1));
			break;
		case Fd::CPPTYPE_BOOL:
			m_pRefl->AddBool(m_Ptr, pField, lua_toboolean(L, -1));
			break;
		case Fd::CPPTYPE_ENUM:
			// Support enum name.
			m_pRefl->AddEnumValue(m_Ptr, pField, GetEnumValue(L, pField, luaL_checkstring(L, -1)));
			break;
		case Fd::CPPTYPE_STRING:
			m_pRefl->AddString(m_Ptr, pField, luaL_checkstring(L, -1));
			break;
		case Fd::CPPTYPE_MESSAGE:
			if( lua_type(L, -1) == LUA_TTABLE ){
				Message* pSubMsg = m_pRefl->AddMessage(m_Ptr, pField);
				assert(pSubMsg);
				WrapMessage(pSubMsg).SetMsg(L, -1);
			}else{
				int Type = lua_type(L, -1);
				luaL_error(L, (pField->full_name() + " expects a table bug got " + lua_typename(L, Type)).c_str());
			}
			break;
		default:
			luaL_error(L, (string("Unknown field type ") + pField->CppTypeName(eCppType) + " of " + pField->full_name()).c_str());
			break;
		}
		lua_pop(L, 1);
		lua_settop(L, SaveStack);
	}
}


// Try to convert lua string to enum value.
// If lua value is not string or not enum string, get int.
int WrapMessage::GetEnumValue(lua_State* L, const FieldDescriptor* pField, const std::string& EnumName) const
{
	assert(pField->cpp_type() == FieldDescriptor::CPPTYPE_ENUM);
	const EnumDescriptor* pEnum = pField->enum_type();
	assert(pEnum);
	  // Looks up a value by name.  Returns NULL if no such value exists.
	const EnumValueDescriptor* pEnumVal = pEnum->FindValueByName(EnumName);
	if( pEnumVal ){
		return pEnumVal->number();
	}
	return -1;
}

std::string WrapMessage::Encode(lua_State* L, int Index)
{
	if(lua_type(L, Index) != LUA_TTABLE){
		luaL_error(L, "msg type error");
		return "";//never here
	}
	assert(m_Ptr);
	m_Ptr->Clear();
	SetMsg(L, Index);
	return m_Ptr->SerializeAsString();
}  // Encode()

int WrapMessage::Decode(lua_State* L, const string& sData)
{
	assert(m_Ptr);
	m_Ptr->Clear();
	if(m_Ptr->ParseFromString(sData)){
		return MsgToTbl(L, *m_Ptr)();
	}
	return 0;
}


/*unsupport by tony 2017-11-17 20:22:06
void WrapMessage::SetRepeatedMapField(const FieldDescriptor& field, const LuaRef& luaTable)
{
	assert(field.is_repeated());
	assert(field.is_map());
	assert(luaTable.isTable());

	const auto itrEnd = luaTable.end();
	for (auto itr = luaTable.begin(); itr != itrEnd; ++itr)
	{
		const LuaRef& key = itr.key();
		const LuaRef& val = itr.value();
		AddToMapField(field, key, val);
	}
}

void WrapMessage::AddToMapField(const FieldDescriptor& field, const LuaRef& key, const LuaRef& val)
{
	assert(field.is_repeated());
	assert(field.is_map());
	assert(field.cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE);
	Message* pMapEntry = m_pRefl->AddMessage(m_Ptr, &field);
	assert(pMapEntry);
	WrapMessage(*pMapEntry);
	setter.SetField("key", key);
	setter.SetField("value", val);
}  // AddToMapField
*/
};//end namespace
