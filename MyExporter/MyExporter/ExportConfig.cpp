#include "stdafx.h"
#include "ExportConfig.h"
#include "resource.h"

ExpoConfig::ExpoConfig()
:m_hwnd(nullptr)
{
	m_bUnitMeter			=	false;
	m_unitScale				=	1.0f;
	m_bBuildNormal			=	false;
	m_defaultMaterialName	=	"DefaultMaterial";
	m_coordSystem			=	IGameConversionManager::IGAME_OGL;
}

INT_PTR CALLBACK ExpoConfig::DlgConfigProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam )
{
	ExpoConfig& config = ExpoConfig::GetSingleton();

	switch (message) 
	{
	case WM_INITDIALOG:
		{
			config.m_hwnd = hWnd;
			CheckRadioButton(hWnd, IDC_Config_Coord3DMax, IDC_Config_CoordOpenGL, IDC_Config_Coord3DMax + config.m_coordSystem);
			CheckDlgButton(hWnd, IDC_Config_BuildNormal, config.m_bBuildNormal);
			CheckDlgButton(hWnd, IDC_Config_UnitMeter, config.m_bUnitMeter);
		}
		break; 

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_Config_Coord3DMax:
			case IDC_Config_CoordD3D:
			case IDC_Config_CoordOpenGL:
				{
					IGameConversionManager::CoordSystem type = decltype(type)(LOWORD(wParam) - IDC_Config_Coord3DMax);
					config.m_coordSystem = type;
				}
				break;

			case IDC_Config_BuildNormal:	config.m_bBuildNormal = IsDlgButtonChecked(hWnd, IDC_Config_BuildNormal); break;
			case IDC_Config_UnitMeter:		config.m_bUnitMeter = IsDlgButtonChecked(hWnd, IDC_Config_UnitMeter); break;

			case IDOK: EndDialog(hWnd, 0); break;
			default: break;
			}
		}
		break;

	default: return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 1;
}

void ExpoConfig::SetUnitSetup( int unitType, float unitScale )
{
	// Units conversion
	const float MM2M = 0.001f;
	const float CM2M = 0.01f;
	const float M2M  =  1.0f;
	const float KM2M = 1000.0f;
	const float IN2M = 0.0254f;
	const float FT2M = 0.3048f;
	const float ML2M = 1609.344f;

	switch(unitType)
	{
	case UNITS_INCHES:		m_unitScale = IN2M; break;
	case UNITS_FEET:		m_unitScale = FT2M; break;
	case UNITS_MILES:		m_unitScale = ML2M; break;
	case UNITS_MILLIMETERS: m_unitScale = MM2M; break;
	case UNITS_CENTIMETERS: m_unitScale = CM2M; break;
	case UNITS_METERS:		m_unitScale = M2M;  break;
	case UNITS_KILOMETERS:	m_unitScale = KM2M; break;
	}

	m_unitScale *= unitScale;
}
