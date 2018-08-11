#include "msgtotbl.hpp"

#include <unordered_set>
namespace pb{

MsgToTbl::MsgToTbl(lua_State* L, const Message& msg) : m_L(L), m_msg(msg), m_pRefl(msg.GetReflection())
{
	if (!m_pRefl){
		luaL_error(L, ("Message" + msg.GetTypeName() + " has no reflection").c_str());
	}
}

int MsgToTbl::operator() ()const
{
	const Descriptor* pDesc = m_msg.GetDescriptor();
	assert(pDesc);
	std::unordered_set<const OneofDescriptor*> oneofDescSet;
	lua_newtable(m_L);

	int nField = pDesc->field_count();
	for (int index = 0; index < nField; ++index)
	{
		const FieldDescriptor* pField = pDesc->field(index);
		assert(pField);
		const OneofDescriptor* pOneof = pField->containing_oneof();
		if (pOneof)
		{
			oneofDescSet.insert(pOneof);
			continue;  // Oneof field should not set default value.
		}
		lua_pushstring(m_L, pField->name().c_str());
		if( not GetField(pField) ){
			//todo log error
		}
		lua_settable(m_L, -3);
	}

	// Set oneof fields.
	for (const OneofDescriptor* pOneof : oneofDescSet)
	{
		const FieldDescriptor* pField = m_pRefl->GetOneofFieldDescriptor(m_msg, pOneof);
		if (pField){
			lua_pushstring(m_L, pField->name().c_str());
			if( not GetField(pField) ){
				//todo log error
			}
			lua_settable(m_L, -3);
		}
	}

	return 1;
}

bool MsgToTbl::GetField(const FieldDescriptor* pField) const
{
	if( pField->is_repeated())
	{
		// returns (TableRef, "") or (nil, error_string)
		return GetRepeatedField(pField);
	}

	using Fd = FieldDescriptor;
	Fd::CppType eCppType = pField->cpp_type();
	switch (eCppType)
	{
	// Scalar field always has a default value.
	case Fd::CPPTYPE_INT32:
		lua_pushinteger(m_L, m_pRefl->GetInt32(m_msg, pField));
		break;
		//return LuaRefValue(L, m_pRefl->GetInt32(m_msg, &field));
	case Fd::CPPTYPE_INT64:
		lua_pushinteger(m_L, m_pRefl->GetInt64(m_msg, pField));
		break;
		//return LuaRefValue(L, m_pRefl->GetInt64(m_msg, &field));
	case Fd::CPPTYPE_UINT32:
		lua_pushinteger(m_L, m_pRefl->GetUInt32(m_msg, pField));
		break;
		//return LuaRefValue(L, m_pRefl->GetUInt32(m_msg, &field));
	case Fd::CPPTYPE_UINT64:
		lua_pushinteger(m_L, m_pRefl->GetUInt64(m_msg, pField));
		break;
		//return LuaRefValue(L, m_pRefl->GetUInt64(m_msg, &field));
	case Fd::CPPTYPE_DOUBLE:
		lua_pushnumber(m_L, m_pRefl->GetDouble(m_msg, pField));
		break;
		//return LuaRefValue(L, m_pRefl->GetDouble(m_msg, &field));
	case Fd::CPPTYPE_FLOAT:
		lua_pushnumber(m_L, m_pRefl->GetFloat(m_msg, pField));
		break;
		//return LuaRefValue(L, m_pRefl->GetFloat(m_msg, &field));
	case Fd::CPPTYPE_BOOL:
		lua_pushboolean(m_L, m_pRefl->GetBool(m_msg, pField));
		break;
		//return LuaRefValue(L, m_pRefl->GetBool(m_msg, &field));
	case Fd::CPPTYPE_ENUM:
		lua_pushinteger(m_L, m_pRefl->GetEnumValue(m_msg, pField));
		break;
		//return LuaRefValue(L, m_pRefl->GetEnumValue(m_msg, &field));
	case Fd::CPPTYPE_STRING:
		lua_pushstring(m_L, m_pRefl->GetString(m_msg, pField).c_str());
		break;
		//return LuaRefValue(L, m_pRefl->GetString(m_msg, &field));
	case Fd::CPPTYPE_MESSAGE:
		// For message field, the default value is null.
		if( m_pRefl->HasField(m_msg, pField) ){
			const Message& subMsg = m_pRefl->GetMessage(m_msg, pField);
			return MsgToTbl(m_L, subMsg)();
		}
		lua_pushnil(m_L);
		break;
	default:
		// Unknown field type CPPTYPE_UNKNOWN of Message.Field
		lua_pushnil(m_L);
		return false;
	}
	return true; 
}

// Return a lua table. Map is supported.
bool MsgToTbl::GetRepeatedField(const FieldDescriptor* pField) const
{
	assert(pField->is_repeated());

	lua_newtable(m_L);
	int nFldSize = m_pRefl->FieldSize(m_msg, pField);
	if( ! pField->is_map() ){
		for( int index = 0; index < nFldSize; ++index ){
			lua_pushinteger(m_L, index + 1);
			if( not GetRepeatedFieldElement(pField, index) ){
				//todo log error
			}
			lua_settable(m_L, -3);
		}
		return true;
	}

	// map
	/* unsupport ...by tony 2017-11-16 19:48:06
	assert(pField->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE);
	for( int index = 0; index < nFldSize; ++index )
	{
		const Message& entryMsg = m_pRefl->GetRepeatedMessage(m_msg, pField, index);
		LuaRef& entryTbl = MsgToTbl(m_rLuaState, entryMsg)();
		const LuaRef& key = entryTbl["key"];
		const LuaRef& value = entryTbl["value"];
		tbl[key] = value;
	}
	*/
	return false;
}

// index starts from 0.
bool MsgToTbl::GetRepeatedFieldElement( const FieldDescriptor* pField, int index) const
{
	assert(pField->is_repeated());
	assert(index >= 0);
	assert(index < m_pRefl->FieldSize(m_msg, pField));

	using Fd = FieldDescriptor;
	Fd::CppType eCppType = pField->cpp_type();
	switch (eCppType)
	{
	case Fd::CPPTYPE_INT32:
		lua_pushinteger(m_L, m_pRefl->GetRepeatedInt32(m_msg, pField, index));
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedInt32(m_msg, &field, index));
	case Fd::CPPTYPE_INT64:
		lua_pushinteger(m_L, m_pRefl->GetRepeatedInt64(m_msg, pField, index));
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedInt64(m_msg, &field, index));
	case Fd::CPPTYPE_UINT32:
		lua_pushinteger(m_L, m_pRefl->GetRepeatedUInt32(m_msg, pField, index));
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedUInt32(m_msg, &field, index));
	case Fd::CPPTYPE_UINT64:
		lua_pushinteger(m_L, m_pRefl->GetRepeatedUInt64(m_msg, pField, index));
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedUInt64(m_msg, &field, index));
	case Fd::CPPTYPE_DOUBLE:
		lua_pushnumber(m_L, m_pRefl->GetRepeatedDouble(m_msg, pField, index));
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedDouble(m_msg, &field, index));
	case Fd::CPPTYPE_FLOAT:
		lua_pushnumber(m_L, m_pRefl->GetRepeatedFloat(m_msg, pField, index));
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedFloat(m_msg, &field, index));
	case Fd::CPPTYPE_BOOL:
		lua_pushboolean(m_L, m_pRefl->GetRepeatedBool(m_msg, pField, index));
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedBool(m_msg, &field, index));
	case Fd::CPPTYPE_ENUM:
		lua_pushinteger(m_L, m_pRefl->GetRepeatedEnumValue(m_msg, pField, index));
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedEnumValue(m_msg, &field, index));
	case Fd::CPPTYPE_STRING:
		lua_pushstring(m_L, m_pRefl->GetRepeatedString(m_msg, pField, index).c_str());
		break;
		//return LuaRefValue(L, m_pRefl->GetRepeatedString(m_msg, &field, index));
	case Fd::CPPTYPE_MESSAGE:
		{
			const Message& subMsg = m_pRefl->GetRepeatedMessage(m_msg, pField, index);
			MsgToTbl(m_L, subMsg)();
			break;
		}
	default:
		// Unknown repeated field type CPPTYPE_UNKNOWN of Message.Field
		lua_pushnil(m_L);
		return false;
	}
	return true;
}

};//end namespace

