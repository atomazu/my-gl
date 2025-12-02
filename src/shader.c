#include "shader.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

struct Shader {
  unsigned int id;
};

Shader *shader_create(const char *vertex_path, const char *fragment_path) {
  Shader *shader = malloc(sizeof(Shader));

  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  char *vs_src = read_file(vertex_path, nullptr);
  const char *vs_src_ptr = vs_src;
  glShaderSource(vertex_shader, 1, &vs_src_ptr, NULL);
  glCompileShader(vertex_shader);

  int success;
  char info_log[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    puts(info_log);
    exit(1);
  }
  free(vs_src);

  char *fs_src = read_file(fragment_path, nullptr);
  const char *fs_src_ptr = fs_src;
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fs_src_ptr, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    puts(info_log);
    exit(1);
  }
  free(fs_src);

  shader->id = glCreateProgram();
  glAttachShader(shader->id, vertex_shader);
  glAttachShader(shader->id, fragment_shader);
  glLinkProgram(shader->id);

  glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader->id, 512, NULL, info_log);
    puts(info_log);
    exit(1);
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader;
}

void shader_destroy(Shader *shader) {
  glDeleteProgram(shader->id);
  free(shader);
}

void shader_use(Shader *shader) { glUseProgram(shader->id); }

void shader_set_1i(Shader *shader, const char *name, int value) {
  glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void shader_set_1f(Shader *shader, const char *name, float value) {
  glUniform1f(glGetUniformLocation(shader->id, name), value);
}

void shader_set_mat4fv(Shader *shader, const char *name, mat4 value) {
  glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE,
                     value[0]);
}
