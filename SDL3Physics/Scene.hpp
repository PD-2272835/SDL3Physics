#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "Entity.hpp"

#define MAX_ENTITIES 1024

struct Scene
{
	char name[64];
	uint64_t id_generator = 0; //used to give EntityHandles a unique signature
	Entity* entities = new Entity[MAX_ENTITIES]{}; //array of entities of size defined by maxEntities
	size_t maxEntities = MAX_ENTITIES;
	Entity zero_entity = {}; //entity to allow operations to gracefully fail
	float gravityStrength = 9.8;
};


//Procedurally programmed stuffs to operate over a Scene's data
namespace SceneManagement
{
	Entity* CreateEntity(Scene& scene);
	void DestroyEntity(Scene& scene, const EntityHandle& entityHandle);

	Entity* EntityFromHandle(Scene &scene, const EntityHandle& handle);
	void UpdateEntities(Scene& scene, double timeDelta);
	

	void LoadSceneResources(Scene &scene);

	//FIXME: better to return a dynamically allocated scene instance instead of operating on an existing scene
	bool LoadSceneFromFile(Scene &scene);
	bool SaveSceneToFile(Scene &scene);

}


#endif