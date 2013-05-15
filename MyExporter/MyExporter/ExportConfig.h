/********************************************************************
	created:	15:5:2013   11:08
	filename	ExportConfig.h
	author:		maval

	purpose:	��������
*********************************************************************/
#ifndef ExportConfig_h__
#define ExportConfig_h__

#include "Singleton.h"

class ExpoConfig : public CSingleton<ExpoConfig>
{
	DECLEAR_SINGLETON(ExpoConfig)
public:
	ExpoConfig();

	static INT_PTR CALLBACK DlgConfigProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	void			SetUnitSetup(int unitType, float unitScale);

public:
	HWND			m_hwnd;
	bool			m_bUnitMeter;						//���������Ƿ�ת��Ϊ��Ϊ��λ
	float			m_unitScale;						//��λת������
	bool			m_bBuildNormal;						//��max�ؽ�mesh�ķ���
	std::string		m_defaultMaterialName;
	IGameConversionManager::CoordSystem	m_coordSystem;	//��������ϵ
};

#endif // ExportConfig_h__