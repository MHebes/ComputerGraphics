#include "Texture.h"
#include "PpmLoader.h"

void Texture::LoadTexture(const std::string filename) {
    PpmLoader loader(filename);

}