#include <glad/glad.h>
#include <stb_image.h>
#include "texture.h"

texture_t texture_load(const char *path, int format) {
  stbi_set_flip_vertically_on_load(true);

  int width, height, channel_count;
  unsigned char *data = stbi_load(path, &width, &height, &channel_count, 0);

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
    puts("failed to load image");
    exit(1);
  }

  return texture;
}

void texture_use(texture_t texture, unsigned int unit) {
  glActiveTexture(unit);
  glBindTexture(GL_TEXTURE_2D, texture);
}
