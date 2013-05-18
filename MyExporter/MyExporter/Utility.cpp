#include "stdafx.h"
#include "Utility.h"

namespace Utility
{
	GMatrix GetLocalTransform( IGameNode* from, IGameNode* to, TimeValue t )
	{
		if(!from && !to)
			return GMatrix();
		else if(!from)
			return to->GetWorldTM(t).Inverse();
		else if(!to)
			return from->GetWorldTM(t);
		else
			return from->GetWorldTM(t) * to->GetWorldTM(t).Inverse();
	}

}