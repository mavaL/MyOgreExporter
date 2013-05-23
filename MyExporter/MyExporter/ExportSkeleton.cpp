#include "stdafx.h"
#include "ExportSkeleton.h"
#include "MyExporter.h"
#include "ExportDialog.h"
#include "Utility.h"
#include "ExportMesh.h"
#include "ExportClip.h"
#include "ExportConfig.h"

ExpoSkeleton::ExpoSkeleton( ExpoObject* parent )
{
	m_parent = dynamic_cast<ExpoMesh*>(parent);
	m_parent->SetSkeleton(this);
	assert(m_parent->GetGameNode()->GetIGameObject()->GetIGameType() == IGameObject::IGAME_MESH && "Construct ExpoSkeleton with IGameMesh!");

	m_name = "E:\\3ds Max 2010\\plugins\\";
	m_name += m_parent->GetGameNode()->GetName();
	m_name += ".skeleton.xml";
}

ExpoSkeleton::~ExpoSkeleton()
{
	for (size_t iJoint=0; iJoint<m_joints.size(); ++iJoint)
		SAFE_DELETE(m_joints[iJoint]);
	m_joints.clear();

	for (size_t iClip=0; iClip<m_clips.size(); ++iClip)
		m_clips[iClip]->DetachOwner();
	m_clips.clear();
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
		if(!parent)
			continue;

		TiXmlElement* boneparent = new TiXmlElement("boneparent");  
		hierarchy->LinkEndChild(boneparent);  
		boneparent->SetAttribute("bone", m_joints[iJoint]->name.c_str());
		boneparent->SetAttribute("parent", parent->name.c_str());
	}

	//animations
	TiXmlElement* animations = new TiXmlElement("animations");  
	skelNode->LinkEndChild(animations);

	//per animation
	for (size_t iClip=0; iClip<m_clips.size(); ++iClip)
	{
		const ExpoClip* clip = m_clips[iClip];
		TiXmlElement* animation = new TiXmlElement("animation");  
		animations->LinkEndChild(animation);
		animation->SetAttribute("name", clip->m_name.c_str());
		animation->SetDoubleAttribute("length", clip->m_length);

		//per track
		TiXmlElement* tracks = new TiXmlElement("tracks");  
		animation->LinkEndChild(tracks);

		for (size_t iTrack=0; iTrack<clip->m_tracks.size(); ++iTrack)
		{
			const ExpoClip::STrack& track = clip->m_tracks[iTrack];
			TiXmlElement* trackNode = new TiXmlElement("track");  
			tracks->LinkEndChild(trackNode);
			trackNode->SetAttribute("bone", track.boneName.c_str());

			//per keyframe
			TiXmlElement* keyframes = new TiXmlElement("keyframes");  
			trackNode->LinkEndChild(keyframes);

			for (size_t iKf=0; iKf<track.keyFrames.size(); ++iKf)
			{
				const ExpoClip::SKeyFrame& kf = track.keyFrames[iKf];
				TiXmlElement* kfNode = new TiXmlElement("keyframe");  
				keyframes->LinkEndChild(kfNode);
				kfNode->SetDoubleAttribute("time", kf.time);

				//translation  
				TiXmlElement* translate = new TiXmlElement("translate");  
				kfNode->LinkEndChild(translate);  
				translate->SetDoubleAttribute("x", kf.position.x);  
				translate->SetDoubleAttribute("y", kf.position.y);  
				translate->SetDoubleAttribute("z", kf.position.z);

				//rotation
				AngAxis rot(kf.rotation);
				TiXmlElement* rotate = new TiXmlElement("rotate");  
				kfNode->LinkEndChild(rotate);  
				rotate->SetDoubleAttribute("angle", rot.angle);  

				TiXmlElement* rotAxis = new TiXmlElement("axis");  
				rotate->LinkEndChild(rotAxis);  
				rotAxis->SetDoubleAttribute("x", rot.axis.x);  
				rotAxis->SetDoubleAttribute("y", rot.axis.y);  
				rotAxis->SetDoubleAttribute("z", rot.axis.z);

				//scale
				TiXmlElement* scale = new TiXmlElement("scale");  
				kfNode->LinkEndChild(scale);
				scale->SetDoubleAttribute("x", kf.scale.x);  
				scale->SetDoubleAttribute("y", kf.scale.y);  
				scale->SetDoubleAttribute("z", kf.scale.z);
			}
		}
	}

	pXmlDoc->SaveFile(m_name.c_str());
	delete pXmlDoc;

	return true;
}

