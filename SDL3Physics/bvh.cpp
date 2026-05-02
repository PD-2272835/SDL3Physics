#include "bvh.hpp"

bvh::bvh()
{
	rootNodeIndex = 0;
	worldSize = -1;
}

bvh::bvh(const std::vector<AABB>& boxes, const size_t worldSize = -1)
{
	if (worldSize > -1)
	{
		TopDownConstruction(boxes);
	}
	else
	{
		BottomUpConstruction(boxes);
	}
}

void bvh::BottomUpConstruction(std::vector<AABB> boxes)
{
	for (size_t i = 0; i < boxes.size(); ++i)
	{
		if (nodes.size() < boxes.size())
		{
			nodes.emplace_back(boxes[i], i, 0, 0, 0);
		}
	}
	
	//construct tree
	std::vector<BVHNode> working = nodes;
	std::vector<BVHNode> temp;
	
	//this is ruthlessly innefficient, top down might just be the way to go
	//for now, just perform bottom up full construction a little as possible
	while (working.size()/2 > 1) //while there are pairs to find
	{
		//group AABB pairs with naive approach
		for (size_t i = 0; i < working.size()/2 ; ++i) //we're working with pairs
		{
			//this could be paralellized
			BVHNode closestNode = working[i]; //no default constructor, use current object so we can tell if no other closest object was found
			float lowestDist = FLT_MAX; //max float value
			int offset = 0;
			for (size_t j = 0; j < working.size(); ++j)
			{
				float distanceBetween = mfg::SqrDistance(working[i].box.center, working[j].box.center);
				if (&working[i] != &working[j] && lowestDist > distanceBetween)
				{
					lowestDist = distanceBetween;
					closestNode = working[j];
				}
			}


			//comparing references like this could be UB?
			if (&closestNode != &working[i])
			{
				//set the parent to be the offset of this node into the nodes array
				size_t nodesIndex = nodes.size() + i;

				working[i].parentOffset = nodesIndex;
				closestNode.parentOffset = nodesIndex;
				
				temp.erase(temp.begin() + i);
				temp.erase(temp.begin() + closestNode.object);

				temp.emplace_back(
					Union(working[i].box, closestNode.box),	//Bounding AABB of this object
					nodesIndex,								//object's offset into prospective nodes array
					0,										//offset of parent (unknown)
					working[i].object,						//offset of left child into the nodes array
					closestNode.object);					//offset of right child into the nodes array
			}
		}
		nodes.insert(nodes.end(), temp.begin(), temp.end()); //add the union pairs of nodes to the tree
		working.insert(working.end(), temp.begin(), temp.end()); //if there's only one element left, leav
		temp = std::vector<BVHNode>(); //replace temp with new vector

	}
}

//the following Top Down BVH construction is heavily adapted from Matthias at 10-minute physics
//https://github.com/matthias-research/pages/blob/master/tenMinutePhysics/24-morton.html#L80

//expand a given coordinate so it can be used to interleave bits by "inserting" two zeros after
//https://fgiesen.wordpress.com/2022/09/09/morton-codes-addendum/
uint64_t BitExpansion(uint64_t x)
{
	x &= 0x1fffff;
	x = (x | x << 32) & 0x1f00000000ffff;
	x = (x | x << 16) & 0x1f0000ff0000ff;
	x = (x | x << 8) & 0x100f00f00f00f00f;
	x = (x | x << 4) & 0x10c30c30c30c30c3;
	x = (x | x << 2) & 0x1249249249249249;
	return x;
}



uint64_t Create3DMorton(float x, float y, float z, const uint32_t worldSize)
{
	//normalize these coords to be in range 0-1
	x = (x + worldSize / 2) / worldSize;
	y = (y + worldSize / 2) / worldSize;
	z = (z + worldSize / 2) / worldSize;

	//ensure this is the case - not needed?
	x = mfg::Clamp(x);
	y = mfg::Clamp(y);
	z = mfg::Clamp(z);

	//create coordinates as 21 bit integer representations of each element
	const int max = 2 ^ 21; //max representable value by each coordinate in a morton code
	auto xi = mfg::Min(static_cast<int>(std::floor(x * max)), max);
	auto yi = mfg::Min(static_cast<int>(std::floor(y * max)), max);
	auto zi = mfg::Min(static_cast<int>(std::floor(z * max)), max);

	//expand the bitpattern of each element and pack them into a morton code
	uint64_t morton = BitExpansion(xi) | (BitExpansion(yi) << 1) | (BitExpansion(zi) << 2);

	return morton;
}


size_t bvh::CreateTopDownSubtree(size_t begin, size_t end)
{
	if (begin == end)
	{
		return begin; //return the index of this leaf
	}
	else {
		size_t m = std::floor((begin + end) / 2); //find the centerpoint of the array where nodes[begin] is the start and nodes[end] is the end
		auto left = CreateTopDownSubtree(begin, m - 1);
		auto right = CreateTopDownSubtree(m, end);
		nodes.emplace_back(
			Union(nodes[left].box, nodes[right].box),
			0,
			0, //parent might need to be set correctly
			left,
			right
		);
	}
}

void bvh::TopDownConstruction(std::vector<AABB> boxes)
{
	for (size_t i = 0; i < boxes.size(); ++i)
	{
		nodes.emplace_back(
			boxes[i],
			Create3DMorton(
				boxes[i].center.x(),
				boxes[i].center.y(),
				boxes[i].center.z(),
				worldSize),
			0, 0, 0);
	}
	
	//sort the nodes array by morton code (better spacial locality/cache friendliness)
	std::sort(nodes.begin(), nodes.end(), 
		[](const BVHNode& a, const BVHNode& b) 
		{return a.object < b.object; });

	rootNodeIndex = CreateTopDownSubtree(0, nodes.size()); //recursively generate the tree structure and get the index of the root node
}