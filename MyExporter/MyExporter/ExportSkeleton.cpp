#include "stdafx.h"
#include "ExportSkeleton.h"
#include "MyExporter.h"
#include "ExportDialog.h"
#include "Utility.h"
#include "ExportMesh.h"

ExpoSkeleton::ExpoSkeleton( IGameNode* node )
:m_node(node)
{
	assert(m_node->GetIGameObject()->GetIGameType() == IGameObject::IGAME_MESH && "Construct ExpoSkeleton with IGameMesh!");

	m_name = "E:\\3ds Max 2010\\plugins\\";
	m_name += m_node->GetName();
	m_name += ".skeleton.xml";
}

ExpoSkeleton::~ExpoSkeleton()
{
	for (size_t iJoint=0; iJoint<m_joints.size(); ++iJoint)
		SAFE_DELETE(m_joints[iJoint]);
	m_joints.clear();
}

bool ExpoSkeleton::Export()
{
	assert(m_skin && "Should call CollectInfo() first!");
	
	TiXmlDocument *pXmlDoc = new TiXmlDocument();  
	TiXmlDeclaration* declarationElem = new TiXmlDeclaration(_T("1.0"), _T(""), _T(""));  
	pXmlDoc->LinkEndChild(declarationElem);  

	//skeleton
	TiXmlElement* skelNode = new TiXmlElement("skeleton");  
	pXmlDoc->LinkEndChild(skelNode);  
	skelNode->SetAttribute("blendmode", "average");
	
	//bones
	TiXmlElement* bones = new TiXmlElement("bones");  
	skelNode->LinkEndChild(bones);

	//each bone
	for (size_t iJoint=0; iJoint<m_joints.size(); ++iJoint)
	{
		const SJoint* joint = m_joints[iJoint];

		TiXmlElement* jointNode = new TiXmlElement("bone");  
		bones->LinkEndChild(jointNode);
		jointNode->SetAttribute("id", joint->boneID);
		jointNode->SetAttribute("name", joint->name.c_str());

		//position  
		TiXmlElement* position = new TiXmlElement("position");  
		jointNode->LinkEndChild(position);  
		position->SetDoubleAttribute("x", joint->position.x);  
		position->SetDoubleAttribute("y", joint->position.y);  
		position->SetDoubleAttribute("z", joint->position.z);

		//rotation -- axis and angle
		AngAxis rot(joint->rotation);

		TiXmlElement* rotation = new TiXmlElement("rotation");  
		jointNode->LinkEndChild(rotation);  
		rotation->SetDoubleAttribute("angle", rot.angle);  
		
		TiXmlElement* rotAxis = new TiXmlElement("axis");  
		rotation->LinkEndChild(rotAxis);  
		rotAxis->SetDoubleAttribute("x", rot.axis.x);  
		rotAxis->SetDoubleAttribute("y", rot.axis.y);  
		rotAxis->SetDoubleAttribute("z", rot.axis.z);

		//scale
		TiXmlElement* scale = new TiXmlElement("scale");  
		jointNode->LinkEndChild(scale);  
		scale->SetDoubleAttribute("x", joint->scale.x);  
		scale->SetDoubleAttribute("y", joint->scale.y);  
		scale->SetDoubleAttribute("z", joint->scale.z);
	}

	//bonehierarchy
	TiXmlElement* hierarchy = new TiXmlElement("bonehierarchy");  
	skelNode->LinkEndChild(hierarchy);

	for (size_t iJoint=0; iJoint<m_joints.size(); ++iJoint)
	{
		SJoint* parent = m_joints[iJoint]->parent;
		TiXmlElement* boneparent = new TiXmlElement("boneparent");  
		hierarchy->LinkEndChild(boneparent);  
		boneparent->SetAttribute("bone", m_joints[iJoint]->name.c_str());
		boneparent->SetAttribute("parent", parent ? parent->name.c_str() : "");
	}

	pXmlDoc->SaveFile(m_name.c_str());
	delete pXmlDoc;

	return true;
}

