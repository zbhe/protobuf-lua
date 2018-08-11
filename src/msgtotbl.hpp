#ifndef DETAIL_MSGTOTBL_H
#define DETAIL_MSGTOTBL_H

#include <google/protobuf/message.h>  // for Message
#include "lua.hpp"
namespace pb{
using namespace ::google::protobuf;

// Message to lua table converter.
class MsgToTbl final
{
public:
	explicit MsgToTbl(lua_State* L, const Message& msg);

public:
	int operator () ()const;

private:
	bool GetField(const FieldDescriptor* pField) const;
	bool GetRepeatedField(const FieldDescriptor* pField) const;
	// index starts from 0.
	bool GetRepeatedFieldElement(const FieldDescriptor* pField, int index) const;

private:
	lua_State* m_L;
	const Message& m_msg;
	const Reflection* m_pRefl;  // = msg.GetReflection();
};
}//end namesapce
#endif  // DETAIL_MSGTOTBL_H
