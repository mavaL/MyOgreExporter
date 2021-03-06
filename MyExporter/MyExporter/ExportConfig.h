/********************************************************************
	created:	15:5:2013   11:08
	filename	ExportConfig.h
	author:		maval

	purpose:	导出设置
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
	bool			m_bUnitMeter;						//导出物体是否转换为米为单位
	float			m_unitScale;						//单位转换乘数
	bool			m_bBuildNormal;						//由max重建mesh的法线
	std::string		m_defaultMaterialName;
	float			m_clipSampleRate;					//动画采样的时间间隔,秒
	bool			m_bCopyFirstFrameAsLast;			//让动画最后一帧与第一帧重合(适合循环动画)
	float			m_clipLengthScale;					//动画快慢控制
	IGameConversionManager::CoordSystem	m_coordSystem;	//导出坐标系
};

#define CONFIG		ExpoConfig::GetSingleton()

#endif // ExportConfig_h__