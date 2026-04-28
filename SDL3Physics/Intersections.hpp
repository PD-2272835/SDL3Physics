#ifndef _INTERSECTIONS_HPP_
#define _INTERSECTIONS_HPP_
#include "vec.hpp"


typedef struct AABB
{
	mfg::vec3 upperBound;
	mfg::vec3 lowerBound;
	mfg::vec3 center;

	AABB() : upperBound(0, 0, 0), lowerBound(0, 0, 0), center(0, 0, 0) {};

	AABB(mfg::vec3 max, mfg::vec3 min) : upperBound(max), lowerBound(min)
	{
		center = mfg::MidPoint(upperBound, lowerBound);
	}
};

typedef struct Sphere
{
	mfg::vec3 pos;
	float radius;
};

AABB Union(const AABB &a, const AABB &b);

bool Intersects(const AABB& a, const AABB& b);
bool Intersects(const mfg::vec3& point, const AABB box);
bool Intersects(const mfg::vec3& point, const Sphere& sphere);
bool Intersects(const Sphere& a, const Sphere& b);

#endif