/********************************************************************
	created:	13:5:2013   10:24
	filename	ExportObject.h
	author:		maval

	purpose:	导出各类型物体基类
*********************************************************************/
#ifndef ExportObject_h__
#define ExportObject_h__

#include "MyExporter.h"

class IGameNode;
class IGameMesh;

class ExpoObject
{
public:
	ExpoObject();
	virtual ~ExpoObject() {}

public:
	virtual bool	Export() = 0;

public:
	MyExporter*		m_exporter;
};

#endif // ExportObject_h__