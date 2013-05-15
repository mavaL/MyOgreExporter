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
	Matrix3		GetWorldTransform(INode* pNode, int time = 0);
	Matrix3		GetLocalTransform(INode* pNode, int time = 0);
}

#endif // Utility_h__