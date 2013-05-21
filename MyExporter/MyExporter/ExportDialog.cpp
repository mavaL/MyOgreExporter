#include "stdafx.h"
#include "ExportDialog.h"
#include "MyExporter.h"
#include "resource.h"

enum eClipViewColumn
{
	eCVC_Name,
	eCVC_Frame,
	eCVC_Length
};

ExpoDlg::ExpoDlg()
:m_hwnd(nullptr)
,m_hSceneInfo(nullptr)
,m_expo(MyExporter::GetSingletonPtr())
,m_hAnimList(nullptr)
,m_hLog(nullptr)
{

}

void ExpoDlg::Init(const std::vector<IGameNode*>& vecRootNodes)
{
	HIMAGELIST hImageList = ImageList_Create(16,16,ILC_COLOR16,2,10);					  
	HBITMAP hBitMap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TREE));					  
	ImageList_Add(hImageList,hBitMap,NULL);								      
	DeleteObject(hBitMap);
	TreeView_SetImageList(m_hSceneInfo, (LPARAM)hImageList, 0);

	//显示MAX场景结构
	TreeView_DeleteAllItems(m_hSceneInfo);
	for (size_t iRootNode=0; iRootNode<vecRootNodes.size(); ++iRootNode)
	{
		IGameNode* pRootNode = vecRootNodes[iRootNode];
		_InitSceneNodeInfo(pRootNode, nullptr);
	}

	//是否有骨骼
	BOOL bSkeleton = FALSE;
	auto& expoObjects = m_expo->GetExpoObjects();
	for(size_t i=0; i<expoObjects.size(); ++i)
	{
		if (expoObjects[i]->GetType() == eExpoType_Skeleton)
		{
			bSkeleton = TRUE;
			break;
		}
	}

	EnableWindow(GetDlgItem(m_hwnd, IDC_Export_AnimAdd), bSkeleton);
	EnableWindow(GetDlgItem(m_hwnd, IDC_Export_AnimEdit), FALSE);
	EnableWindow(GetDlgItem(m_hwnd, IDC_Export_AnimRemove), FALSE);

	///动画列表控件
	m_hAnimList = ::GetDlgItem(m_hwnd, IDC_Export_AnimList);
	int style = LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT;
	ListView_SetExtendedListViewStyleEx(m_hAnimList, style, style);

	LVCOLUMN col;
	col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	col.cx = 150;
	col.iSubItem = eCVC_Name;
	col.pszText = "Name";
	ListView_InsertColumn(m_hAnimList, eCVC_Name, &col);

	col.cx = 150;
	col.iSubItem = eCVC_Frame;
	col.pszText = "Frames";
	ListView_InsertColumn(m_hAnimList, eCVC_Frame, &col);

	col.cx = 150;
	col.iSubItem = eCVC_Length;
	col.pszText = "Length";
	ListView_InsertColumn(m_hAnimList, eCVC_Length, &col);
}

void ExpoDlg::Destroy()
{

}

INT_PTR CALLBACK ExpoDlg::ExportDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	MyExporter& expo = MyExporter::GetSingleton();
	static int curSelClip = -1;

	switch (message) 
	{
	case WM_INITDIALOG:
		{
			expo.dlgExpo->m_hwnd = hWnd;
			expo.dlgExpo->m_hSceneInfo	= GetDlgItem(hWnd, IDC_Export_SceneInfo);
			expo.dlgExpo->m_hAnimList	= GetDlgItem(hWnd, IDC_Export_AnimList);
			expo.dlgExpo->m_hLog		= GetDlgItem(hWnd, IDC_Export_Log);

			expo.Init();
		}

		break; 

	case WM_DESTROY:
		expo.Destroy();
		break;

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_Export_Cancel: EndDialog(hWnd, 0); break;
			case IDC_Export_Start:	expo.DoExport(); break;

			case IDC_Export_AnimAdd: 
				DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DlgAddClip), 
					expo.ip->GetMAXHWnd(), ExpoDlg::EditClipDlgProc, (LPARAM)0);
				break;

			case IDC_Export_AnimRemove:
				{
					char szText[128];
					ListView_GetItemText(expo.dlgExpo->m_hAnimList, curSelClip, 0, szText, _countof(szText));
					expo.DeleteClip(szText);
					ListView_DeleteItem(expo.dlgExpo->m_hAnimList, curSelClip);

					curSelClip = -1;
					EnableWindow(GetDlgItem(hWnd, IDC_Export_AnimEdit), FALSE);
					EnableWindow(GetDlgItem(hWnd, IDC_Export_AnimRemove), FALSE);
				}
				break;

			default: return FALSE;
			}
		}
		break;

	case WM_NOTIFY:
		{
			NMHDR* pHdr = ((LPNMHDR)lParam);
			if(pHdr->idFrom == IDC_Export_AnimList && pHdr->code == NM_CLICK)
			{
				curSelClip = ListView_GetNextItem(expo.dlgExpo->m_hAnimList, -1, LVNI_SELECTED);
				if(curSelClip != -1)
				{
					EnableWindow(GetDlgItem(hWnd, IDC_Export_AnimEdit), TRUE);
					EnableWindow(GetDlgItem(hWnd, IDC_Export_AnimRemove), TRUE);
				}
				else
				{
					EnableWindow(GetDlgItem(hWnd, IDC_Export_AnimEdit), FALSE);
					EnableWindow(GetDlgItem(hWnd, IDC_Export_AnimRemove), FALSE);
				}
			}
		}
		break;

	default: return FALSE;
	}

	return TRUE;
}

