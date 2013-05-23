#include "stdafx.h"
#include "ExportClip.h"
#include "MyExporter.h"
#include "ExportConfig.h"

ExpoClip::ExpoClip(ExpoObject* owner)
:m_start(-1)
,m_end(-1)
,m_length(-1)
{
	m_pOwner = dynamic_cast<ExpoSkeleton*>(owner);
}

ExpoClip::~ExpoClip()
{
	//与骨骼脱离关联
	if(m_pOwner)
		m_pOwner->DeleteClip(m_name);
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

void ExpoClip::_CollectTrack( STrack& track, ExpoSkeleton::SJoint* joint )
{
	track.boneName = joint->name;

	//统一转换成tick
	int nStartTick = m_start * GetTicksPerFrame();
	int nEndTick = m_end * GetTicksPerFrame();
	int nRate = CONFIG.m_clipSampleRate * TIME_TICKSPERSEC;

	int t;
	for (t=nStartTick; t<=nEndTick; t+=nRate)
		_CollectKeyFrame(track, joint, t);
	//最后一个key
	if(t != nEndTick)
		_CollectKeyFrame(track, joint, nEndTick);

	if(CONFIG.m_bCopyFirstFrameAsLast)
	{
		auto& kfs = track.keyFrames;
		kfs[kfs.size()-1] = kfs[0];
		kfs[kfs.size()-1].time = m_length;
	}
}

void ExpoClip::_CollectKeyFrame(STrack& track, ExpoSkeleton::SJoint* joint, int t)
{
	const GMatrix tm = joint->node->GetLocalTM(t);
	const GMatrix relTM = joint->node->GetLocalTM(t) * joint->node->GetLocalTM().Inverse();

	SKeyFrame kf;
	kf.position = tm.Translation() - joint->position;
	kf.rotation = relTM.Rotation();
	kf.scale = relTM.Scaling();

	AngAxis rot(kf.rotation);
	rot.angle = -rot.angle;
	kf.rotation.Set(rot);

	kf.time = (t - m_start * GetTicksPerFrame()) / (float)TIME_TICKSPERSEC * CONFIG.m_clipLengthScale;

	track.keyFrames.push_back(kf);
}
