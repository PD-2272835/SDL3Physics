#ifndef _INTERSECTIONS_HPP_
#define _INTERSECTIONS_HPP_
#include "vec.hpp"
#include "functions.hpp"


typedef struct AABB
{
	sgm::vec3 upperBound;
	sgm::vec3 lowerBound;
	sgm::vec3 center;

	AABB() : upperBound(0, 0, 0), lowerBound(0, 0, 0), center(0, 0, 0) {};

	AABB(sgm::vec3 max, sgm::vec3 min) : upperBound(max), lowerBound(min)
	{
		center = sgm::MidPoint(upperBound, lowerBound);
	}
};

typedef struct Sphere
{
	sgm::vec3 pos;
	float radius;
};

AABB Union(const AABB &a, const AABB &b);

bool Intersects(const AABB& a, const AABB& b);
bool Intersects(const sgm::vec3& point, const AABB& box);
bool Intersects(const sgm::vec3& point, const Sphere& sphere);
bool Intersects(const Sphere& a, const Sphere& b);
bool Intersects(const AABB& a, const Sphere& b);

#endif