bool ExpoSkeleton::CollectInfo()
{
	MyExporter& exporter = MyExporter::GetSingleton();
	m_skin = m_parent->GetGameNode()->GetIGameObject()->GetIGameSkin();
	assert(m_skin && "This mesh doesn't have any skin!");

	//TODO:目前不支持Bip动画
	Control* nodeControl = m_skin->GetBone(0, false)->GetTMController();
	if ((nodeControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (nodeControl->ClassID() == BIPBODY_CONTROL_CLASS_ID))
	{
		exporter.dlgExpo->LogInfo("Not support physique animation now!!!");
		return false;
	}

	//找到所有根骨骼
	std::set<IGameNode*> rootBones;
	for (int iBone=0; iBone<m_skin->GetTotalBoneCount(); ++iBone)
	{
		IGameNode* pBone = m_skin->GetIGameBone(iBone);
		while (	pBone->GetNodeParent() )
		{
			if(pBone->GetNodeParent()->GetMaxNode() == exporter.ip->GetRootNode())
				break;
			pBone = pBone->GetNodeParent();
		}
		
		rootBones.insert(pBone);
	}

	//收集骨骼信息
	for (auto iBone=rootBones.begin(); iBone!=rootBones.end(); ++iBone)
	{
		IGameNode* pRootBone = *iBone;
		if(!_LoadJoint(pRootBone, nullptr))
			return false;
	}

	/////蒙皮信息
	IGameMesh* mesh = dynamic_cast<IGameMesh*>(m_parent->GetGameNode()->GetIGameObject());
	int numSkinedVert = m_skin->GetNumOfSkinnedVerts();

	if(numSkinedVert < mesh->GetNumberOfVerts())
	{
		exporter.dlgExpo->LogInfo("Error: There are some vertexs not get skinned!!!");
		return false;
	}

	m_vertAssigns.resize(numSkinedVert);
	for (size_t i=0; i<numSkinedVert; ++i)
	{
		IGameSkin::VertexType type = m_skin->GetVertexType(i);
		auto& weightMap = m_vertAssigns[i].weights;

		if (type == IGameSkin::IGAME_RIGID)		//角色头部可能是这种类型
		{
			IGameNode* pBoneNode = m_skin->GetIGameBone(i, 0);
			const SJoint* joint = _GetJoint(pBoneNode);
			weightMap[joint->boneID] = 1.0f;	//rigid weight, no blend
		}
		else if (type == IGameSkin::IGAME_RIGID_BLENDED)	//正常多骨骼混合类型
		{
			int numWeights = m_skin->GetNumberOfBones(i);
			for(int iWt=0; iWt<numWeights; ++iWt)
			{
				IGameNode* pBoneNode = m_skin->GetIGameBone(i, iWt);
				float weight = m_skin->GetWeight(i, iWt);
				const SJoint* joint = _GetJoint(pBoneNode);
				weightMap[joint->boneID] = weight;
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
	assert(pJoint);

// 	if(m_skin->GetBoneIndex(pJoint) == -1)
// 		return true;

	if (_GetJoint(pJoint))
	{
// 		char szBuf[128];
// 		sprintf_s(szBuf, 128, "Error:The joint already loaded!!!	 [%s]", pJoint->GetName());
// 		exporter.dlgExpo->LogInfo(szBuf);
//		return false;
		return true;
	}

	SJoint* joint = new SJoint;
	joint->node = pJoint;
	//探测有无名字重复的joint
	joint->name = pJoint->GetName();
	for	(size_t i=0; i<m_joints.size(); ++i)
	{
		if (m_joints[i]->name == joint->name)
		{
// 			char szBuf[128];
// 			sprintf_s(szBuf, 128, "Warning: Bone name duplicated.	 [%s]", joint->name.c_str());
// 			exporter.dlgExpo->LogInfo(szBuf);
			return true;
		}
	}

	//bone handle
	joint->boneHandle = pJoint->GetNodeID();

	//parent handle
	joint->parent = parent;

	//获取joint变换
	const GMatrix tm = pJoint->GetLocalTM();
	
	joint->position = tm.Translation();		//???????????????????????????是否需要转换度量
	joint->rotation = tm.Rotation();
	joint->scale	= tm.Scaling();

	//注意MAX的变换是左手系,故角度也是
	AngAxis rot(joint->rotation);
	if(CONFIG.m_coordSystem == IGameConversionManager::IGAME_OGL)
		rot.angle = -rot.angle;
	joint->rotation.Set(rot);

	m_joints.push_back(joint);

	//bone id
	joint->boneID = m_joints.size() - 1;

	//children
	for (int iChild=0; iChild<pJoint->GetChildCount(); ++iChild)
	{
		if(!_LoadJoint(pJoint->GetNodeChild(iChild), joint))
			return false;
	}

	return true;
}

void ExpoSkeleton::AddClip( ExpoClip* clip )
{
	assert(clip);
	m_clips.push_back(clip);
}

void ExpoSkeleton::DeleteClip( const std::string name )
{
	for	(auto iter=m_clips.begin(); iter!=m_clips.end(); ++iter)
	{
		if ((*iter)->GetName() == name)
		{
			m_clips.erase(iter);
			break;
		}
	}
}
