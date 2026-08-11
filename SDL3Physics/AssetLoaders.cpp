#include "AssetLoaders.hpp"
 
//line parsing, start is modified if a new line, or the end of file is reached
std::string GetLineIter(const char* dataStream, const size_t &size, size_t &start)
{
	char ch;
	std::string r;
	for (size_t i = start; i < size; ++i)
	{
		ch = dataStream[i];
		if (dataStream[i] && ch != '\r')
		{
			r.push_back(ch);
		}
		else {
			break;
		}
	}
	start += r.length() + 2; //FIXME sometimes /n and /r are used in different combinations based on operating system/editor that saved the file
	return r;
}

template<size_t dim>
sgm::vec<dim, float> ParseObjVector(std::string line)
{
	sgm::vec<dim, float> r;
	auto pos = line.find(' '); //find the first delimiter
	for (size_t i = 0; i < dim; ++i)
	{
		r[i] = std::stof(line.substr(0, pos)); //convert current number from string to float
		line = line.erase(0, pos+1); //pos + 1 to also remove the delimiter
		pos = line.find(' '); //set pos to the position of the next delimiter
	}
	return r;
}

Internal_FaceIndex ParseFaceIndex(std::string line)
{
	Internal_FaceIndex res;
	size_t i = 0;
	auto pos = line.find('/');
	while (pos != std::string::npos)
	{
		res.data[i] = std::stoi(line.substr(0, pos)) - 1;
		line.erase(0, pos + 1);
		pos = line.find('/');
		++i;
	}
	res.data[i] = std::stoi(line.substr(0, pos)) - 1; //FIXME gotta be a better way of ensuring last element gets processed
	return res;
}


std::vector<Internal_FaceIndex> ParseObjFace(std::string line)
{
	std::vector<Internal_FaceIndex> res;
	auto pos = line.find(' ');
	while (pos != std::string::npos)
	{
		res.push_back(ParseFaceIndex(line.substr(0, pos)));
		line.erase(0, pos + 1);
		pos = line.find(' ');
	}
	res.push_back(ParseFaceIndex(line.substr(0, pos))); //FIXME gotta be a better way of ensuring last element gets processed
	return res;
}



bool Texture::LoadFromDisk(const char* filepath)
{
	return false;
}

bool Texture::UploadToGPU(SDL_GPUCommandBuffer* cmdBuffer, const Buffer* textureBuffer, Buffer* vertexBuffer, Buffer* indexBuffer)
{
	return false;
}


bool Mesh::LoadFromDisk(const char* filepath)
{
	size_t size;
	size_t startOffset = 0;
	void* file = SDL_LoadFile(filepath, &size);

	std::vector<sgm::vec3> positions;
	std::vector<sgm::vec2> uvs;
	std::vector<sgm::vec3> normals;
	AABB bounds;

	if (file)
	{
		std::string line;
		int linenum = 0;
		std::vector<Internal_FaceIndex> faces;
		while (startOffset < size)
		{
			linenum++;
			//std::cout << linenum;
			line = GetLineIter((char*)file, size, startOffset);
			if (line.length() == 0) continue;
			//std::cout << " " << line << " ";
			std::vector<Internal_FaceIndex> face;
			sgm::vec3 pos;
			switch (line.c_str()[0])
			{
			case 'v':
				switch (line.c_str()[1])
				{
				case ' ':
					//vertex position
					pos = ParseObjVector<3>(line.substr(2, line.length()));
					positions.push_back(pos);
					bounds.lowerBound = sgm::Min(bounds.lowerBound, pos);
					bounds.upperBound = sgm::Max(bounds.lowerBound, pos);
					break;
				case 't':
					//UV
					uvs.push_back(ParseObjVector<2>(line.substr(4, line.length())));
					break;
				case 'n':
					//normal
					normals.push_back(ParseObjVector<3>(line.substr(3, line.length())));
					break;
				default:
					break;
				}
				break;
			case 'f':
				//parse faces
				face = ParseObjFace(line.substr(2, line.length()));
				faces.emplace_back(face[0]);
				faces.emplace_back(face[1]);
				faces.emplace_back(face[2]);
				break;
			default:
				break;
			}
		}

		SDL_free(file);

		//fill data into model
		for (size_t i = 0; i < faces.size(); ++i)
		{
			Internal_FaceIndex face = faces.at(i);
			Vertices.emplace_back(positions.at(face.data[0]),
				normals.at(face.data[2]),
				uvs.at(face.data[1]));
			Indices.emplace_back(static_cast<uint32_t>(i)); //casts i to uint32_t
		}

		//std::shared_ptr<Mesh> model(this); //create a smart ptr to the resulting model
		//return shared_from_this();
		return true;
	}

	return false;
	//return nullptr;
}

bool Mesh::UploadToGPU(SDL_GPUCommandBuffer* cmdBuffer, const Buffer* tBuffer, Buffer* vBuffer, Buffer* iBuffer)
{
	if (Handle.isGfxInitialized) return false; //do not upload duplicate models, especially if they are still in scope
	if (vBuffer == nullptr || iBuffer == nullptr) return false; //guard against no buffer provided
	
	bool uploadCheck = true;

	if (uploadCheck)
	{
		Handle.vertexBuffer = {
			vBuffer,
			this->Vertices.size(),
			vBuffer->End / sizeof(Vertex)
		};
		uploadCheck = vBuffer->UploadData(cmdBuffer, (void*)this->Vertices.data(), Handle.vertexBuffer.size * sizeof(Vertex), vBuffer->End);
	}

	if (uploadCheck)
	{
		Handle.indexBuffer = {
			iBuffer,
			this->Indices.size(),
			iBuffer->End / sizeof(uint32_t)
		};
		uploadCheck = iBuffer->UploadData(cmdBuffer, (void*)this->Indices.data(), Handle.vertexBuffer.size * sizeof(uint32_t), iBuffer->End);
	}

	if (tBuffer != nullptr)
	{
		for (size_t i = 0; i < this->Textures.size(); i++)
		{
			//FIXME: what if the texture buffer runs out of space after uploading one or more texture?
			if(uploadCheck) uploadCheck = Textures.at(i)->UploadToGPU(cmdBuffer, tBuffer);
		}
	}

	Handle.isGfxInitialized = uploadCheck;
	return uploadCheck;

	//add the buffers and other data to the buffer handles array
	
	//do the uploading

	//draw call centric - Draws happen multiple times per frame
	//divisions are *slightly* slower than multiplications, so better to store the offset in strides of vertices
	//uploadCheck |= handle.buffer->UploadData(cmd, this->Vertices.data(), handle.size * sizeof(Vertex), handle.buffer->End);


	//draw call centric - Draws happen multiple times per frame
	//divisions are *slightly* slower than multiplications, so better to store the offset in strides of indices
	//uploadCheck |= handle.buffer->UploadData(cmd, this->Indices.data(), this->Indices.size() * sizeof(uint32_t), handle.buffers.at(1).buffer->End);

}