INT_PTR CALLBACK ExpoDlg::EditClipDlgProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam )
{
	MyExporter& expo = MyExporter::GetSingleton();
	switch (message) 
	{
	case WM_INITDIALOG:
		{
			SetDlgItemInt(hWnd, IDC_AddClip_Start, expo.startFrame, FALSE);
			SetDlgItemInt(hWnd, IDC_AddClip_End, expo.endFrame, FALSE);
		}
		break;

	case WM_SHOWWINDOW: ::SetFocus(GetDlgItem(hWnd, IDC_AddClip_Name)); break;

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_AddClip_Ok:		
				{ 
					if(expo.dlgExpo->OnAddClip(hWnd))
						EndDialog(hWnd, 1); 
				} 
				break;
			case IDC_AddClip_Cancel: EndDialog(hWnd, 0); break;

			case IDC_AddClip_Start:
				{
					//动画起始帧不可小于最小帧
					if (HIWORD(wParam) == EN_KILLFOCUS)
					{
						char szBuf[128];
						::GetDlgItemText(hWnd, IDC_AddClip_Start, szBuf, 128);
						int s = ::atoi(szBuf);
						if (s < expo.startFrame)
							::SetDlgItemInt(hWnd, IDC_AddClip_Start, expo.startFrame, FALSE);
					}
				}
				break;

			case IDC_AddClip_End:
				{
					//动画结束帧不可大于最大帧
					if (HIWORD(wParam) == EN_KILLFOCUS)
					{
						char szBuf[128];
						::GetDlgItemText(hWnd, IDC_AddClip_End, szBuf, 128);
						int e = ::atoi(szBuf);
						if (e > expo.endFrame)
							::SetDlgItemInt(hWnd, IDC_AddClip_End, expo.endFrame, FALSE);
					}
				}
				break;

			default: return FALSE;
			}
		}
		break;

	default: return FALSE;
	}

	return TRUE;
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

bool ExpoDlg::OnAddClip( HWND hwnd )
{
	char clipName[256], clipStart[64], clipEnd[64];
	::GetDlgItemText(hwnd, IDC_AddClip_Name, clipName, _countof(clipName));
	::GetDlgItemText(hwnd, IDC_AddClip_Start, clipStart, _countof(clipStart));
	::GetDlgItemText(hwnd, IDC_AddClip_End, clipEnd, _countof(clipEnd));

	//动画名字是否为空
	if (clipName[0] == 0)
	{
		MessageBox(hwnd, "This animation name is empty!", "Add Clip", MB_OK);
		return false;
	}

	auto& expoObjects = m_expo->GetExpoObjects();
	//动画名字是否重复
	for(size_t i=0; i<expoObjects.size(); ++i)
	{
		if(expoObjects[i]->GetType() == eExpoType_Clip &&
			expoObjects[i]->GetName() == clipName)
		{
			MessageBox(hwnd, "This animation name is already exist!", "Add Clip", MB_OK);
			return false;
		}
	}

	int nStart = ::atoi(clipStart);
	int nEnd = ::atoi(clipEnd);

	SClipParam clip;
	clip.nStart = nStart;
	clip.nEnd = nEnd;
	clip.length = (nEnd - nStart) / (float)GetFrameRate();

	if(m_expo->AddClip(clipName, clip))
	{
		std::string strFrame(clipStart);
		strFrame += "-";
		strFrame += clipEnd;

		char len[64];
		sprintf_s(len, _countof(len), "%f", clip.length);

		LVITEM lvItem = {0};
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = ListView_GetItemCount(m_hAnimList);
		lvItem.iSubItem = 0;
		lvItem.pszText = "";

		int item = ListView_InsertItem(m_hAnimList, (LPARAM)&lvItem);
		ListView_SetItemText(m_hAnimList,item, eCVC_Name, clipName);         
		ListView_SetItemText(m_hAnimList,item, eCVC_Frame, (LPSTR)strFrame.c_str());  
		ListView_SetItemText(m_hAnimList,item, eCVC_Length, len);

		return true;
	}
	else
	{
		return false;
	}
}
