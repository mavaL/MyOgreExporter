/********************************************************************
	created:	12:5:2013   18:34
	filename	ExportMesh.h
	author:		maval

	purpose:	����Mesh���
	TODO:		˫�����mesh����
*********************************************************************/
#ifndef ExportMesh_h__
#define ExportMesh_h__

#include "ExportObject.h"
#include "ExportSkeleton.h"
#include "Utility.h"

//Point3 hash�ػ�
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
	typedef std::vector<Point3>	VecUV;
	typedef std::unordered_map<unsigned int, unsigned int>	IndexRemap;
	typedef std::unordered_map<Point3, std::vector<unsigned int>>	PosIndexMap;

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
		std::vector<SFace>	faces;					//������
		std::vector<SVertex>vertexList;				//���ж���
		IndexRemap			indexmap;				//MAX��������->vertexList�е�����
		PosIndexMap			posIdxMap;				//������Ҿ�����ͬ����Ķ��������
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