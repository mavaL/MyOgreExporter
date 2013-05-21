/********************************************************************
	created:	21:5:2013   10:00
	filename	ExportClip.h
	author:		maval

	purpose:	导出动画相关信息
*********************************************************************/
#ifndef ExportClip_h__
#define ExportClip_h__

#include "ExportObject.h"
#include "ExportSkeleton.h"

class ExpoClip : public ExpoObject
{
public:
	struct SKeyFrame 
	{
		SKeyFrame():time(-1),position(-1,-1,-1),rotation(0.0f,0.0f,0.0f,1.0f),scale(1,1,1) {}

		int		time;
		Point3	position;
		Quat	rotation;
		Point3	scale;
	};

	struct STrack 
	{
		std::string	boneName;
		std::vector<SKeyFrame>	keyFrames;
	};
	typedef std::vector<STrack>	TrackContainer;

public:
	ExpoClip(ExpoObject* owner);
	~ExpoClip();

public:
	virtual eExpoType	GetType()		{ return eExpoType_Clip; }
	virtual bool		CollectInfo();
	virtual bool		Export()		{ return true; /*在CollectInfo中已将动画数据注入ExpoSkeleton*/ }
	void				DetachOwner()	{ m_pOwner = nullptr; }

private:
	void			_CollectTrack(STrack& track, ExpoSkeleton::SJoint* joint);
	void			_CollectKeyFrame();

private:
	ExpoSkeleton*	m_pOwner;
	int				m_start, m_end;		//起始/结束关键帧
	float			m_length;			//动画长度,秒
	TrackContainer	m_tracks;
};

#endif // ExportClip_h__