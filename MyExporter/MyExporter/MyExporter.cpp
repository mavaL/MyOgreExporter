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
#include "stdafx.h"
#include "MyExporter.h"
#include "resource.h"
#include "ExportDialog.h"
#include "ExportMesh.h"

#define MyExporter_CLASS_ID	Class_ID(0x8b0c44de, 0x651b6a99)

MyExporter g_MyExporter;

__declspec( dllexport ) void InitExporter(Interface* ip,IUtil* iu)
{
	g_MyExporter.BeginEditParams(ip, iu);
}

__declspec( dllexport ) void DestroyExporter(Interface* ip,IUtil* iu)
{
	g_MyExporter.EndEditParams(ip, iu);
}

class MyExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return &g_MyExporter; }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return MyExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("MyExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetMyExporterDesc() { 
	static MyExporterClassDesc MyExporterDesc;
	return &MyExporterDesc; 
}


//--- MyExporter -------------------------------------------------------
MyExporter::MyExporter()
{
	iu = nullptr;
	ip = nullptr;	
	hPanel = nullptr;
	pScene = nullptr;
	dlgExpo = new ExpoDlg(this);
}

MyExporter::~MyExporter()
{
	SAFE_DELETE(dlgExpo);
}

void MyExporter::BeginEditParams(Interface* ip,IUtil* iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		DlgProc,
		GetString(IDS_PARAMS),
		0);
}
	
void MyExporter::EndEditParams(Interface* ip,IUtil* iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

INT_PTR CALLBACK MyExporter::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
			g_MyExporter.Init(hWnd);
			break;

		case WM_DESTROY:
			g_MyExporter.Destroy(hWnd);
			break;

		case WM_COMMAND:
			{
				if (LOWORD(wParam) == IDC_EnterExporter)
					DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DlgExport), 
					g_MyExporter.ip->GetMAXHWnd(), ExpoDlg::ExportDlgProc, (LPARAM)0);
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			g_MyExporter.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return 0;
	}
	return 1;
}

void MyExporter::Init(HWND hWnd)
{
	pScene = GetIGameInterface();
	m_rootNodes.clear();

	//ogre×ø±êÏµ
	IGameConversionManager * cm = GetConversionManager();
	cm->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	if(!pScene->InitialiseIGame())
		MessageBox(ip->GetMAXHWnd(), "InitialiseIGame() failed!", "Error", MB_ICONERROR);
	pScene->SetStaticFrame(0);

	for (int iRootNode=0; iRootNode<pScene->GetTopLevelNodeCount(); ++iRootNode)
	{
		IGameNode* pRootNode = pScene->GetTopLevelNode(iRootNode);
		m_rootNodes.push_back(pRootNode);
	}
}

void MyExporter::Destroy(HWND hWnd)
{
	m_rootNodes.clear();
	pScene->ReleaseIGame();
}

void MyExporter::DoExport()
{
	if(m_rootNodes.size() > 1)
		MessageBox(ip->GetMAXHWnd(), "Only support export one object now!", "Warning", MB_ICONWARNING);

	if (!m_rootNodes.empty())
	{
		ExpoObject* obj = nullptr;
		auto type = m_rootNodes[0]->GetIGameObject()->GetIGameType();
		switch (type)
		{
		case IGameObject::IGAME_MESH:
			{
				obj = new ExpoMesh(m_rootNodes[0]);
			}
			break;

		default:
			{
				MessageBox(ip->GetMAXHWnd(), "Not support export type!", "Ogre exporter", MB_OK);
			}
			return;
		}
		
		obj->Export();
// 		if(obj->Export())
// 			MessageBox(ip->GetMAXHWnd(), "Export finished.", "Finished", MB_OK);
// 		else
// 			MessageBox(ip->GetMAXHWnd(), "Export failed.", "Finished", MB_OK);

		SAFE_DELETE(obj);
	}
}
