#ifndef _BVH_HPP_
#define _BVH_HPP_
#include <vec.hpp> //mfg
#include <functions.hpp>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Intersections.hpp"
#include "Entity.hpp"
#include "AssetManagement.hpp"


// Adapted from Box2D creator Erin Catto's slides at GDC 2019
// https://box2d.org/files/ErinCatto_DynamicBVH_GDC2019.pdf

struct BVHNode
{
	AABB box;
	uint64_t object; // this value is an offset for bottom up or a morton code for top down
	uint32_t parentOffset;
	uint32_t left;
	uint32_t right;
	Entity* entity;


	BVHNode(AABB newBox, uint64_t objOffset, uint32_t parOffset, uint32_t leftOffset, uint32_t rightOffset, Entity* entityPtr) :
			box(newBox), object(objOffset), parentOffset(parOffset), left(leftOffset), right(rightOffset)
	{
		entity = entityPtr;
	};
	BVHNode(AABB newBox, uint64_t objOffset, uint32_t parOffset, uint32_t leftOffset, uint32_t rightOffset) :
			box(newBox), object(objOffset), parentOffset(parOffset), left(leftOffset), right(rightOffset)
	{
		entity = nullptr;
	};

	bool isLeaf() const { return left <= 0 && right <= 0; } //this node is a leaf if left and right are both not set
};

uint64_t BitExpansion(uint64_t x);
uint64_t Create3DMorton(float x, float y, float z, const uint32_t worldSize);


class Bvh
{
public:

	Bvh();
	Bvh(std::vector<Entity>* entities, size_t worldSize);

	std::vector<BVHNode> nodes;
	size_t rootNodeIndex; //index into the nodes array that contains all physics objects
	size_t worldSize;

	//slow, probably don't use
	void BottomUpConstruction(const std::vector<AABB>* boxes);

	void TopDownConstruction(std::vector<Entity>* entities);
	size_t CreateTopDownSubtree(size_t begin, size_t end);

	bool FindCollision(AABB box, Entity* entity, BVHNode node, std::vector<EntityHandle>* collisionInfo);
	std::vector<EntityHandle> CheckCollision(Entity* entity);
};

#endif