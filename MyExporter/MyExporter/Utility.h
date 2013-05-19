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

	template <class T>
	inline void hash_combine(std::size_t & seed, const T & v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
}

#endif // Utility_h__