#pragma once

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
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "utilapi.h"

#include "Singleton.h"
#include "ExportDialog.h"

enum eExpoType
{
	eExpoType_Mesh,
	eExpoType_Material,
	eExpoType_Skeleton
};

class ExpoObject;

//////////////////////////////////////////////////////////////////////////
class MyExporter 
	: public UtilityObj
	, public CSingleton<MyExporter>
{
	friend class ExpoDlg;
	DECLEAR_SINGLETON(MyExporter)

public:
	//Constructor/Destructor
	MyExporter();
	virtual ~MyExporter();

	virtual void DeleteThis() { }		

	virtual void BeginEditParams(Interface *ip,IUtil *iu);
	virtual void EndEditParams(Interface *ip,IUtil *iu);

	virtual void Init(HWND hWnd);
	virtual void Destroy(HWND hWnd);

	void		 DoExport();
	void		 DoExport(eExpoType type, IGameNode* node);
	void		 DoExport(ExpoObject* obj);

public:
	HWND				hPanel;
	IGameScene*			pScene;
	Interface*			ip;
	IUtil*				iu;
	ExpoDlg*			dlgExpo;

private:
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	typedef std::vector<IGameNode*>	RootNodeContainer;
	RootNodeContainer	m_rootNodes;
};


#define MYEXPORTER	MyExporter::GetSingleton()


extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
