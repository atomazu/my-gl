#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "utils.h"

typedef struct {
  vec3 position;
  vec2 texture_position;
} Vertex;

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);
void handle_camera(GLFWwindow *window, Camera *camera, float delta_time);
void handle_window(GLFWwindow *window);

Mouse mouse = {.sensitivity = 0.1f};

int main(int argc, char *argv[]) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    puts("window creation failed");
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, cursor_pos_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    puts("couldn't load gl functions");
    glfwTerminate();
    exit(1);
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(opengl_debug_callback, NULL);

  Camera camera;
  camera_init((vec3){0.0f, 0.0f, -4.0f}, (vec3){0.0f, 0.0f, 1.0f},
              (vec2){90.0f, 0.0f}, 60.0f, &camera);

  glViewport(0, 0, 800, 600);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  shader_t shader = shader_create("./assets/shaders/default.vert",
                                  "./assets/shaders/default.frag");

  shader_t light_shader = shader_create("./assets/shaders/light.vert",
                                        "./assets/shaders/light.frag");

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

  Vertex vertices[] = {// Front face
                       {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
                       {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
                       {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
                       {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},

                       // Back face
                       {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
                       {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
                       {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},
                       {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},

                       // Left face
                       {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
                       {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
                       {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
                       {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},

                       // Right face
                       {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
                       {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
                       {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
                       {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},

                       // Top face
                       {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
                       {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
                       {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
                       {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},

                       // Bottom face
                       {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
                       {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
                       {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
                       {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}}};

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // vao
  unsigned int crate_VAO;
  glGenVertexArrays(1, &crate_VAO);
  glBindVertexArray(crate_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);

  // tex coords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // light vao
  unsigned int light_VAO;
  glGenVertexArrays(1, &light_VAO);
  glBindVertexArray(light_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);

  // tex coords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // draw order
  glEnable(GL_DEPTH_TEST);

  float last_frame = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    float delta_time = glfwGetTime() - last_frame;
    last_frame = glfwGetTime();

    handle_window(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view, proj;
    handle_camera(window, &camera, delta_time);
    camera_view(&camera, (vec3){0.0f, 1.0f, 0.0f}, &view);
    camera_projection(&camera, &proj);

    vec3 light_position = {0.0f, -5.0f, 0.0f};
    vec3 light_color = {1.0f, 1.0f, 1.0f};
    vec3 object_color = {1.0f, 1.0f, 0.2f};

    shader_use(shader);
    shader_set_1i(shader, "aTexture", 0);
    shader_set_mat4fv(shader, "view", view);
    shader_set_mat4fv(shader, "proj", proj);
    shader_set_vec3fv(shader, "lightColor", light_color);

    // draw cubes
    texture_use(wall_texture, GL_TEXTURE0);
    vec3 variations[] = {{0.0f, 0.0f, 0.0f},    {2.0f, 5.0f, -15.0f},
                         {-1.5f, -2.2f, -2.5f}, {-3.8f, -2.0f, -12.3f},
                         {2.4f, -0.4f, -3.5f},  {-1.7f, 3.0f, -7.5f},
                         {1.3f, -2.0f, -2.5f},  {1.5f, 2.0f, -2.5f},
                         {1.5f, 0.2f, -1.5f},   {-1.3f, 1.0f, -1.5f}};

    glBindVertexArray(crate_VAO);
    for (int i = 0; i < 10; i++) {
      mat4 model = GLM_MAT4_IDENTITY_INIT;
      glm_translate(model, variations[i]);
      if (i % 3 == 0) {
        glm_rotate(model, glfwGetTime() * glm_rad(50.0f),
                   (vec3){0.5f, 1.0f, -0.5f});
      }
      vec3 scaled_color;
      float distance = glm_vec3_distance(variations[i], light_position);
      glm_vec3_scale(light_color, 5 / distance, scaled_color);
      glm_vec3_add(object_color, scaled_color, scaled_color);
      glm_vec3_scale(scaled_color, 5 / distance, scaled_color);
      shader_set_vec3fv(shader, "objectColor", scaled_color);
      shader_set_mat4fv(shader, "model", model);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    // draw light
    shader_use(light_shader);
    shader_set_mat4fv(light_shader, "view", view);
    shader_set_mat4fv(light_shader, "proj", proj);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, light_position);
    shader_set_mat4fv(light_shader, "model", model);

    glBindVertexArray(light_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void handle_camera(GLFWwindow *window, Camera *camera, float delta_time) {
  vec2 rotation_amount;
  float speed = 5.0f;
  glm_vec2_scale(mouse.delta, mouse.sensitivity, rotation_amount);
  camera_rotate(camera, rotation_amount, true);
  mouse_consume(&mouse);

  vec3 translation = {0.0f, 0.0f, 0.0f};
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    glm_vec3_add(translation, (vec3){0.0f, 0.0f, 1.0f}, translation);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    glm_vec3_add(translation, (vec3){0.0f, 0.0f, -1.0f}, translation);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    glm_vec3_add(translation, (vec3){1.0f, 0.0f, 0.0f}, translation);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    glm_vec3_add(translation, (vec3){-1.0f, 0.0f, 0.0f}, translation);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    glm_vec3_add(translation, (vec3){0.0f, 1.0f, 0.0f}, translation);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    glm_vec3_add(translation, (vec3){0.0f, -1.0f, 0.0f}, translation);
  }
  glm_vec3_normalize(translation);
  glm_vec3_scale(translation, speed * delta_time, translation);
  camera_translate(camera, translation);
}

void handle_window(GLFWwindow *window) {
  // close window if esc is pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_KEY_ESCAPE);

  // update frame buffer to match window size (workaround)
  int fb_width, fb_height;
  glfwGetFramebufferSize(window, &fb_width, &fb_height);
  glViewport(0, 0, fb_width, fb_height);
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
  mouse.delta[0] += xpos - mouse.position[0];
  mouse.delta[1] += mouse.position[1] - ypos;
  glm_vec2_copy((vec2){xpos, ypos}, mouse.position);
}
