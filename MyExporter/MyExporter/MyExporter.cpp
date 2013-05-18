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
#include "ExportConfig.h"
#include "ExportMesh.h"
#include "ExportMaterial.h"
#include "ExportSkeleton.h"

#define MyExporter_CLASS_ID	Class_ID(0x8b0c44de, 0x651b6a99)

__declspec( dllexport ) void InitExporter(Interface* ip,IUtil* iu)
{
	MyExporter::GetSingleton().BeginEditParams(ip, iu);
}

__declspec( dllexport ) void DestroyExporter(Interface* ip,IUtil* iu)
{
	MyExporter::GetSingleton().EndEditParams(ip, iu);
}

class MyExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return MyExporter::GetSingletonPtr(); }
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
	dlgExpo = new ExpoDlg;
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
	MyExporter& exporter = MyExporter::GetSingleton();
	switch (msg) 
	{
		case WM_COMMAND:
			{
				if (LOWORD(wParam) == IDC_EnterExporter)
					DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DlgExport), 
					exporter.ip->GetMAXHWnd(), ExpoDlg::ExportDlgProc, (LPARAM)0);
				else if	(LOWORD(wParam) == IDC_Exporter_Options)
					DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DlgExportConfig), 
					exporter.ip->GetMAXHWnd(), ExpoConfig::DlgConfigProc, (LPARAM)0);
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			exporter.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
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

	//ogre坐标系
	IGameConversionManager * cm = GetConversionManager();
	cm->SetCoordSystem(CONFIG.m_coordSystem);

	if(!pScene->InitialiseIGame())
		MessageBox(ip->GetMAXHWnd(), "InitialiseIGame() failed!", "Error", MB_ICONERROR);
	pScene->SetStaticFrame(0);

	for (int iRootNode=0; iRootNode<pScene->GetTopLevelNodeCount(); ++iRootNode)
	{
		IGameNode* pRootNode = pScene->GetTopLevelNode(iRootNode);
		m_rootNodes.push_back(pRootNode);
	}

	dlgExpo->Init(hWnd);
}

void MyExporter::Destroy(HWND hWnd)
{
	m_rootNodes.clear();
	pScene->ReleaseIGame();
	dlgExpo->Destroy();
}

void MyExporter::DoExport()
{
	//确定单位
	int unitType;
	float unitScale;
	GetMasterUnitInfo(&unitType, &unitScale);
	CONFIG.SetUnitSetup(unitType, unitScale);

	for (size_t i=0; i<m_rootNodes.size(); ++i)
	{
		if(m_rootNodes[i]->GetIGameObject()->GetIGameType() == IGameObject::IGAME_MESH)
			DoExport(eExpoType_Mesh, m_rootNodes[i]);
	}
}

void MyExporter::DoExport( eExpoType type, IGameNode* node )
{
	ExpoObject* obj = nullptr;

	switch (type)
	{
	case eExpoType_Mesh:		obj = new ExpoMesh(node); break;
	case eExpoType_Material:	obj = new ExpoMaterial(node); break;
	case eExpoType_Skeleton:	obj = new ExpoSkeleton(node); break;
	default:					dlgExpo->LogInfo("Not support export type!!!"); break;
	}

	DoExport(obj);

	SAFE_DELETE(obj);
}

void MyExporter::DoExport( ExpoObject* obj )
{
	std::string expoName("Exporting ");
	expoName += obj->GetName();

	if(obj->Export())
	{
		expoName += " succeed.";
		dlgExpo->LogInfo(expoName);
	}
	else
	{
		expoName += " failed!!!";
		dlgExpo->LogInfo(expoName);
	}
}
