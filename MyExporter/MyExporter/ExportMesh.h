/********************************************************************
	created:	12:5:2013   18:34
	filename	ExportMesh.h
	author:		maval

	purpose:	导出Mesh相关
*********************************************************************/
#ifndef ExportMesh_h__
#define ExportMesh_h__

#include "ExportObject.h"

class ExpoMesh : public ExpoObject
{
	typedef std::unordered_map<int, Point2>	texUvMap;
	struct SVertex
	{
		Point3		position;
		Point3		normal;
		Point3		diffuse;
		float		alpha;
		texUvMap	uvMap;
	};

	struct SFace
	{
		SVertex		vertexs[3];
	};

	struct SSubMesh
	{
		SSubMesh():matName(""),bUse32bitindex(false),bHasDiffuse(false) {}

		std::string			matName;
		std::vector<SFace>	faces;
		bool				bUse32bitindex;
		bool				bHasDiffuse;
	};

public:
	ExpoMesh(IGameNode* node);

public:
	virtual bool	Export();

private:
	void		_CollectInfo();
	bool		_WriteMesh();
	bool		_WriteSubmesh(const SSubMesh& subMesh, TiXmlElement* xmlParent);

private:
	IGameMesh*				m_mesh;
	std::map<int, Point3>	m_tvMap;	//???
	std::vector<SSubMesh>	m_subMeshes;
};

#endif // ExportMesh_h__