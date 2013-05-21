#include "stdafx.h"
#include "ExportClip.h"
#include "MyExporter.h"

ExpoClip::ExpoClip(ExpoObject* owner)
:m_start(-1)
,m_end(-1)
,m_length(-1)
{
	m_pOwner = dynamic_cast<ExpoSkeleton*>(owner);
}

bool ExpoClip::CollectInfo()
{
	const SClipParam& param = MYEXPORTER.GetClip(m_name);
	m_start	= param.nStart;
	m_end	= param.nEnd;
	m_length = param.length;

	if(m_start < 0 || m_end < 0 || m_start >= m_end)
	{
		MessageBox(0, "The start/end frame is invalid!", "Add Clip", MB_OK);
		return false;
	}

	auto& joints = m_pOwner->GetJoints();
	m_tracks.resize(joints.size());

	for (size_t iJoint=0; iJoint<joints.size(); ++iJoint)
		_CollectTrack(m_tracks[iJoint], joints[iJoint]);

	//与骨骼关联
	m_pOwner->AddClip(this);
	return true;
}

ExpoClip::~ExpoClip()
{
	//与骨骼脱离关联
	if(m_pOwner)
		m_pOwner->DeleteClip(m_name);
}

void ExpoClip::_CollectTrack( STrack& track, ExpoSkeleton::SJoint* joint )
{
	track.boneName = joint->name;
}

void ExpoClip::_CollectKeyFrame()
{

}
