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
		if (dataStream[i] && ch != '\r') //strtok might have been a better idea
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
mfg::vec<dim, float> ParseObjData(std::string line, char del)
{
	mfg::vec<dim, float> r;
	auto pos = line.find(del); //find the first delimiter
	for (size_t i = 0; i < dim; ++i)
	{
		r[i] = std::stof(line.substr(0, pos)); //convert current number from string to float
		line = line.erase(0, pos+1); //pos + 1 to also remove the delimiter
		pos = line.find(del); //set pos to the position of the next delimiter
	}
	return r;
}


Asset LoadObj(const char* path)
{
	size_t size;
	size_t startOffset = 0;
	void* file = SDL_LoadFile(path, &size);

	if (file)
	{
		Model* model;
		std::string line;
		int linenum = 0;
		while (startOffset < size)
		{
			linenum++;
			std::cout << linenum;
			line = GetLineIter((char*)file, size, startOffset);
			if (line.length() == 0) continue;
			switch (line.c_str()[0])
			{
			case 'v':
				switch (line.c_str()[1])
				{
				case ' ':
					//vertex position
					std::cout << "v " << mfg::VecToString(ParseObjData<3>(line.substr(2, line.length()), ' ')) << "\n";
					break;
				case 't':
					//UV
					std::cout << "vt " << mfg::VecToString(ParseObjData<2>(line.substr(4, line.length()), ' ')) << "\n";
					break;
				case 'n':
					//normal
					std::cout << "vn " << mfg::VecToString(ParseObjData<3>(line.substr(3, line.length()), ' ')) << "\n";
					break;
				default:
					break;
				}
				break;
			case 'f':
				//parse faces
				 
				break;
			default:
				break;
			}
			//startOffset += line.length();
		}
		

		SDL_free(file);
		return *model;
	}
}


Asset LoadTexture(const char* path)
{
	Asset r;
	return r;
}