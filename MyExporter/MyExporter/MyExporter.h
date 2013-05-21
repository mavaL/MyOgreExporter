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
#include "ExportObject.h"

typedef std::vector<ExpoObject*>	ExpoContainer;

struct SClipParam 
{
	SClipParam():nStart(-1),nEnd(-1),length(-1) {}

	int		nStart, nEnd;
	float	length;
};
typedef std::unordered_map<std::string, SClipParam>	Clips;

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

	void		Init();
	void		Destroy();

	bool		DoCollectInfo(eExpoType type, IGameNode* node = nullptr, 
		ExpoObject* parent = nullptr, const std::string& name = "");
	void		DoExport();

	const ExpoContainer&	GetExpoObjects() const { return m_expoObjects; }

	bool		AddClip(const std::string& name, const SClipParam& clip);
	const SClipParam&	GetClip(const std::string& name);
	void		DeleteClip(const std::string& name);

public:
	HWND				hPanel;
	IGameScene*			pScene;
	Interface*			ip;
	IUtil*				iu;
	ExpoDlg*			dlgExpo;

	int					startFrame;	//开始/结束关键帧
	int					endFrame;
	Clips				clips;

private:
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	ExpoContainer		m_expoObjects;
};


#define MYEXPORTER	MyExporter::GetSingleton()


extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
