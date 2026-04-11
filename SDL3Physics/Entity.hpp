#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_
#define MAX_ENTITIES 100
#include <vec.hpp>
#include <quaternion.hpp>



//MEGASTRUCT - this object holds data synonymous with all entities in the scene, in an attempt at cache friendliness
//this also means I don't have to worry too much about writing a well optimised ECS, while maintaining some of the advantages of separating code and data
//should be noted that this approach is more procedure oriented and less generic than ECS
//inspiration for this approach - Randy Prime https://gist.github.com/randyprime/065370cfa73c0dd3cb008eb858e6ba4b
//and "TheStackFrame" - https://www.youtube.com/watch?v=j8v9O3aFk04
typedef struct Entity
{	
	char name[64];		//optional name of this enitity
	
	size_t selfHandle;	//offset in the scene array to this object - "self"  hard limit of 2^32 | 2^64 objects on 32 and 64 bit systems respectively 

	mfg::vec3 position;
	mfg::vec3 rotation; //change this to a quaternion once quaternions are done
	mfg::vec3 scale;

	bool hasGravity;
	bool enabled;		//should we skip updating this object?
	bool initialized;	//this should also skip the traversal, used to allocate new objects

	void (*Start)();
	void (*Update)();
	void (*FixedUpdate)();
};


class Scene
{
	Entity entities[MAX_ENTITIES];




	void StartCallbacka()
	{
		for (int i = 0; i < MAX_ENTITIES; ++i)
		{
			entities[i].Start();
		}
	}

	void UpdateCallback()
	{
		for (int i = 0; i < MAX_ENTITIES; ++i)
		{
			entities[i].Update();
		}
	}

	void FixedUpdateCallback()
	{
		for (int i = 0; i < MAX_ENTITIES; ++i)
		{
			entities[i].FixedUpdate();
		}
	}
};

#endif