#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include "shader.h"
#include "texture.h"

int main(int argc, char *argv[]) {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    puts("window creation failed");
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    puts("couldn't load gl functions");
    glfwTerminate();
    exit(1);
  }

  glViewport(0, 0, 800, 600);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  shader_t shader = shader_create("./assets/shaders/default.vert",
                                  "./assets/shaders/default.frag");
  float vertices[] = {
      // positions          // texture coords
      // Front face
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f,
      0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,

      // Back face
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,
      0.5f, -0.5f, 0.0f, 1.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,

      // Left face
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -0.5f,
      0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

      // Right face
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f,
      -0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f,

      // Top face
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f,
      -0.5f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

      // Bottom face
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,
      -0.5f, 0.5f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f};

  // load texture
  texture_t wall_texture = texture_load("assets/wall.jpg", GL_RGB);

  // ebo
  unsigned int indices[] = {// Front
                            0, 1, 2, 2, 3, 0,
                            // Back
                            4, 5, 6, 6, 7, 4,
                            // Left
                            8, 9, 10, 10, 11, 8,
                            // Right
                            12, 13, 14, 14, 15, 12,
                            // Top
                            16, 17, 18, 18, 19, 16,
                            // Bottom
                            20, 21, 22, 22, 23, 20};
  unsigned int EBO;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // vbo
  unsigned int VBO;
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // vao
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // tex coords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(shader);

  shader_set_1i(shader, "aTexture", 0);
  glEnable(GL_DEPTH_TEST);

  // set up camera
  vec3 camera_pos = {0.0f, 0.0f, 3.0f};
  vec3 camera_dir = {0.0f, 0.0f, -1.0f};
  vec3 camera_up = {0.0f, 1.0f, 0.0f};
  float speed = 10.0f;
  float last_frame = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    float delta_time = glfwGetTime() - last_frame;
    last_frame = glfwGetTime();

    // close window if esc is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_KEY_ESCAPE);

    // update frame buffer to match window size (workaround)
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      vec3 camera_front;
      glm_vec3_copy(camera_dir, camera_front);
      glm_vec3_scale(camera_front, speed * delta_time, camera_front);
      glm_vec3_add(camera_pos, camera_front, camera_pos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      vec3 camera_front;
      glm_vec3_copy(camera_dir, camera_front);
      glm_vec3_scale(camera_front, speed * delta_time, camera_front);
      glm_vec3_sub(camera_pos, camera_front, camera_pos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      vec3 camera_right;
      glm_vec3_cross(camera_dir, camera_up, camera_right);
      glm_vec3_normalize(camera_right);
      glm_vec3_scale(camera_right, speed * delta_time, camera_right);
      glm_vec3_add(camera_pos, camera_right, camera_pos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      vec3 camera_right;
      glm_vec3_cross(camera_dir, camera_up, camera_right);
      glm_vec3_normalize(camera_right);
      glm_vec3_scale(camera_right, speed * delta_time, camera_right);
      glm_vec3_sub(camera_pos, camera_right, camera_pos);
    }

    vec3 center;
    glm_vec3_add(camera_pos, camera_dir, center);

    mat4 view = GLM_MAT4_IDENTITY_INIT;
    glm_lookat(camera_pos, center, camera_up, view);

    mat4 proj;
    glm_perspective(glm_rad(45.0f), 8.0f / 6.0f, 0.1f, 100.0f, proj);

    shader_set_mat4fv(shader, "view", view);
    shader_set_mat4fv(shader, "proj", proj);

    // draw cubes
    texture_use(wall_texture, GL_TEXTURE0);
    vec3 variations[] = {{0.0f, 0.0f, 0.0f},    {2.0f, 5.0f, -15.0f},
                         {-1.5f, -2.2f, -2.5f}, {-3.8f, -2.0f, -12.3f},
                         {2.4f, -0.4f, -3.5f},  {-1.7f, 3.0f, -7.5f},
                         {1.3f, -2.0f, -2.5f},  {1.5f, 2.0f, -2.5f},
                         {1.5f, 0.2f, -1.5f},   {-1.3f, 1.0f, -1.5f}};

    glBindVertexArray(VAO);
    for (int i = 0; i < 10; i++) {
      mat4 model = GLM_MAT4_IDENTITY_INIT;
      glm_translate(model, variations[i]);
      if (i % 3 == 0) {
        glm_rotate(model, glfwGetTime() * glm_rad(50.0f),
                   (vec3){0.5f, 1.0f, -0.5f});
      }
      shader_set_mat4fv(shader, "model", model);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
