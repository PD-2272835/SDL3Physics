#include "AssetLoaders.hpp"
 
//line parsing, start is modified if a new line, or the end of file is reached
std::string GetLineIter(const char* dataStream, const size_t &size, size_t &start)
{
	char ch;
	std::string r;
	for (size_t i = start; i < size; ++i)
	{
		ch = dataStream[i];
		if (dataStream[i] && ch != ('\n' || '\0'))
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


Asset LoadObj(const char* path)
{
	size_t size;
	size_t startOffset;
	void* file = SDL_LoadFile(path, &size);

	if (file)
	{
		Model model;
		std::string line;
		while (startOffset < size)
		{
			line = GetLineIter((char*)file, size, startOffset);
			switch (line.c_str()[0])
			{
			case 'v':
				switch (line.c_str()[1])
				{
				case 't':
					//UV
				case 'n':
					//normal
				default:
					break;
				}
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