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
#include "ExportClip.h"

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

	startFrame = -1;
	endFrame = -1;
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
				switch (LOWORD(wParam))
				{
				case IDC_EnterExporter:
					DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DlgExport), 
						exporter.ip->GetMAXHWnd(), ExpoDlg::ExportDlgProc, (LPARAM)0);
					break;

				case IDC_Exporter_Options:
					DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DlgExportConfig), 
						exporter.ip->GetMAXHWnd(), ExpoConfig::DlgConfigProc, (LPARAM)0);
					break;

				default: return FALSE;
				}
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			exporter.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

void MyExporter::Init()
{
	pScene = GetIGameInterface();

	//ogre坐标系
	IGameConversionManager * cm = GetConversionManager();
	cm->SetCoordSystem(CONFIG.m_coordSystem);

	if(!pScene->InitialiseIGame())
	{
		MessageBox(ip->GetMAXHWnd(), "InitialiseIGame() failed!", "Error", MB_ICONERROR);
		return;
	}
	pScene->SetStaticFrame(0);

	startFrame = pScene->GetSceneStartTime() / pScene->GetSceneTicks();
	endFrame = pScene->GetSceneEndTime() / pScene->GetSceneTicks();

	//确定度量
	int unitType;
	float unitScale;
	GetMasterUnitInfo(&unitType, &unitScale);
	CONFIG.SetUnitSetup(unitType, unitScale);

	//收集所有需导出物体的数据
	std::vector<IGameNode*> vecRoot;
	for (int iRootNode=0; iRootNode<pScene->GetTopLevelNodeCount(); ++iRootNode)
	{
		IGameNode* pRootNode = pScene->GetTopLevelNode(iRootNode);
		
		if(pRootNode->GetIGameObject()->GetIGameType() == IGameObject::IGAME_MESH)
			DoCollectInfo(eExpoType_Mesh, pRootNode);

		vecRoot.push_back(pRootNode);
	}

	dlgExpo->Init(vecRoot);
}

void MyExporter::Destroy()
{
	dlgExpo->Destroy();
	pScene->ReleaseIGame();

	for(size_t i=0; i<m_expoObjects.size(); ++i)
		delete m_expoObjects[i];
	m_expoObjects.clear();
}

bool MyExporter::DoCollectInfo( eExpoType type, IGameNode* node, ExpoObject* parent, const std::string& name )
{
	ExpoObject* obj = nullptr;

	switch (type)
	{
	case eExpoType_Mesh:		obj = new ExpoMesh(node); break;
	case eExpoType_Material:	obj = new ExpoMaterial(node); break;
	case eExpoType_Skeleton:	obj = new ExpoSkeleton(parent); break;
	case eExpoType_Clip:		obj = new ExpoClip(parent); break;
	default:					dlgExpo->LogInfo("Not support exporting object type!!!"); break;
	}

	if(name.length())
		obj->SetName(name);

	if(!obj->CollectInfo())
	{
		char msg[MAX_PATH];
		sprintf_s(msg, MAX_PATH, "Error: Collecting info failed!!!	[%s]", obj->GetName().c_str());
		dlgExpo->LogInfo(msg);

		return false;
	}
	else
	{
		char msg[MAX_PATH];
		sprintf_s(msg, MAX_PATH, "Collecting info succeed.	[%s]", obj->GetName().c_str());
		dlgExpo->LogInfo(msg);
	}

	m_expoObjects.push_back(obj);

	return true;
}

void MyExporter::DoExport()
{
	//导出所有物体
	for (size_t i=0; i<m_expoObjects.size(); ++i)
	{
		std::string expoName("Exporting ");
		expoName += m_expoObjects[i]->GetName();

		if(m_expoObjects[i]->Export())
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
}

bool MyExporter::AddClip( const std::string& name, const SClipParam& clip )
{
	assert(clips.find(name) == clips.end());
	clips.insert(std::make_pair(name, clip));

	//对每个skeleton都要加入该动画
	for(size_t i=0; i<m_expoObjects.size(); ++i)
	{
		if (m_expoObjects[i]->GetType() == eExpoType_Skeleton)
		{
			if(!DoCollectInfo(eExpoType_Clip, nullptr, m_expoObjects[i], name))
				return false;
		}
	}

	return true;
}

void MyExporter::DeleteClip( const std::string& name )
{
	auto iter = clips.find(name);
	assert(iter != clips.end());
	clips.erase(iter);

	for(auto itObj=m_expoObjects.begin(); itObj!=m_expoObjects.end();/* ++itObj*/)
	{
		if ((*itObj)->GetType() == eExpoType_Clip && (*itObj)->GetName() == name)
		{
			delete *itObj;
			itObj = m_expoObjects.erase(itObj);
		}
		else
		{
			++itObj;
		}
	}
}

const SClipParam& MyExporter::GetClip( const std::string& name )
{
	auto iter = clips.find(name);
	assert(iter != clips.end());
	return iter->second;
}
