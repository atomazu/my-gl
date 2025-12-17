#include "camera.h"

void mouse_consume(Mouse *mouse) {
  glm_vec2_copy((vec2){0.0f, 0.0f}, mouse->delta);
}

void camera_init(vec3 position, vec3 direction, vec2 rotation, float fov,
                 Camera *camera) {
  glm_vec3_copy(position, camera->position);
  glm_vec3_copy(direction, camera->direction);
  glm_vec2_copy(rotation, camera->rotation);
  camera->fov = fov;
}

void camera_rotate(Camera *camera, vec2 rotation, bool clamp) {
  glm_vec2_add(camera->rotation, rotation, camera->rotation);
  if (clamp)
    camera->rotation[1] = glm_clamp(camera->rotation[1], -89.0f, 89.0f);

  // update direction vector
  float yaw_rad = glm_rad(camera->rotation[0]);
  float pitch_rad = glm_rad(camera->rotation[1]);
  vec3 new_dir = {cos(yaw_rad) * cos(pitch_rad), sin(pitch_rad),
                  sin(yaw_rad) * cos(pitch_rad)};
  glm_vec3_copy(new_dir, camera->direction);
  glm_normalize(camera->direction);
}

void lookat(vec3 eye, vec3 center, vec3 up, mat4 dest) {
  vec3 dir;
  glm_vec3_sub(eye, center, dir);
  glm_normalize(dir);

  vec3 right;
  glm_vec3_cross(up, dir, right);
  glm_normalize(right);

  vec3 cam_up;
  glm_vec3_cross(dir, right, cam_up);
  glm_normalize(up);

  mat4 translation = {{1.0f, 0.0f, 0.0f, 0.0f},
                      {0.0f, 1.0f, 0.0f, 0.0f},
                      {0.0f, 0.0f, 1.0f, 0.0f},
                      {-eye[0], -eye[1], -eye[2], 1.0f}};

  mat4 orientation = {{right[0], cam_up[0], dir[0], 0.0f},
                      {right[1], cam_up[1], dir[1], 0.0f},
                      {right[2], cam_up[2], dir[2], 0.0f},
                      {0.0f, 0.0f, 0.0f, 1.0f}};

  glm_mat4_mul(orientation, translation, dest);
}

void camera_view(Camera *camera, vec3 world_up, mat4 *view) {
  vec3 center;
  glm_vec3_add(camera->position, camera->direction, center);
  lookat(camera->position, center, world_up, *view);
}

void camera_projection(Camera *camera, mat4 *projection) {
  glm_perspective(glm_rad(camera->fov), 8.0f / 6.0f, 0.1f, 100.0f, *projection);
}

void camera_translate(Camera *camera, vec3 translation) {
  vec3 camera_front;
  glm_vec3_copy(camera->direction, camera_front);
  glm_vec3_scale(camera_front, translation[2], camera_front);
  glm_vec3_add(camera->position, camera_front, camera->position);

  vec3 camera_right;
  glm_vec3_cross(camera->direction, (vec3){0.0f, 1.0f, 0.0f}, camera_right);
  glm_vec3_normalize(camera_right);
  glm_vec3_scale(camera_right, translation[0], camera_right);
  glm_vec3_add(camera->position, camera_right, camera->position);

  vec3 up_movement;
  glm_vec3_scale((vec3){0.0f, 1.0f, 0.0f}, translation[1], up_movement);
  glm_vec3_add(camera->position, up_movement, camera->position);
}

void camera_destroy(Camera *camera) { free(camera); }
