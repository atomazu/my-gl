#ifndef TEXTURE_H
#define TEXTURE_H

typedef unsigned int texture_t;

texture_t texture_load(const char *path, int format);
void texture_use(unsigned int texture_t, unsigned int unit);

#endif
