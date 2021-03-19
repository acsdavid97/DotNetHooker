// dllmain.h : Declaration of module class.

class CDotNetHookerModule : public ATL::CAtlDllModuleT< CDotNetHookerModule >
{
public :
	DECLARE_LIBID(LIBID_DotNetHookerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DOTNETHOOKER, "{a7bc3ae9-619c-4155-a15c-1badfdd18bd1}")
};

extern class CDotNetHookerModule _AtlModule;
