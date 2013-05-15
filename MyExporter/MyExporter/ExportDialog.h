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
	ExpoDlg();

public:
	static INT_PTR CALLBACK ExportDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	void		Init(HWND hwnd);
	void		Destroy();
	void		LogInfo(const std::string& info);

private:
	void		_InitSceneNodeInfo(IGameNode* pNode, HTREEITEM hParent);

private:
	MyExporter*	m_expo;
	HWND		m_hwnd;
	HWND		m_hSceneInfo;
	HWND		m_hAnimList;
	HWND		m_hLog;
};

#endif // ExportDialog_h__