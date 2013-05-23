/********************************************************************
	created:	15:5:2013   12:52
	filename	ExportSkeleton.h
	author:		maval

	purpose:	�������������Ϣ
*********************************************************************/
#ifndef ExportSkeleton_h__
#define ExportSkeleton_h__

#include "ExportObject.h"

class ExpoMesh;
class ExpoClip;

class ExpoSkeleton : public ExpoObject
{
public:
	struct SJoint 
	{
		SJoint():name(""),node(nullptr),boneHandle(-1),parent(nullptr),boneID(-1),
			position(-1,-1,-1),rotation(0.0f,0.0f,0.0f,1.0f),scale(1,1,1) {}

		SJoint*		parent;
		IGameNode*	node;
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
	typedef std::vector<SVertexAssignment> VertAssignments;

public:
	ExpoSkeleton(ExpoObject* parent);
	~ExpoSkeleton();

public:
	virtual eExpoType	GetType()	{ return eExpoType_Skeleton; }
	virtual bool	Export();
	virtual bool	CollectInfo();
	VertAssignments&	GetVertexAssigns() { return m_vertAssigns; }
	const JointList&	GetJoints() const { return m_joints; }
	IGameSkin*		GetSkin()	{ return m_skin; }
	void			AddClip(ExpoClip* clip);
	void			DeleteClip(const std::string name);

private:
	bool			_LoadJoint(IGameNode* pJoint, SJoint* parent);
	SJoint*			_GetJoint(IGameNode* pJoint);

private:
	ExpoMesh*		m_parent;
	IGameSkin*		m_skin;
	JointList		m_joints;
	VertAssignments	m_vertAssigns;

	typedef std::vector<ExpoClip*>	ClipContainer;
	ClipContainer	m_clips;
};

#endif // ExportSkeleton_h__