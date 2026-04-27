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
		if (nodes.size() < boxes.size())
		{
			nodes.emplace_back(boxes[i], i, 0, 0, 0, true);
		}
	}
	
	//construct tree
	std::vector<Node> working = nodes;
	std::vector<Node> temp;
	
	//this is ruthlessly innefficient, top down might just be the way to go
	//for now, just perform bottom up full construction a little as possible
	while (working.size()/2 > 1) //while there are pairs to find
	{
		//group AABB pairs with naive approach
		for (size_t i = 0; i < working.size()/2 ; ++i) //we're finding pairs 
		{
			//this could be paralellized
			Node closestNode = working[i]; //no default constructor, use current object so we can tell if no other closest object was found
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

				temp.emplace_back(
					Union(working[i].box, closestNode.box),	//Bounding AABB of this object
					nodesIndex,								//object's offset into prospective nodes array
					0,										//offset of parent (unknown)
					working[i].objectOffset,				//offset of left child into the nodes array
					closestNode.objectOffset,				//offset of right child into the nodes array
					false);
			}
		}
		nodes.insert(nodes.end(), temp.begin(), temp.end()); //add the union pairs of nodes to the tree
		working.insert(working.end(), temp.begin(), temp.end()); //if there's only one element left, leav
		temp = std::vector<Node>(); //replace temp with new vector

	}
	
}