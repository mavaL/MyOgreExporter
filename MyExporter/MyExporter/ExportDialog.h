/********************************************************************
	created:	12:5:2013   19:30
	filename	ExportDialog.h
	author:		maval

	purpose:	�����������
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
	static INT_PTR CALLBACK EditClipDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	void		Init(const std::vector<IGameNode*>& vecRootNodes);
	void		Destroy();
	bool		OnAddClip(HWND hwnd);
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