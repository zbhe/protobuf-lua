#ifndef DETAIL_MESSAGESPTR_H
#define DETAIL_MESSAGESPTR_H

#include <google/protobuf/descriptor.h>  // for Descriptor
#include <google/protobuf/message.h>  // for Message
#include <google/protobuf/descriptor.pb.h>  // for map_entry()
#include <memory>  // for shared_ptr<>
#include <string>
#include "lua.hpp"
namespace pb{
using namespace ::google::protobuf;
class WrapMessage
{
	private:
		Message* m_Ptr;
		const Descriptor* m_pDesc;  // = m_rMsg.GetDescriptor();
		const Reflection* m_pRefl;  // = m_rMsg.GetReflection();
	private:
		//void SetRepeatedMapField(const FieldDescriptor& field, int Index);
		//void AddToRepeatedField(const FieldDescriptor& field, int Index);
		//void AddToMapField(const FieldDescriptor& field, const std::string& key, int Index);
		void SetField(lua_State*L, const std::string& sField);
		void SetRepeatedField(lua_State*L, const FieldDescriptor* pField);
		int GetEnumValue(lua_State* L, const FieldDescriptor* pField, const std::string& EnumName) const;
	public:
		explicit WrapMessage(Message* MsgPtr);
		void DestroyMessage();
		void SetMsg(lua_State*L, int Index);
		std::string Encode(lua_State* L, int Index);
		int Decode(lua_State* L, const std::string& Data);
};

};//end namespace
#endif  // DETAIL_MESSAGESPTR_H

