/********************************************************************
	created:	12:5:2013   19:30
	filename	ExportDialog.h
	author:		maval

	purpose:	导出操作面板
*********************************************************************/
#ifndef ExportDialog_h__
#define ExportDialog_h__

class MyExporter;

class ExpoDlg
{
public:
	ExpoDlg(MyExporter* expo);

public:
	static INT_PTR CALLBACK ExportDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	void		Init(HWND hwnd);
	void		Destroy();

private:
	void		_InitSceneNodeInfo(IGameNode* pNode, HTREEITEM hParent);

private:
	HWND		m_hwnd;
	MyExporter*	m_pExpo;
	HWND		m_hSceneInfo;
};

#endif // ExportDialog_h__