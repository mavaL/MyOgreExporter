#include "stdafx.h"
#include "ExportSkeleton.h"
#include "MyExporter.h"
#include "ExportDialog.h"
#include "Utility.h"

ExpoSkeleton::ExpoSkeleton( IGameNode* node )
:m_node(node)
{
	assert(m_node->GetIGameObject()->GetIGameType() == IGameObject::IGAME_MESH && "Construct ExpoSkeleton with IGameMesh!");

	m_name = "E:\\3ds Max 2010\\plugins\\";
	m_name += m_node->GetName();
	m_name += ".skeleton";
}

bool ExpoSkeleton::Export()
{
	assert(m_skin && "Should call CollectInfo() first!");
	return false;
}

bool ExpoSkeleton::CollectInfo()
{
	MyExporter& exporter = MyExporter::GetSingleton();
	m_skin = m_node->GetIGameObject()->GetIGameSkin();
	assert(m_skin && "This mesh doesn't have any skin!");

	//TODO:Ŀǰ��֧��Bip����
	Control* nodeControl = m_skin->GetBone(0, false)->GetTMController();
	if ((nodeControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (nodeControl->ClassID() == BIPBODY_CONTROL_CLASS_ID))
	{
		exporter.dlgExpo->LogInfo("Not support physique animation now!!!");
		return false;
	}

	//�ҵ����и�����
	std::vector<INode*> vecRootBones;
	for (int iBone=0; iBone<m_skin->GetTotalBoneCount(); ++iBone)
	{
		INode* pBone = m_skin->GetBone(iBone);
		while (pBone->GetParentNode() != exporter.ip->GetRootNode())
			pBone = pBone->GetParentNode();
		
		if(std::find(vecRootBones.begin(), vecRootBones.end(), pBone) != vecRootBones.end())
			vecRootBones.push_back(pBone);
	}

	//�ռ�������Ϣ
	for (size_t iBone=0; iBone<vecRootBones.size(); ++iBone)
	{
		INode* pRootBone = vecRootBones[iBone];
		if(!_LoadJoint(pRootBone))
			return false;

		//children
		for (int iChild=0; iChild<pRootBone->NumberOfChildren(); ++iChild)
			if(!_LoadJoint(pRootBone->GetChildNode(iChild)))
				return false;
	}

	return true;
}

ExpoSkeleton::SJoint* ExpoSkeleton::_GetJoint( INode* pJoint )
{
	if(!pJoint)
		return nullptr;

	for	(size_t i=0; i<m_joints.size(); ++i)
		if(m_joints[i].boneHandle == pJoint->GetHandle())
			return &m_joints[i];

	return nullptr;
}

bool ExpoSkeleton::_LoadJoint( INode* pJoint )
{
	MyExporter& exporter = MyExporter::GetSingleton();

	//�ܿ϶���jointδ������
	if (_GetJoint(pJoint))
	{
		exporter.dlgExpo->LogInfo("This seems shouldn't be happening, but joint already loaded!!!");
		return false;
	}

	SJoint joint;
	//̽�����������ظ���joint
	joint.name = pJoint->GetName();
	for	(size_t i=0; i<m_joints.size(); ++i)
	{
		if (m_joints[i].name == joint.name)
		{
			char szBuf[128];
			sprintf_s(szBuf, 128, "Warning: Bone name duplicated.	[%s]", joint.name);
			exporter.dlgExpo->LogInfo(szBuf);
		}
	}

	//bone handle
	joint.boneHandle = pJoint->GetHandle();

	//parent handle
	SJoint* parent = _GetJoint(pJoint->GetParentNode());
	joint.parentHandle = parent ? parent->boneHandle : -1;

	//��ȡjoint�任
// 	Matrix3 transTM;
// 	if (parent == nullptr)
// 		transTM = Utility::GetWorldTransform(pJoint);
// 	else
// 		transTM = Utility::GetLocalTransform(pJoint);

	return true;
}
