/********************************************************************
	created:	15:5:2013   12:52
	filename	ExportSkeleton.h
	author:		maval

	purpose:	������������
*********************************************************************/
#ifndef ExportSkeleton_h__
#define ExportSkeleton_h__

#include "ExportObject.h"

class ExpoMesh;

class ExpoSkeleton : public ExpoObject
{
public:
	struct SJoint 
	{
		SJoint():name(""),boneHandle(-1),parent(nullptr),boneID(-1),
			position(-1,-1,-1),rotation(0.0f,0.0f,0.0f,1.0f),scale(1,1,1) {}

		SJoint*		parent;
		std::string	name;
		int			boneID;			//.skeleton�еĹ���ID.������boneHandle,��Ϊ������OGRE����vector��ȡ,Ҫ��ID����~
		int			boneHandle;		//3DMAX�Ľڵ�handle
		Point3		position;
		Quat		rotation;
		Point3		scale;
	};
	typedef std::vector<SJoint*> JointList;

	struct SVertexAssignment 
	{
		std::unordered_map<int, float> weights;
	};
	typedef std::map<DWORD, SVertexAssignment> VertAssignmentMap;

public:
	ExpoSkeleton(IGameNode* node);
	~ExpoSkeleton();

public:
	virtual bool	Export();
	bool			CollectInfo(ExpoMesh* parent);
	VertAssignmentMap& GetVertexAssigns() { return m_vertAssigns; }


private:
	bool			_LoadJoint(IGameNode* pJoint, SJoint* parent);
	SJoint*			_GetJoint(IGameNode* pJoint);

private:
	IGameNode*		m_node;
	IGameSkin*		m_skin;
	JointList		m_joints;
	VertAssignmentMap	m_vertAssigns;
};

#endif // ExportSkeleton_h__