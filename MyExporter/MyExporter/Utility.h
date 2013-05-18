/********************************************************************
	created:	16:5:2013   2:01
	filename	Utility.h
	author:		maval

	purpose:	¹¤¾ßº¯Êý
*********************************************************************/
#ifndef Utility_h__
#define Utility_h__

namespace Utility
{
	GMatrix		GetLocalTransform(IGameNode* from, IGameNode* to, TimeValue t=TIME_NegInfinity);
}

#endif // Utility_h__