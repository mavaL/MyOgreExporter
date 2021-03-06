/********************************************************************
	created:	12:5:2013   18:34
	filename	ExportMesh.h
	author:		maval

	purpose:	导出Mesh相关
	TODO:		双面材质mesh导出
*********************************************************************/
#ifndef ExportMesh_h__
#define ExportMesh_h__

#include "ExportObject.h"
#include "ExportSkeleton.h"
#include "Utility.h"

//Point3 hash特化
namespace std
{
	template<>
	struct hash<typename Point3>
	{
		inline size_t operator()(const Point3& v) const
		{
			size_t seed = 0;
			Utility::hash_combine(seed, v.x);
			Utility::hash_combine(seed, v.y);
			Utility::hash_combine(seed, v.z);
			return seed;
		}
	};
}

class ExpoMesh : public ExpoObject
{
public:
	typedef std::vector<Point3>	VecUV;
	typedef std::unordered_map<Point3, std::vector<unsigned int>>	PosIndexMap;

	struct SVertex
	{
		Point3		position;
		Point3		normal;
		Point3		diffuse;
		float		alpha;
		VecUV		uv;
		std::unordered_map<int, float>* weights;
	};
	typedef std::vector<SVertex>	VertexList;

	struct SFace
	{
		unsigned int	vertexs[3];
	};

	struct SSubMesh
	{
		SSubMesh():matName(""),skeletonName(""),bUse32bitindex(false),
			bHasDiffuse(false),uvCount(0),bSkined(false),pSkeleton(nullptr) {}

		std::string			matName;
		std::string			skeletonName;
		std::vector<SFace>	faces;					//所有面
		VertexList			vertexList;				//所有顶点
		PosIndexMap			posIdxMap;				//方便查找具有相同坐标的顶点的索引
		bool				bUse32bitindex;
		bool				bHasDiffuse;
		int					uvCount;
		bool				bSkined;
		ExpoSkeleton*		pSkeleton;
	};

public:
	ExpoMesh(IGameNode* node);

public:
	virtual eExpoType	GetType()	{ return eExpoType_Mesh; }
	virtual bool	Export();
	virtual bool	CollectInfo();

	void			SetSkeleton(ExpoSkeleton* skel) { m_subMesh.pSkeleton = skel; }
	IGameNode*		GetGameNode() { return m_pNode; }

private:
	bool		_WriteMesh();
	bool		_WriteSubmesh(const SSubMesh& subMesh, TiXmlElement* xmlParent);

private:
	IGameNode*				m_pNode;
	IGameMesh*				m_mesh;
	SSubMesh				m_subMesh;
};

#endif // ExportMesh_h__