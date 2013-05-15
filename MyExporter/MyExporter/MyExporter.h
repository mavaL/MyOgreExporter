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

#include "ExportConfig.h"
#include "Singleton.h"

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
	, public SceneExport
{
	friend class ExpoDlg;
	DECLEAR_SINGLETON(MyExporter)

public:
	//////////////////////////////////////////////////////////////////////
	///////	SceneExport的接口,我们并不用SceneExport的方式,而是UtilityObject,
	//////	用SceneExport是为了能调GetMasterUnitInfo这个函数...
	virtual int				ExtCount()			{ return 0; }
	virtual const MCHAR *	Ext(int n)			{ return "No Use!"; }
	virtual const MCHAR *	LongDesc()			{ return "No Use!"; }
	virtual const MCHAR *	ShortDesc()			{ return "No Use!"; }
	virtual const MCHAR *	AuthorName()		{ return "No Use!"; }
	virtual const MCHAR *	CopyrightMessage()	{ return "No Use!"; }
	virtual const MCHAR *	OtherMessage1()		{ return "No Use!"; }
	virtual const MCHAR *	OtherMessage2()		{ return "No Use!"; }
	virtual unsigned int	Version()			{ return 0; }
	virtual void			ShowAbout(HWND hWnd)	{}
	virtual int				DoExport(const MCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0) { return 0; }

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
	ExpoConfig			config;

private:
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	typedef std::vector<IGameNode*>	RootNodeContainer;
	RootNodeContainer	m_rootNodes;
};



extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
