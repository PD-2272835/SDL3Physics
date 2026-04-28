#ifndef _BVH_HPP_
#define _BVH_HPP_
#include <vec.hpp> //mfg
#include <functions.hpp>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Intersections.hpp"

// Adapted from Box2D creator Erin Catto's slides at GDC 2019
// https://box2d.org/files/ErinCatto_DynamicBVH_GDC2019.pdf

struct BVHNode
{
	AABB box;
	size_t object; // this value is an offset for bottom up or a morton code for top down
	size_t parentOffset;
	size_t left;
	size_t right;

	BVHNode(AABB newBox, size_t objOffset, size_t parOffset, size_t leftOffset, size_t rightOffset) : 
			box(newBox), object(objOffset), parentOffset(parOffset), left(leftOffset), right(rightOffset)
	{
	};

	bool isLeaf() const { return left < 0 && right < 0; } //this node is a leaf if left and right are both not set
};

uint64_t BitExpansion(uint64_t x);
uint64_t Create3DMorton(float x, float y, float z, const uint32_t worldSize);


class bvh
{
	std::vector<BVHNode> nodes;
	size_t rootNodeIndex; //index into the nodes array that contains all physics objects
	size_t worldSize;

	bvh();
	bvh(const std::vector<AABB>& boxes, const size_t worldSize);

	//slow, probably don't use
	void BottomUpConstruction(std::vector<AABB> boxes);

	void TopDownConstruction(std::vector<AABB> boxes);
	size_t CreateTopDownSubtree(size_t begin, size_t end);
};

#endif