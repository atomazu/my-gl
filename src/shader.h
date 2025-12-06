#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <cglm/cglm.h>

typedef unsigned int shader_t;

shader_t shader_create(const char *vertex_path, const char *fragment_path);
void shader_destroy(shader_t shader);
void shader_use(shader_t shader);
void shader_set_1i(shader_t shader, const char *name, int value);
void shader_set_1f(shader_t shader, const char *name, float value);
void shader_set_mat4fv(shader_t shader, const char *name, mat4 value);

#endif
