#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <glad/glad.h>

char *read_file(const char *path, size_t *out_size);
void APIENTRY opengl_debug_callback(GLenum source, GLenum type, unsigned int id,
                                    GLenum severity, GLsizei length,
                                    const char *message, const void *userParam);

#endif
