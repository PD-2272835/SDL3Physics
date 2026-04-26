#ifndef _BVH_HPP_
#define _BVH_HPP_
#include <vec.hpp> //mfg
#include <vector>
#include "Entity.hpp"

// Adapted from Box2D creator Erin Catto's slides at GDC 2019
// https://box2d.org/files/ErinCatto_DynamicBVH_GDC2019.pdf

typedef struct AABB
{
	mfg::vec3 upperBound;
	mfg::vec3 lowerBound;
};

AABB Union(const AABB A, const AABB B);

struct Node
{
	AABB box;
	//using offsets into the bvh's nodes array to ensure that data remains tightly packed
	size_t objectOffset;
	size_t parentOffset;
	size_t left;
	size_t right;
	bool isLeaf;

	Node(AABB newBox, size_t objOffset, size_t parOffset, size_t leftOffset, size_t rightOffset, bool leaf) :
		box(newBox),
		objectOffset(objOffset),
		parentOffset(parOffset),
		left(leftOffset),
		right(rightOffset),
		isLeaf(leaf)
	{}
};


class bvh
{
	std::vector<Node> nodes;
	size_t rootNodeIndex; //index into the nodes array that contains all physics objects

	bvh();
	bvh(std::vector<AABB> boxes);

	void BottomUpConstruction(std::vector<AABB> boxes);

	void TopDownConstruction(std::vector<AABB> boxes);
};

#endif