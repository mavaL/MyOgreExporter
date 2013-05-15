#include "stdafx.h"
#include "Utility.h"

namespace Utility
{
	Matrix3 GetWorldTransform( INode* pNode, int time )
	{
		return pNode->GetNodeTM(time);
	}

	Matrix3 GetLocalTransform( INode* pNode, int time )
	{
		Matrix3 worldTM = pNode->GetNodeTM(time);

		if(pNode->GetParentNode()->IsRootNode())
			return worldTM;

		return worldTM * Inverse(pNode->GetParentNode()->GetNodeTM(time));
	}

}