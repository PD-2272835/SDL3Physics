#include "bvh.hpp"

bvh::bvh()
{
	rootNodeIndex = 0;
}

bvh::bvh(std::vector<AABB> boxes)
{
	BottomUpConstruction(boxes);
}

void bvh::BottomUpConstruction(std::vector<AABB> boxes)
{
	for (size_t i = 0; i < boxes.size(); ++i)
	{
		nodes.emplace_back(boxes[i], i, 0, 0, 0, true); //construct in place within the nodes array
	}
}