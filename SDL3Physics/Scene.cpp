#include "Scene.hpp"


Scene* SceneManagement::CreateScene(SDL_GPUDevice* device)
{
	Scene* s = new Scene;

	s->name = "New Scene";
	s->id_generator = 0;
	s->entities = new Entity[MAX_ENTITIES];
	s->maxEntities = MAX_ENTITIES;
	s->vertexBuffer = Buffer(device, SDL_GPU_BUFFERUSAGE_VERTEX, UINT16_MAX * sizeof(Vertex));
	s->indexBuffer = Buffer(device, SDL_GPU_BUFFERUSAGE_INDEX, UINT16_MAX * sizeof(uint32_t));

	return s;
}

void SceneManagement::DeleteScene(Scene* scene)
{
	delete[] scene->entities;
	scene->vertexBuffer.Delete();
	scene->indexBuffer.Delete();
	delete scene;
}



Entity* SceneManagement::CreateEntity(Scene* scene)
{
	Entity* newEntity = &scene->zero_entity;
	size_t newIndex = -1;

	for (size_t i = 0; i < scene->maxEntities; ++i)
	{
		if (!scene->entities[i].allocated)
		{
			newEntity = &scene->entities[i];
			newIndex = i;
			break;
		}
	}
	if (newIndex == -1)
	{
		return nullptr; //no available entity was found for allocation/creation return nulltptr
	}

	newEntity->allocated = true;
	newEntity->enabled = true;
	newEntity->selfHandle.offset = newIndex;
	newEntity->selfHandle.id = scene->id_generator;
	newEntity->name = "Entity" + std::to_string(scene->id_generator);
	scene->id_generator++;

	return newEntity; //return newly allocated entity
}


Entity* SceneManagement::EntityFromHandle(Scene* scene, const EntityHandle &handle) {
	if (handle.offset < 0 || handle.offset > scene->maxEntities)
	{
		return &scene->zero_entity; //return placeholder if handle points outside the entities array
	}

	Entity entity = scene->entities[handle.offset];

	if (entity.selfHandle.id == handle.id)
	{
		return &scene->entities[handle.offset]; //return the handle pointed value
	}
	else {
		return &scene->zero_entity; //handle no longer points to the same object, return placeholder
	}
}

//destroys the entity associated with this handle, invalidating the handle
void SceneManagement::DestroyEntity(Scene* scene, const EntityHandle& entityHandle)
{
	Entity* entity = SceneManagement::EntityFromHandle(scene, entityHandle);
	*entity = Entity(); //set this Enitity to default state (unallocated and reset)
}


std::vector<Entity> SceneManagement::GetCollisionEntities(Scene* scene)
{
	std::vector<Entity> res;
	for (size_t i = 0; i < scene->maxEntities; ++i)
	{
		Entity* currentEntity = &scene->entities[i];
		if (currentEntity->allocated && currentEntity->enabled && currentEntity->hasCollision)
		{
			res.push_back(*currentEntity);
		}
	}

	return res;
}



//this should run each frame
void SceneManagement::UpdateEntities(SDL_GPUCommandBuffer* cmd, Scene* scene, Bvh& collisionTree, double timeDelta)
{
	//iterate through all entities
	for (size_t i = 0; i < scene->maxEntities; ++i)
	{
		Entity* currentEntity = &scene->entities[i];
		if (currentEntity->allocated && currentEntity->enabled) //only process allocated/active entities
		{
			//this null check may not work as expected debug mode does some BS
			if (currentEntity->Update != nullptr) currentEntity->Update(currentEntity); //process the update callback for 

			//do collision stuff here
			if (currentEntity->hasCollision)
			{
				std::vector<EntityHandle> collisions = collisionTree.CheckCollision(currentEntity);
				if (!collisions.empty())
				{
					std::cout << currentEntity->name << " collided with ";
					for (auto i : collisions)
					{
						std::cout << EntityFromHandle(scene, i)->name << ", ";
					}
					std::cout << "\n";
				}
			}

			//account for mass?
			if (currentEntity->hasPhysics)
			{
				if (currentEntity->hasGravity)
				{
					currentEntity->acceleration += sgm::vec3(0, -scene->gravityStrength, 0);
					//std::cout << timeDelta << " " << sgm::VecToString(currentEntity.acceleration) << "\n";
				}
				currentEntity->velocity += currentEntity->acceleration * timeDelta;
				currentEntity->position += currentEntity->velocity * timeDelta;
				currentEntity->acceleration = 0;
			}

			//draw this entity
			if (currentEntity->renderable)
			{
				//realistically rendering should happen within this loop, 
				//as this would prevent looping over every object in the scene twice
			}
		}
	}
}


