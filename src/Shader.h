#pragma once
#include <glad/glad.h>

GLuint LoadShader(GLint type, const char* path);
GLuint LoadProgram(GLuint vs, GLuint fs);
