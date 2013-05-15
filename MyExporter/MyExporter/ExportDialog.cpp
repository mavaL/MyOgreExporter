#include "stdafx.h"
#include "ExportDialog.h"
#include "MyExporter.h"
#include "resource.h"

ExpoDlg::ExpoDlg()
:m_hwnd(nullptr)
,m_hSceneInfo(nullptr)
,m_expo(MyExporter::GetSingletonPtr())
,m_hAnimList(nullptr)
,m_hLog(nullptr)
{

}

HIMAGELIST hImageList = nullptr;

void ExpoDlg::Init(HWND hwnd)
{
	m_hwnd = hwnd;
	m_hSceneInfo	= GetDlgItem(m_hwnd, IDC_Export_SceneInfo);
	m_hAnimList		= GetDlgItem(m_hwnd, IDC_Export_AnimList);
	m_hLog			= GetDlgItem(m_hwnd, IDC_Export_Log);

	hImageList = ImageList_Create(16,16,ILC_COLOR16,2,10);					  
	HBITMAP hBitMap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TREE));					  
	ImageList_Add(hImageList,hBitMap,NULL);								      
	DeleteObject(hBitMap);
	TreeView_SetImageList(m_hSceneInfo, (LPARAM)hImageList, 0);

	//显示MAX场景结构
	TreeView_DeleteAllItems(m_hSceneInfo);
	for (size_t iRootNode=0; iRootNode<m_expo->m_rootNodes.size(); ++iRootNode)
	{
		IGameNode* pRootNode = m_expo->m_rootNodes[iRootNode];
		_InitSceneNodeInfo(pRootNode, nullptr);
	}
}

void ExpoDlg::Destroy()
{

}

INT_PTR CALLBACK ExpoDlg::ExportDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch (message) 
	{
	case WM_INITDIALOG:
		MyExporter::GetSingleton().Init(hWnd);
		break; 

	case WM_DESTROY:
		MyExporter::GetSingleton().Destroy(hWnd);
		break;

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_Export_Cancel: EndDialog(hWnd, 0); break;
			case IDC_Export_Start:	MyExporter::GetSingleton().DoExport(); break;
			default: break;
			}
		}
		break;

	default: return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 1;
}

void ExpoDlg::_InitSceneNodeInfo( IGameNode* pNode, HTREEITEM hParent )
{
	assert(pNode);

	//self
	TVINSERTSTRUCT tvis;
	tvis.hParent = hParent;
	tvis.hInsertAfter = hParent != nullptr ? TVI_LAST : TVI_ROOT;
	tvis.item.mask = LVIF_TEXT|LVIF_IMAGE|TVIF_SELECTEDIMAGE;
	tvis.item.pszText = pNode->GetName();
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 1;

	hParent = TreeView_InsertItem(m_hSceneInfo, &tvis);

	//children
	for (int iChild=0; iChild<pNode->GetChildCount(); ++iChild)
		_InitSceneNodeInfo(pNode->GetNodeChild(iChild), hParent);
}

void ExpoDlg::LogInfo( const std::string& info )
{
	std::string line = info + "\r\n";
	//插入一行
	Edit_SetSel(m_hLog, -1, -1);
	Edit_ReplaceSel(m_hLog, line.c_str());
}
