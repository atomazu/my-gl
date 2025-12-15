#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

typedef struct {
  vec3 position;
  vec3 direction;
  vec2 rotation;
  float fov;
} Camera;

typedef struct {
  vec2 position;
  vec2 delta;
  float sensitivity;
  bool read;
} Mouse;

void mouse_consume(Mouse *mouse);

void camera_init(vec3 position, vec3 direction, vec2 rotation, float fov,
                 Camera *camera);

void camera_rotate(Camera *camera, vec2 rotation, bool clamp);
void camera_view(Camera *camera, vec3 world_up, mat4 *view);
void camera_projection(Camera *camera, mat4 *projection);
void camera_translate(Camera *camera, vec3 translation);
void camera_destroy(Camera *camera);

#endif
