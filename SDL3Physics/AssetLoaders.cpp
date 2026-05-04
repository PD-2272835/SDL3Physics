#include "AssetLoaders.hpp"

#include <functions.hpp>
#include <iostream>
 
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
	start += r.length() + 2; //FIXME sometimes /n and /r are used in different combinations based on operating system
	return r;
}

template<size_t dim>
mfg::vec<dim, float> ParseObjVector(std::string line)
{
	mfg::vec<dim, float> r;
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

std::shared_ptr<Mesh> LoadObj(const char* path)
{
	size_t size;
	size_t startOffset = 0;
	void* file = SDL_LoadFile(path, &size);

	std::vector<mfg::vec3> positions;
	std::vector<mfg::vec2> uvs;
	std::vector<mfg::vec3> normals;

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
			switch (line.c_str()[0])
			{
			case 'v':
				switch (line.c_str()[1])
				{
				case ' ':
					//vertex position
					//std::cout << "v " << mfg::VecToString(ParseObjVector<3>(line.substr(2, line.length()))) << "\n";
					positions.push_back(ParseObjVector<3>(line.substr(2, line.length())));
					break;
				case 't':
					//UV
					//std::cout << "vt " << mfg::VecToString(ParseObjVector<2>(line.substr(4, line.length()))) << "\n";
					uvs.push_back(ParseObjVector<2>(line.substr(4, line.length())));
					break;
				case 'n':
					//normal
					//std::cout << "vn " << mfg::VecToString(ParseObjVector<3>(line.substr(3, line.length()))) << "\n";
					normals.push_back(ParseObjVector<3>(line.substr(3, line.length())));
					break;
				default:
					break;
				}
				break;
			case 'f':
				//parse faces
				//std::cout << "f " << ParseObjData(line.substr(3, line.length()), ' ') << '\n';
				//std::cout << "reached f parse\n";
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

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture*> textures;

		//fill data into model
		for (size_t i = 0; i < faces.size(); ++i)
		{
			Internal_FaceIndex face = faces.at(i);
			vertices.emplace_back(positions.at(face.data[0]),
				normals.at(face.data[2]),
				uvs.at(face.data[1]));
			indices.emplace_back(i); //casts i to uint32_t
		}

		std::shared_ptr<Mesh> model(new Mesh(vertices, indices, textures)); //create a dynamically allocated smart ptr to the resulting model
		return model;
	}

	return nullptr;
}


Asset LoadTexture(const char* path)
{
	Asset r;
	return r;
}