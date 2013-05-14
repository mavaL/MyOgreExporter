//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "PluginWrapper.h"

#define PluginWrapper_CLASS_ID	Class_ID(0x72f014d8, 0xbcb20d31)


class PluginWrapper : public UtilityObj 
{
public:
		
	//Constructor/Destructor
	PluginWrapper();
	virtual ~PluginWrapper();

	virtual void DeleteThis() { }		
	
	virtual void BeginEditParams(Interface *ip,IUtil *iu);
	virtual void EndEditParams(Interface *ip,IUtil *iu);
	
	// Singleton access
	static PluginWrapper* GetInstance() { 
		static PluginWrapper thePluginWrapper;
		return &thePluginWrapper; 
	}

private:
	HMODULE		m_hExporter;
};


class PluginWrapperClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return PluginWrapper::GetInstance(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return PluginWrapper_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("PluginWrapper"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetPluginWrapperDesc() { 
	static PluginWrapperClassDesc PluginWrapperDesc;
	return &PluginWrapperDesc; 
}




//--- PluginWrapper -------------------------------------------------------
PluginWrapper::PluginWrapper()
:m_hExporter(nullptr)
{
}

PluginWrapper::~PluginWrapper()
{

}

typedef void (*EXPORT_FUNC)(Interface* ip,IUtil* iu);

void PluginWrapper::BeginEditParams(Interface* ip,IUtil* iu) 
{
	m_hExporter = ::LoadLibrary("F:\\MyOgreExporter\\MyExporter\\Build\\Bin\\Debug\\MyExporter.dlu");

	if(!m_hExporter)
	{
		MessageBox(ip->GetMAXHWnd(), "Load Exporter.dll failed!", "Error", MB_ICONERROR);
		return;
	}

	EXPORT_FUNC initFunc = (EXPORT_FUNC)::GetProcAddress(m_hExporter, "InitExporter");
	
	if(!initFunc)
	{
		MessageBox(ip->GetMAXHWnd(), "Get InitExporter() failed!", "Error", MB_ICONERROR);
		return;
	}

	initFunc(ip, iu);
}
	
void PluginWrapper::EndEditParams(Interface* ip,IUtil* iu) 
{
	EXPORT_FUNC destroyFunc = (EXPORT_FUNC)::GetProcAddress(m_hExporter, "DestroyExporter");

	if(!destroyFunc)
	{
		MessageBox(ip->GetMAXHWnd(), "Get DestroyExporter() failed!", "Error", MB_ICONERROR);
		return;
	}

	destroyFunc(ip, iu);

	::FreeLibrary(m_hExporter);
	m_hExporter = nullptr;
}