bool ExpoSkeleton::CollectInfo(ExpoMesh* parent)
{
	MyExporter& exporter = MyExporter::GetSingleton();
	m_skin = m_node->GetIGameObject()->GetIGameSkin();
	assert(m_skin && "This mesh doesn't have any skin!");

	//TODO:目前不支持Bip动画
	Control* nodeControl = m_skin->GetBone(0, false)->GetTMController();
	if ((nodeControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (nodeControl->ClassID() == BIPBODY_CONTROL_CLASS_ID))
	{
		exporter.dlgExpo->LogInfo("Not support physique animation now!!!");
		return false;
	}

	//找到所有根骨骼
	std::vector<IGameNode*> vecRootBones;
	for (int iBone=0; iBone<m_skin->GetTotalBoneCount(); ++iBone)
	{
		IGameNode* pBone = m_skin->GetIGameBone(iBone);
		while (pBone->GetNodeParent())
			pBone = pBone->GetNodeParent();
		
		if(std::find(vecRootBones.begin(), vecRootBones.end(), pBone) == vecRootBones.end())
			vecRootBones.push_back(pBone);
	}

	//收集骨骼信息
	for (size_t iBone=0; iBone<vecRootBones.size(); ++iBone)
	{
		IGameNode* pRootBone = vecRootBones[iBone];
		if(!_LoadJoint(pRootBone, nullptr))
			return false;
	}

	//蒙皮信息
	IGameMesh* mesh = dynamic_cast<IGameMesh*>(m_node->GetIGameObject());
	int numSkinedVert = m_skin->GetNumOfSkinnedVerts();

	if(numSkinedVert < mesh->GetNumberOfVerts())
	{
		exporter.dlgExpo->LogInfo("Error: There are some vertexs not get skinned!!!");
		return false;
	}

	//以三角面来进行遍历,因为我们想获取每个蒙皮顶点的顶点索引
	auto& indexMap = parent->GetIndexMap();
	int iFaceCount = mesh->GetNumberOfFaces();
	for (int iFace=0; iFace<iFaceCount; ++iFace)
	{
		FaceEx* pFace = mesh->GetFace(iFace);
		for (int i = 0; i < 3; i++)
		{
			DWORD vertexIndex = pFace->vert[i];
			IGameSkin::VertexType type = m_skin->GetVertexType(pFace->vert[i]);
			auto& weightMap = m_vertAssigns[indexMap[pFace->vert[i]]].weights;

			if (type == IGameSkin::IGAME_RIGID)		//角色头部可能是这种类型
			{
				IGameNode* pBoneNode = m_skin->GetIGameBone(vertexIndex, 0);
				const SJoint* joint = _GetJoint(pBoneNode);

				if(weightMap.find(joint->boneID) == weightMap.end())
					weightMap.insert(std::make_pair(joint->boneID, 1.0f));	//rigid weight, no blend
			}
			else if (type == IGameSkin::IGAME_RIGID_BLENDED)	//正常多骨骼混合类型
			{
				int numWeights = m_skin->GetNumberOfBones(vertexIndex);
				for(int iWt=0; iWt<numWeights; ++iWt)
				{
					IGameNode* pBoneNode = m_skin->GetIGameBone(vertexIndex, iWt);
					float weight = m_skin->GetWeight(vertexIndex, iWt);
					const SJoint* joint = _GetJoint(pBoneNode);

					if(weightMap.find(joint->boneID) == weightMap.end())
						weightMap.insert(std::make_pair(joint->boneID, weight));
				}

				if(numWeights > 4)
				{
					exporter.dlgExpo->LogInfo("Warning: There are some vertexs skinned with more than 4 bones!!!");
				}
			}
			else
			{
				exporter.dlgExpo->LogInfo("Warning: Unknown vertex skin type!!!");
			}
		}
	}

	return true;
}

ExpoSkeleton::SJoint* ExpoSkeleton::_GetJoint( IGameNode* pJoint )
{
	if(!pJoint)
		return nullptr;

	for	(size_t i=0; i<m_joints.size(); ++i)
		if(m_joints[i]->boneHandle == pJoint->GetNodeID())
			return m_joints[i];

	return nullptr;
}

bool ExpoSkeleton::_LoadJoint( IGameNode* pJoint, SJoint* parent )
{
	MyExporter& exporter = MyExporter::GetSingleton();

	//能肯定该joint未被解析
	if (_GetJoint(pJoint))
	{
		exporter.dlgExpo->LogInfo("This seems shouldn't be happening, but joint already loaded!!!");
		return false;
	}

	SJoint* joint = new SJoint;
	//探测有无名字重复的joint
	joint->name = pJoint->GetName();
	for	(size_t i=0; i<m_joints.size(); ++i)
	{
		if (m_joints[i]->name == joint->name)
		{
			char szBuf[128];
			sprintf_s(szBuf, 128, "Warning: Bone name duplicated.	 [%s]", joint->name.c_str());
			exporter.dlgExpo->LogInfo(szBuf);
		}
	}

	//bone handle
	joint->boneHandle = pJoint->GetNodeID();

	//parent handle
	joint->parent = parent;

	//获取joint变换
	const GMatrix tm = Utility::GetLocalTransform(pJoint, pJoint->GetNodeParent());
	
	joint->position = tm.Translation();
	joint->rotation = tm.Rotation();
	joint->scale	= tm.Scaling();

	//why??
	AngAxis rot(joint->rotation);
	rot.angle = -rot.angle;
	joint->rotation.Set(rot);

	m_joints.push_back(joint);

	//bone id
	joint->boneID = m_joints.size() - 1;

	//children
	for (int iChild=0; iChild<pJoint->GetChildCount(); ++iChild)
		if(!_LoadJoint(pJoint->GetNodeChild(iChild), joint))
			return false;

	return true;
}
