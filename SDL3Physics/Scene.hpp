#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "SDL3/SDL_gpu.h"
#include <matrix.hpp>
//#include <functions.hpp>
#include "Entity.hpp"
#include "Buffer.hpp"
#include "AssetManagement.hpp" //includes std::vector
#include "Application.hpp"
#include "bvh.hpp"
#define MAX_ENTITIES 1024


//Idk where else to put this
struct UniformBuffer
{
	mfg::mat4 View;
	mfg::mat4 Model;
	float time;
};

//global scene state
typedef struct Scene
{
	std::string name;
	uint64_t id_generator = 0; //used to give EntityHandles a unique signature
	Entity* entities = new Entity[MAX_ENTITIES]{}; //array of entities of size defined by maxEntities
	size_t maxEntities = MAX_ENTITIES;
	Entity zero_entity = {}; //entity to allow operations to gracefully fail
	float gravityStrength = 9.8f;
	//track buffers in scene struct
	Buffer vertexBuffer;
	Buffer indexBuffer;

	std::vector<std::shared_ptr<Asset>> assetRefs; //manage the lifetime of objects and don't unload them until this scene is destroyed

};


//Procedurally programmed stuffs to operate over a Scene's data
namespace SceneManagement
{
	Scene* CreateScene(SDL_GPUDevice* device);
	void DeleteScene(Scene* scene);

	Entity* CreateEntity(Scene* scene);
	void DestroyEntity(Scene* scene, const EntityHandle &entityHandle);

	std::vector<Entity> GetCollisionEntities(Scene* scene);

	Entity* EntityFromHandle(Scene* scene, const EntityHandle &handle); //Get a ptr to an entity from an Entity Handle
	void UpdateEntities(SDL_GPUCommandBuffer* cmd, Scene* scene, Bvh& collisionTree, double timeDelta); //Update all entities in provided scene
	
	void DrawEntity(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* renderPass, Scene* scene, Entity& entity);
	void DrawScene(SDL_GPUCommandBuffer* cmd, Scene* scene);
	
	void LoadEntityResources(Scene* scene, SDL_GPUCommandBuffer* cmd, const Entity &entity);
	void LoadSceneResources(Scene* scene, SDL_GPUCommandBuffer* cmd); //Load all required assets from a scene into assetManagement

	//FIXME: better to return a dynamically allocated scene instance instead of operating on an existing scene
	bool LoadSceneFromFile(Scene &scene);
	bool SaveSceneToFile(Scene &scene);

}


#endif