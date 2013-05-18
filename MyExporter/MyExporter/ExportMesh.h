/********************************************************************
	created:	12:5:2013   18:34
	filename	ExportMesh.h
	author:		maval

	purpose:	导出Mesh相关
	TODO:		双面材质mesh导出
	ISSUE		开始没用顶点索引的时候,光照和UV倒是显示正常.加了索引后,就不正常了.
*********************************************************************/
#ifndef ExportMesh_h__
#define ExportMesh_h__

#include "ExportObject.h"
#include "ExportSkeleton.h"

class ExpoMesh : public ExpoObject
{
	typedef std::vector<Point3>	VecUV;
	typedef std::unordered_map<unsigned int, unsigned int>	IndexRemap;

	struct SVertex
	{
		Point3		position;
		Point3		normal;
		Point3		diffuse;
		float		alpha;
		VecUV		uv;
	};

	struct SFace
	{
		unsigned int	vertexs[3];
	};

	struct SSubMesh
	{
		SSubMesh():matName(""),skeletonName(""),bUse32bitindex(false),
			bHasDiffuse(false),uvCount(0),bSkined(false),pSkeleton(nullptr) {}
		~SSubMesh() { SAFE_DELETE(pSkeleton); }

		std::string			matName;
		std::string			skeletonName;
		std::vector<SFace>	faces;
		std::vector<SVertex>vertexList;
		IndexRemap			indexmap;
		bool				bUse32bitindex;
		bool				bHasDiffuse;
		int					uvCount;
		bool				bSkined;
		ExpoSkeleton*		pSkeleton;
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
	IGameNode*				m_pNode;
	IGameMesh*				m_mesh;
	SSubMesh				m_subMesh;
};

#endif // ExportMesh_h__