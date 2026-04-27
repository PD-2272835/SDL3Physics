#include "Intersections.hpp"


//Union
AABB Union(const AABB& a, const AABB& b)
{
	AABB res;
	res.upperBound = mfg::Max(a.upperBound, b.upperBound);
	res.lowerBound = mfg::Min(a.lowerBound, b.lowerBound);
	res.center = mfg::MidPoint(res.upperBound, res.lowerBound);
	return res;
}


//Imtersections

//Intersection test between two AABBs
bool Intersects(const AABB& a, const AABB& b)
{
	return (
		a.lowerBound.x() <= b.upperBound.x() &&
		a.upperBound.x() >= b.lowerBound.x() &&
		a.lowerBound.y() <= b.upperBound.y() &&
		a.upperBound.y() >= b.lowerBound.y() &&
		a.lowerBound.z() <= b.upperBound.z() &&
		a.upperBound.z() >= b.lowerBound.z());
}

//Intersection test between a point and an AABB
bool Intersects(const mfg::vec3& point, const AABB box)
{
	return (
		point.x() >= box.lowerBound.x() &&
		point.x() <= box.upperBound.x() &&
		point.y() >= box.lowerBound.y() &&
		point.y() <= box.upperBound.y() &&
		point.z() >= box.lowerBound.z() &&
		point.z() <= box.upperBound.z());
}

//Intersection test between a point and a Sphere
bool Intersects(const mfg::vec3& point, const Sphere& sphere)
{
	float dist = mfg::Distance(point, sphere.pos);
	return (dist < sphere.radius);
}

//Intersection test between two spheres
bool Intersects(const Sphere& a, const Sphere& b)
{
	float dist = mfg::Distance(a.pos, b.pos);
	return (dist < a.radius + b.radius);
}

