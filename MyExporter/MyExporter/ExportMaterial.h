/********************************************************************
	created:	14:5:2013   16:35
	filename	ExportMaterial.h
	author:		maval

	purpose:	导出材质相关
*********************************************************************/
#ifndef ExportMaterial_h__
#define ExportMaterial_h__

#include "ExportObject.h"

class ExpoMaterial : public ExpoObject
{
public:
	ExpoMaterial(IGameNode* node);

public:
	virtual eExpoType	GetType()	{ return eExpoType_Material; }
	virtual bool	Export();
	virtual	bool	CollectInfo();

private:
	bool			_StreamPass(std::ostream &of, IGameMaterial *mtl);

private:
	IGameNode*		m_node;
	IGameMaterial*	m_material;
	std::string		m_stream;
};

#endif // ExportMaterial_h__