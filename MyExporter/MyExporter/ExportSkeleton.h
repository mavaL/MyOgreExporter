/********************************************************************
	created:	15:5:2013   12:52
	filename	ExportSkeleton.h
	author:		maval

	purpose:	µ¼³ö¹Ç÷À¶¯»­
*********************************************************************/
#ifndef ExportSkeleton_h__
#define ExportSkeleton_h__

#include "ExportObject.h"

class ExpoSkeleton : public ExpoObject
{
	struct SJoint 
	{
		SJoint():name(""),boneHandle(-1),parent(nullptr),
			position(-1,-1,-1),rotation(0.0f,0.0f,0.0f,1.0f),scale(1,1,1) {}

		SJoint*		parent;
		std::string	name;
		int			boneHandle;
		Point3		position;
		Quat		rotation;
		Point3		scale;
	};
	typedef std::vector<SJoint*> JointList;

public:
	ExpoSkeleton(IGameNode* node);
	~ExpoSkeleton();

public:
	virtual bool	Export();
	bool			CollectInfo();

private:
	bool			_LoadJoint(IGameNode* pJoint, SJoint* parent);
	SJoint*			_GetJoint(IGameNode* pJoint);

private:
	IGameNode*		m_node;
	IGameSkin*		m_skin;
	JointList		m_joints;
};

#endif // ExportSkeleton_h__