#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>

unsigned int loadTexture(const char* path, const std::string& directory, bool gamma = 0);
unsigned int loadCubemap(const char* path);

#endif