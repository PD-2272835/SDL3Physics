#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_
#include <vec.hpp>
#include <quaternion.hpp>

enum Kind {
	null
};

//do not construct these unless you are absolutely sure
typedef struct EntityHandle //if a reference to an object is stored for a long period of time, use a handle as the referenced object may be destroyed
{
	size_t offset; //offset into the scene's entity array
	uint64_t id;   //unique id
};


//MEGASTRUCT - this object holds data synonymous with all entities in the scene, in an attempt at cache friendliness
//this also means I don't have to worry too much about writing a well optimised ECS, while maintaining some of the advantages of separating code and data
//should be noted that this approach is more procedure oriented and less generic than an ECS
//inspiration for this approach:
// -Randy Prime https://gist.github.com/randyprime/065370cfa73c0dd3cb008eb858e6ba4b
// -TheStackFrame - https://www.youtube.com/watch?v=j8v9O3aFk04
typedef struct Entity
{	
	char name[32];		//optional name of this enitity
	
	EntityHandle selfHandle{};	//offset in the scene array to this object - "self"  hard limit of 2^32 | 2^64 objects on 32 and 64 bit systems respectively 

	mfg::vec3 position{};
	mfg::vec3 rotation{}; //change this to a quaternion once quaternions are done
	mfg::vec3 scale{};

	mfg::vec3 velocity{};	//physics related data
	mfg::vec3 acceleration{};
	bool hasGravity = false;
	bool hasPhysics = false;
	bool hasCollision = false;
	bool renderable = false;

	
	bool enabled = false;		//should we skip updating this object?
	bool allocated = false;		//this should also skip the traversal, used to allocate new objects

	void (*Start)() = nullptr;
	void (*Update)() = nullptr;
};


#endif