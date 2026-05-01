#include "AssetLoaders.hpp"
 
//line parsing, start is modified if a new line, or the end of file is reached
std::string GetLineIter(const char* dataStream, const size_t &size, size_t &start)
{
	char ch;
	std::string r;
	for (size_t i = start; i < size; ++i)
	{
		ch = dataStream[i];
		if (dataStream[i] && ch != ('\n' || '\0')) //strtok might have been a better idea
		{
			r.push_back(ch);
		} 
		else
		{
			start = i;
			break;
		}
	}
	return r;
}

template<size_t dim>
mfg::vec<dim, float> ParseObjCoord(std::string line)
{
	mfg::vec<dim, float> r;
	auto pos = line.find(' ');
	for (size_t i = 0; i < dim; ++i)
	{
		r[i] = std::stof(line.substr(line.begin(), pos));
		line = line.erase(0, pos + 1);
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
		Model model;
		std::string line;
		while (startOffset < size)
		{
			line = GetLineIter((char*)file, size, startOffset);
			if (line.length() == 0) continue;
			switch (line.c_str()[0])
			{
			case 'v':
				switch (line.c_str()[1])
				{
				case ' ':
					//vertex position
					ParseObjCoord<3>(line.substr(3, line.length()));
				case 't':
					//UV
					ParseObjCoord<2>(line.substr(4, line.length()));
				case 'n':
					//normal
				default:
					break;
				}
			case 'f':
				//parse faces
			default:
				break;
			}
		}
		

		SDL_free(file);
		return model;
	}
}


Asset LoadTexture(const char* path)
{

}