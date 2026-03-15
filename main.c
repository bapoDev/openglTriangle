#include "glad/include/glad/gl.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char* strread(FILE* stream)
{
    char* res = (char *)malloc(sizeof(char));
    *res = 0;
    assert(res);
    int size = 1;
    while (!feof(stream)) {
        res = (char *)realloc(res, size * sizeof(char) + 1);
        assert(res);
        res[size - 1] = fgetc(stream);
        res[size] = 0;
        size++;
    }
    res[size - 2] = 0;
    return res;
}

void error_callback_glfw(int error, const char* description)
{
    fprintf(stderr, "GLFW ERROR: code %i msg: %s.\n", error, description);
}

int main(void)
{
    printf("Starting GLFW %s.\n", glfwGetVersionString());

    glfwSetErrorCallback(error_callback_glfw);

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.:\n");
        return 1;
    }

    bool full_screen = true;

    GLFWmonitor* mon = NULL;
    int win_w = 800, win_h = 600;

    if (full_screen) {
        mon = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(mon);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        win_w = mode->width;
        win_h = mode->height;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    GLFWwindow* window = glfwCreateWindow(
        win_w,
        win_h,
        "Extended OpenGL Init",
        mon,
        NULL);
    if (!window) {
        fprintf(stderr, "ERROR: Could not open window with GLFW3.\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    int version_glad = gladLoadGL(glfwGetProcAddress);
    if (version_glad == 0) {
        fprintf(stderr, "ERROR: Failed to initialize OpenGL context.\n");
        return 1;
    }
    printf("Loaded OpenGL %i.%i\n", GLAD_VERSION_MAJOR(version_glad), GLAD_VERSION_MINOR(version_glad));
    printf("Renderer: %s.\n", glGetString(GL_RENDERER));
    printf("OpenGL version suppoprted %s.\n", glGetString(GL_VERSION));

    float points[] = {
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    FILE* vert = fopen("./shaders/test.vert", "r");
    FILE* frag = fopen("./shaders/test.frag", "r");

    const char* vertex_shader = strread(vert);
    const char* fragment_shader = strread(frag);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    int params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);

    if (GL_TRUE != params) {
        int max_length = 2048, actual_length = 0;
        char slog[2048];
        glGetShaderInfoLog(vs, max_length, &actual_length, slog);
        fprintf(stderr, "ERROR: Shader index %u did not compile.\n%s\n", vs, slog);
        return 1;
    }
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);

    if (GL_TRUE != params) {
        int max_length = 2048, actual_length = 0;
        char slog[2048];
        glGetShaderInfoLog(fs, max_length, &actual_length, slog);
        fprintf(stderr, "ERROR: Shader index %u did not compile.\n%s\n", fs, slog);
        return 1;
    }
    
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    int time_loc = glGetUniformLocation( shader_program, "time" );
    int s_width = glGetUniformLocation(shader_program, "s_width");
    int s_height = glGetUniformLocation(shader_program, "s_height");
    assert(time_loc > -1);
    
    glDeleteShader(fs);
    glDeleteShader(vs);
    
    glGetProgramiv(shader_program, GL_LINK_STATUS, &params);

    if (GL_TRUE != params) {
        int max_length = 2048, actual_length = 0;
        char plog[2048];
        glGetProgramInfoLog( shader_program, max_length, &actual_length, plog);
        fprintf(stderr, "ERROR: Could not link shader program GL index %u.\n%s\n", shader_program, plog);
        return 1;
    }

    double prev_s = glfwGetTime();
    double title_countdown_s = 1.0;

    while (!glfwWindowShouldClose(window)) {

        double curr_s = glfwGetTime();
        double elapsed_s = curr_s - prev_s;
        prev_s = curr_s;

        title_countdown_s -= elapsed_s;

        if (title_countdown_s <= 0.0 && elapsed_s > 0.0) {
            double fps = 1.0 / elapsed_s;
            char tmp[256];
            sprintf(tmp, "FPS %.2lf", fps);
            glfwSetWindowTitle(window, tmp);
            title_countdown_s = 1.0;
        }
        
        glfwPollEvents();

        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, 1);

        glfwGetWindowSize(window, &win_w, &win_h);
        glViewport(0, 0, win_w, win_h);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

        glUseProgram(shader_program);
        glUniform1f(time_loc, (float)curr_s);
        glUniform1i(s_width, win_w);
        glUniform1i(s_height, win_h);
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 1, 3);

        glfwSwapBuffers(window);
    }

    free((char*)vertex_shader);
    free((char*)fragment_shader);
    fclose(vert);
    fclose(frag);
    glfwTerminate();
    return 0;
}
