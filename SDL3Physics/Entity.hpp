#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_
#include <vec.hpp>
#include <quaternion.hpp>

enum Kind {
	null
};

//do not construct these unless you are absolutely sure
struct EntityHandle //if a reference to an object is stored for a long period of time, use a handle as the referenced object may be destroyed
{
	size_t offset; //offset into the scene's entity array
	uint64_t id;   //unique id
};


//MEGASTRUCT - this object holds data synonymous with all entities in the scene, in an attempt at cache friendliness
//this also means I don't have to worry too much about writing a well optimised ECS, while maintaining some of the advantages of separating code and data
//should be noted that this approach is more procedure oriented and less generic than an ECS
//inspiration for this approach - Randy Prime https://gist.github.com/randyprime/065370cfa73c0dd3cb008eb858e6ba4b
//and "TheStackFrame" - https://www.youtube.com/watch?v=j8v9O3aFk04
typedef struct Entity
{	
	char name[64];		//optional name of this enitity
	
	EntityHandle selfHandle;	//offset in the scene array to this object - "self"  hard limit of 2^32 | 2^64 objects on 32 and 64 bit systems respectively 

	mfg::vec3 position;
	mfg::vec3 rotation; //change this to a quaternion once quaternions are done
	mfg::vec3 scale;

	mfg::vec3 velocity;	//physics related data
	mfg::vec3 acceleration;
	bool hasGravity;
	bool hasPhysics;
	bool renderable;

	
	bool enabled;		//should we skip updating this object?
	bool allocated;		//this should also skip the traversal, used to allocate new objects

	void (*Start)();
	void (*Update)();
	void (*FixedUpdate)();
};



#define MAX_ENTITIES 1024

class Scene
{
	char name[64];
	size_t maxEntities = MAX_ENTITIES - 1; //max index allowable to access the entities array
	uint64_t id_generator = 0;
	Entity* entities = new Entity[MAX_ENTITIES];

public:
	//returns a handle offset to the newly allocated entity
	Entity* CreateEntity()
	{
		Entity* newEntity;
		size_t newIndex = -1;
		
		for (int i = 0; i < maxEntities; ++i)
		{
			if (!entities[i].allocated)
			{
				newEntity = &entities[i];
				newIndex = i;
				break;
			}
		}
		if (newIndex == -1)
		{
			return nullptr; //no available entity was found for allocation/creation return nulltptr
		}

		*newEntity = {};
		newEntity->allocated = true;
		newEntity->selfHandle.offset = newIndex;
		newEntity->selfHandle.id = id_generator;
		id_generator++;

		return newEntity; //return neawly allocated entity
	}

	

	Entity zero_entity = {}; //entity to allow operations to gracefully fail

	Entity* HandleToEntity(const EntityHandle &handle) {
		if (handle.offset < 0 && handle.offset > maxEntities)
		{
			return &zero_entity; //return placeholder if handle points outside the entities array
		}

		Entity entity = entities[handle.offset];

		if (entity.selfHandle.id == handle.id)
		{
			return &entities[handle.offset]; //return the handle pointed value
		} 
		else {
			return &zero_entity; //handle no longer points to the same object, return placeholder
		}
	}


	void DestroyEntity(const EntityHandle &entityHandle)
	{
		Entity* entity = HandleToEntity(entityHandle);
		*entity = {};
	}

	void ProcessStartCallback()
	{
		for (int i = 0; i < maxEntities; ++i)
		{
			if (entities[i].allocated && entities[i].enabled)
			{
				entities[i].Start();
			}
		}
	}

	void ProcessUpdateCallback()
	{
		for (int i = 0; i < maxEntities; ++i)
		{
			if (entities[i].allocated && entities[i].enabled)
			{
				entities[i].Update();
			}
		}
	}

	void ProcessFixedUpdateCallback()
	{
		for (int i = 0; i < maxEntities; ++i)
		{
			entities[i].FixedUpdate();
		}
	}
};

#endif