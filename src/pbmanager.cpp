#include <sstream>  // for ostringstream

#include "pbmanager.hpp"
namespace pb{

class ErrorCollector : public compiler::MultiFileErrorCollector
{
public:
    void Clear() { m_sError.clear(); }
    const std::string& GetError() const { return m_sError; }

    // Only record the last error.
    void AddError(const std::string & filename,
        int line, int column, const std::string & message) override
    {
        std::ostringstream oss;
        oss << filename << ":" << line << ": " << message;
        m_sError = oss.str();
    }
private:
    std::string m_sError;
};


PbManager PbManager::Instance;
PbManager::PbManager()
	: m_pDiskSourceTree(new DiskSourceTree),  // unique_ptr
	m_pErrorCollector(new ErrorCollector),  // unique_ptr
	m_pImporter(new Importer(m_pDiskSourceTree.get(), m_pErrorCollector.get())),//unique_ptr
	m_pMsgFactory(new MsgFactory)  // unique_ptr
{
	// The current dir is the default proto path.
	AddProtoPath("");
}

PbManager::~PbManager()
{
}

// e.g. AddProtoPath("proto")
// e.g. AddProtoPath("d:/proto")
void PbManager::AddProtoPath(const string& sProtoPath)
{
	MapPath("", sProtoPath);
}

void PbManager::MapPath( const string& sVirtualPath, const string& sDiskPath)
{
	m_pDiskSourceTree->MapPath(sVirtualPath, sDiskPath);
}

// e.g. ImportProtoFile("bar/foo.proto")
bool PbManager::ImportProtoFile(const string& sProtoFile)
{
	m_pErrorCollector->Clear();
	const FileDescriptor* pDesc = m_pImporter->Import(sProtoFile);
	if( pDesc ){
		return true;
	}
	return false;
}

Message* PbManager::MakeMessage(const string& sTypeName) const
{
	const Descriptor* pDesc = m_pImporter->pool()->FindMessageTypeByName(sTypeName);
	if( ! pDesc ){
		return nullptr;
	}
	const Message* pProtoType = m_pMsgFactory->GetPrototype(pDesc);
	if( !pProtoType ){
		return nullptr;
	}
	return pProtoType->New();
}

};//end namespace

