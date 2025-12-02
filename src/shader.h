#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <cglm/cglm.h>

typedef struct Shader Shader;

Shader *shader_create(const char *vertex_path, const char *fragment_path);
void shader_destroy(Shader *shader);
void shader_use(Shader *shader);
void shader_set_1i(Shader *shader, const char *name, int value);
void shader_set_1f(Shader *shader, const char *name, float value);
void shader_set_mat4fv(Shader *shader, const char *name, mat4 value);

#endif
