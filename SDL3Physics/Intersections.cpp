#include "Intersections.hpp"


//Union
AABB Union(const AABB& a, const AABB& b)
{
	AABB res;
	res.upperBound = sgm::Max(a.upperBound, b.upperBound);
	res.lowerBound = sgm::Min(a.lowerBound, b.lowerBound);
	res.center = sgm::MidPoint(res.upperBound, res.lowerBound);
	return res;
}


//Imtersections

//Intersection test between two AABBs
bool Intersects(const AABB& a, const AABB& b)
{
	return (
		(a.lowerBound.x() <= b.upperBound.x() &&
		a.upperBound.x() >= b.lowerBound.x()) &&
		(a.lowerBound.y() <= b.upperBound.y() &&
		a.upperBound.y() >= b.lowerBound.y()) &&
		(a.lowerBound.z() <= b.upperBound.z() &&
		a.upperBound.z() >= b.lowerBound.z()));
}

//Intersection test between a point and an AABB
bool Intersects(const sgm::vec3& point, const AABB& box)
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
bool Intersects(const sgm::vec3& point, const Sphere& sphere)
{
	float dist = sgm::Distance(point, sphere.pos);
	return (dist < sphere.radius);
}

//Intersection test between two spheres
bool Intersects(const Sphere& a, const Sphere& b)
{
	float dist = sgm::Distance(a.pos, b.pos);
	return (dist < a.radius + b.radius);
}

//Intersection test between a box and a sphere (depends on sphere and point test)
bool Intersects(const AABB& box, const Sphere& sphere)
{
	sgm::vec3 closestPoint = sgm::vec3(
		sgm::Max(box.lowerBound.x(), sgm::Min(sphere.pos.x(), box.upperBound.x())),
		sgm::Max(box.lowerBound.y(), sgm::Min(sphere.pos.y(), box.upperBound.y())),
		sgm::Max(box.lowerBound.z(), sgm::Min(sphere.pos.z(), box.upperBound.z()))
	);
	
	return Intersects(closestPoint, sphere);
}