//Possibly use this to decouple entity updates from entity drawing
void SceneManagement::DrawScene(SDL_GPUCommandBuffer* cmd, Scene* scene)
{
	Application* App = Application::GetInstance();

	SDL_GPUBufferBinding vertexBindings[1];
	vertexBindings[0].buffer = scene->vertexBuffer.Handle;
	vertexBindings[0].offset = 0;

	SDL_GPUBufferBinding indexBindings[1];
	indexBindings[0].buffer = scene->indexBuffer.Handle;
	indexBindings[0].offset = 0;

	SDL_GPUTextureSamplerBinding textureBindings[1];
	textureBindings->texture = AssetManagement::GetInstance()->GetAsset("C:\\Users\\eater\\Desktop\\KenneyCarsOBJ\\Textures\\colormap.png").Handle


	SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
		cmd,
		&App->colorInfo,
		1,
		&App->depthInfo
	);

	//bind pipeline to renderpass
	SDL_BindGPUGraphicsPipeline(renderPass, App->GFXPipeline);

	//bind all the scene's buffers
	SDL_BindGPUVertexStorageBuffers(renderPass, 0, &App->vSSBO.Handle, 1); // "slot" corresponds to "binding" in the shader
	SDL_BindGPUVertexBuffers(renderPass, 0, vertexBindings, 1);
	SDL_BindGPUIndexBuffer(renderPass, indexBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);
	SDL_BindGPUFragmentSamplers(renderPass, 0, )


	for (size_t i = 0; i < scene->maxEntities; ++i)
	{
		Entity entity = scene->entities[i];
		if (entity.renderable && entity.allocated && entity.enabled)
		{
			SceneManagement::DrawEntity(cmd, renderPass, scene, entity);
		}
	}

	SDL_EndGPURenderPass(renderPass);
}


void SceneManagement::DrawEntity(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* renderPass, Scene* scene, Entity& entity)
{	
	GFXHandle* handle = &AssetManagement::GetInstance()->GetAsset(entity.meshPath.data()).get()->Handle;

	if (handle->isGfxInitialized)
	{
		//push UniformData 
		sgm::mat4 model = sgm::mat4(1.f);
		sgm::mat4 translate = sgm::Translate(entity.position);
		sgm::mat4 rotate = entity.rotation.ToMat();
		sgm::mat4 scale = sgm::Scale(entity.scale);

		model = sgm::mul(model, translate);
		model = sgm::mul(model, rotate);
		model = sgm::mul(model, scale);

		Application* app = Application::GetInstance();
		//update push constants so that this entity is rendered with it's transformation parameters
		UniformBuffer uniformData = { sgm::mul(app->mainCamera.GetMatrix(), model), model, app->Time.delta};

		//Update the uniform data used to render this Entity
		//TODO: Move this to be pulled from a storage buffer that's filled by UpdateEntities()
		SDL_PushGPUVertexUniformData(cmd, 0, &uniformData, sizeof(uniformData));

		//Draw this Entity
		SDL_DrawGPUIndexedPrimitives(renderPass, handle->indexBuffer.size, 1, handle->indexBuffer.offset, handle->vertexBuffer.offset, 0);
	}

}


void SceneManagement::LoadEntityResources(Scene* scene, SDL_GPUCommandBuffer* cmd, const Entity &entity)
{
	if (entity.allocated && entity.renderable)
	{
		std::shared_ptr<Asset> ref = AssetManagement::GetInstance()->GetAsset(entity.meshPath.c_str()); 
		
		if (ref == nullptr) //guard against invalid file path
		{
			std::cout << "invalid filepath\n";
			return;
		}

		scene->assetRefs.push_back(ref); //FIXME, this should only happen for asset bundles

		ref.get()->UploadToGPU(cmd, nullptr, &scene->vertexBuffer, &scene->indexBuffer);
	}
}


void SceneManagement::LoadSceneResources(Scene* scene, SDL_GPUCommandBuffer* cmd)
{
	//TODO: resource Management
	for (size_t i = 0; i < scene->maxEntities; ++i)
	{
		Entity currentEntity = scene->entities[i];

		SceneManagement::LoadEntityResources(scene, cmd, currentEntity);
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
