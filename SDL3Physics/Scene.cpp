#include "Scene.hpp"

Entity* SceneManagement::CreateEntity(Scene &scene)
{
	Entity* newEntity = &scene.zero_entity;
	size_t newIndex = -1;

	for (size_t i = 0; i < scene.maxEntities; ++i)
	{
		if (!scene.entities[i].allocated)
		{
			newEntity = &scene.entities[i];
			newIndex = i;
			break;
		}
	}
	if (newIndex == -1)
	{
		return nullptr; //no available entity was found for allocation/creation return nulltptr
	}

	newEntity->allocated = true;
	newEntity->selfHandle.offset = newIndex;
	newEntity->selfHandle.id = scene.id_generator;
	scene.id_generator++;

	return newEntity; //return newly allocated entity
}


Entity* SceneManagement::EntityFromHandle(Scene &scene, const EntityHandle &handle) {
	if (handle.offset < 0 && handle.offset > scene.maxEntities)
	{
		return &scene.zero_entity; //return placeholder if handle points outside the entities array
	}

	Entity entity = scene.entities[handle.offset];

	if (entity.selfHandle.id == handle.id)
	{
		return &scene.entities[handle.offset]; //return the handle pointed value
	}
	else {
		return &scene.zero_entity; //handle no longer points to the same object, return placeholder
	}
}

//destroys the entity associated with this handle, invalidating the handle
void SceneManagement::DestroyEntity(Scene &scene, const EntityHandle& entityHandle)
{
	Entity* entity = SceneManagement::EntityFromHandle(scene, entityHandle);
	*entity = {};
}

//this should run each frame
void SceneManagement::UpdateEntities(Scene &scene, double timeDelta)
{
	for (size_t i = 0; i < scene.maxEntities; ++i)
	{
		Entity currentEntity = scene.entities[i];
		if (currentEntity.allocated && currentEntity.enabled) //only process allocated/active entities
		{
			//this null check may not work as expected debug mode does some BS
			if (currentEntity.Update != nullptr) currentEntity.Update(); //process the update callback for 

			//do collision stuff here


			//account for mass?
			if (currentEntity.hasPhysics)
			{
				if (currentEntity.hasGravity)
				{
					currentEntity.acceleration += mfg::vec3(0, -scene.gravityStrength/timeDelta, 0);
				}
				currentEntity.velocity += currentEntity.acceleration * timeDelta;
				currentEntity.position += currentEntity.velocity * timeDelta;
				currentEntity.acceleration = 0;
			}

			//DrawEntity();

		}
	}
}

/*
void SceneManagement::LoadEntityResources(Scene& scene, const Entity& entity)
{
	if (entity.allocated && entity.renderable && entity.meshPath != nullptr)
	{
		AssetManagement::GetAsset(entity.meshPath)
		scene.vertexBuffer.UploadData(nullptr, );
	}
}
*/

void SceneManagement::LoadSceneResources(Scene& scene)
{
	//TODO: resource Management

	for (size_t i = 0; i < scene.maxEntities; ++i)
	{
		Entity &currentEntity = scene.entities[i];

		if (currentEntity.allocated && currentEntity.renderable)
		{

		}

	}
}




//TODO: Scene File Serialization/Deserialization
bool SceneManagement::LoadSceneFromFile(Scene& scene)
{
	return false;
}

bool SceneManagement::SaveSceneToFile(Scene& scene)
{
	return false;
}
