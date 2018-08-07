#ifndef DETAIL_LUAPBINTFIMPL_H
#define DETAIL_LUAPBINTFIMPL_H

#include <memory>  // for unique_ptr<>
#include <string>
#include <exception>
#include <google/protobuf/compiler/importer.h>  // for DiskSourceTree
#include <google/protobuf/descriptor.h>  // for Descriptor
#include <google/protobuf/dynamic_message.h>  // for GetPrototype()
#include <google/protobuf/message.h>  // for Message
#include "lua.hpp"
#include "wrapmessage.hpp"  // for MessageSptr

namespace protocol{

using namespace ::google::protobuf;

class LuaException: public std::exception
{
	std::string Msg;
	public:
			LuaException(const std::string& Err):Msg(Err){}
			const char* what()const noexcept override {
				return Msg.c_str();
			}
};

class ErrorCollector;
class PbManager
{
private:
	PbManager();
	PbManager(const PbManager&) = delete;
	static PbManager Instance;
	virtual ~PbManager();

public:
	using string = std::string;

	static PbManager& GetManager(){return Instance;}
	// e.g. AddProtoPath("proto")
	// e.g. AddProtoPath("d:/proto")
	void AddProtoPath(const string& sProtoPath);
	void MapPath(const string& sVirtualPath, const string& sDiskPath);

	// Input file must be relative to proto paths.
	// e.g. ImportProtoFile("bar/foo.proto")
	bool ImportProtoFile(const string& sProtoFile);

	// Make a dynamic Message.
	Message* MakeMessage(const string& sTypeName) const;

	// Encode lua table to string.
	string Encode(lua_State* L, const string& sMsgTypeName, int Index) const;

	// Decode string to lua message table. Return a lua table.
	// Return nil if parse failed.
	int Decode(lua_State* L, const string& sMsgTypeName, const string& sData) const;

private:
	using DiskSourceTree = compiler::DiskSourceTree;
	using Importer = compiler::Importer;
	using MsgFactory = DynamicMessageFactory;

	std::unique_ptr<DiskSourceTree> m_pDiskSourceTree;
	std::unique_ptr<ErrorCollector> m_pErrorCollector;
	std::unique_ptr<Importer> m_pImporter;
	std::unique_ptr<MsgFactory> m_pMsgFactory;
};  // class PbManager

};//end namespace
#endif  // DETAIL_LUAPBINTFIMPL_H
