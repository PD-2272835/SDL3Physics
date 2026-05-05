#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "SDL3/SDL_gpu.h"
#include "Entity.hpp"
#include "Buffer.hpp"
#include "AssetManagement.hpp"


#define MAX_ENTITIES 1024

//global scene state
struct Scene
{
	char name[64];
	uint64_t id_generator = 0; //used to give EntityHandles a unique signature
	Entity* entities = new Entity[MAX_ENTITIES]{}; //array of entities of size defined by maxEntities
	size_t maxEntities = MAX_ENTITIES;
	Entity zero_entity = {}; //entity to allow operations to gracefully fail
	float gravityStrength = 9.8;

	//track buffers in scene struct
	Buffer vertexBuffer;
	Buffer indexBuffer;
};


//Procedurally programmed stuffs to operate over a Scene's data
namespace SceneManagement
{
	Entity* CreateEntity(Scene &scene);
	void DestroyEntity(Scene &scene, const EntityHandle &entityHandle);

	Entity* EntityFromHandle(Scene &scene, const EntityHandle &handle); //Get a ptr to an entity from an Entity Handle
	void UpdateEntities(Scene &scene, double timeDelta); //Update all entities in provided scene
	
	//void InitializeScene(Scene &scene);
	void DrawEnitity(const Entity &entity);

	void LoadEntityResources(Scene &scene, const Entity &entity);
	void LoadSceneResources(Scene &scene); //Load all required assets from a scene into assetManagement

	//FIXME: better to return a dynamically allocated scene instance instead of operating on an existing scene
	bool LoadSceneFromFile(Scene &scene);
	bool SaveSceneToFile(Scene &scene);

}


#endif