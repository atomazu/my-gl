#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include "shader.h"

unsigned int texture_load(char *path, int format) {
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

void texture_use(unsigned int texture, unsigned int unit) {
  glActiveTexture(unit);
  glBindTexture(GL_TEXTURE_2D, texture);
}

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
  Shader *shader = shader_create("./assets/shaders/default.vert",
                                 "./assets/shaders/default.frag");

  float vertices[] = {
      // positions        // colors         // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };

  // load texture
  unsigned int wall_texture = texture_load("assets/wall.jpg", GL_RGB);
  unsigned int smiley_texture = texture_load("assets/awesomeface.png", GL_RGBA);

  // ebo
  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // tex coords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(shader);

  // create transformation matrix
  mat4 trans = GLM_MAT4_IDENTITY_INIT;
  glm_translate(trans, (vec3){0.5f, -0.5f, 0.0f});
  glm_rotate(trans, glm_rad(90.0f), (vec3){0.0f, 0.0f, 1.0f});

  mat4 trans2 = GLM_MAT4_IDENTITY_INIT;
  glm_translate(trans2, (vec3){-0.5f, 0.5f, 0.0f});

  shader_set_mat4fv(shader, "transform", trans);
  shader_set_1i(shader, "texture1", 0);
  shader_set_1i(shader, "texture2", 1);

  float scale = 1.0f;

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_KEY_ESCAPE);

    // update frame buffer to match window size (workaround)
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    glClear(GL_COLOR_BUFFER_BIT);

    texture_use(wall_texture, GL_TEXTURE0);
    texture_use(smiley_texture, GL_TEXTURE1);

    glm_rotate(trans, glm_rad(1.0f), (vec3){0.0f, 0.0f, 1.0f});
    shader_set_mat4fv(shader, "transform", trans);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    scale = sin(glfwGetTime());
    glm_scale(trans2, (vec3){scale, scale, scale});
    shader_set_mat4fv(shader, "transform", trans2);
    glm_scale(trans2, (vec3){1.0f / scale, 1.0f / scale, 1.0f / scale});
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
