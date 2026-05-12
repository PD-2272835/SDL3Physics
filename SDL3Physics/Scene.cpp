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


//this should run each frame
void SceneManagement::UpdateEntities(SDL_GPUCommandBuffer* cmd, Scene* scene, double timeDelta)
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


			//account for mass?
			if (currentEntity->hasPhysics)
			{
				if (currentEntity->hasGravity)
				{
					currentEntity->acceleration += mfg::vec3(0, -scene->gravityStrength, 0);
					//std::cout << timeDelta << " " << mfg::VecToString(currentEntity.acceleration) << "\n";
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
			//std::cout << mfg::VecToString(currentEntity->position) << "\n";
		}
	}
}


//Possibly use this to decouple entity updates from entity drawing
void SceneManagement::DrawScene(SDL_GPUCommandBuffer* cmd, Scene* scene)
{
	Application* App = Application::GetInstance();

	SDL_GPUBufferBinding vertexBindings[1];
	vertexBindings[0].buffer = scene->vertexBuffer.ID;
	vertexBindings[0].offset = 0;

	SDL_GPUBufferBinding indexBindings[1];
	indexBindings[0].buffer = scene->indexBuffer.ID;
	indexBindings[0].offset = 0;


	SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
		cmd,
		&App->colorInfo,
		1,
		&App->depthInfo
	);

	//bind pipeline to renderpass
	SDL_BindGPUGraphicsPipeline(renderPass, App->GFXPipeline);

	//bind all the scene's buffers
	SDL_BindGPUVertexStorageBuffers(renderPass, 0, &App->vSSBO.ID, 1); // "slot" corresponds to "binding" in the shader
	SDL_BindGPUVertexBuffers(renderPass, 0, vertexBindings, 1);
	SDL_BindGPUIndexBuffer(renderPass, indexBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);


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


void SceneManagement::DrawEntity(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* renderPass, Scene* scene, const Entity& entity)
{	
	GFXHandle* handle = &AssetManagement::GetInstance()->GetAsset(entity.meshPath.data()).get()->handle;

	if (handle->gfxInitialized)
	{
		//push UniformData 
		mfg::mat4 model = mfg::mat4(1.f);
		mfg::mat4 translate = mfg::Translate(entity.position);
		mfg::mat4 scale = mfg::Scale(entity.scale);

		model = mfg::mul(model, translate);
		model = mfg::mul(model, scale);

		Application* app = Application::GetInstance();
		//update push constants so that this entity is rendered with it's transformation parameters
		UniformBuffer uniformData = { app->mainCamera.GetMatrix(), model, app->Time.delta};

		//Update the uniform data used to render this Entity
		//TODO: Move this to be pulled from a storage buffer that's filled by UpdateEntities()
		SDL_PushGPUVertexUniformData(cmd, 0, &uniformData, sizeof(uniformData));

		//Draw this Entity
		SDL_DrawGPUIndexedPrimitives(renderPass, handle->indexSize, 1, handle->indexOffset, handle->vertexOffset, 0);
	}

}


void SceneManagement::LoadEntityResources(Scene* scene, SDL_GPUCommandBuffer* cmd, const Entity &entity)
{
	if (entity.allocated && entity.renderable)
	{
		std::shared_ptr<Asset> ref = AssetManagement::GetInstance()->GetAsset(entity.meshPath.c_str()); //intermediate step to store 
		
		scene->assetRefs.push_back(ref);

		//we can assume that the returned pointer is a mesh as we are using a path to a 3D model uwu
		//this should be changed if loading an obj or other 3D model returns a different struct
		Mesh* mesh = static_cast<Mesh*>(ref.get());
		GFXHandle handle;
		bool uploadCheck = false;

		//draw call centric - Draws happen multiple times per frame
		//divisions are *slightly* slower than multiplications, so better to store the offset in strides of vertices
		handle.vertexOffset = scene->vertexBuffer.End / sizeof(Vertex); 
		handle.vertexSize = mesh->Vertices.size();
		uploadCheck |= scene->vertexBuffer.UploadData(cmd, (void*)mesh->Vertices.data(), handle.vertexSize * sizeof(Vertex), scene->vertexBuffer.End);


		//draw call centric - Draws happen multiple times per frame
		//divisions are *slightly* slower than multiplications, so better to store the offset in strides of indices
		handle.indexOffset = scene->indexBuffer.End / sizeof(uint32_t);
		handle.indexSize = mesh->Indices.size();
		uploadCheck |= scene->indexBuffer.UploadData(cmd, (void*)mesh->Indices.data(), handle.indexSize * sizeof(uint32_t), scene->indexBuffer.End);

		handle.gfxInitialized = uploadCheck;

		ref.get()->handle = handle;
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
