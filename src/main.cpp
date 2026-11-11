#define _CRT_SECURE_NO_WARNINGS

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#include <raymath.h>

#include <cassert>
#include <iostream>
#include <vector>
#include "Shader.h"

void APIENTRY DebugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

struct App
{
    GLFWwindow* window;
    int object_index;
};

struct Mesh
{
    std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texture_coordinates;//texture coordinates
    //fastObjMesh cpu_mesh;
};

void LoadMesh(Mesh* mesh, const char* path)
{
    fastObjMesh* obj_mesh;
    obj_mesh = fast_obj_read(path);
    
    mesh->positions.resize(obj_mesh->index_count);
	mesh->normals.resize(obj_mesh->index_count);
	mesh->texture_coordinates.resize(obj_mesh->index_count);


	//copy the positions from the obj mesh to our mesh
    for (fastObjUInt i = 0; i < obj_mesh->index_count; i++)
    {
		Vector3* obj_positions_buffer = (Vector3*)obj_mesh->positions;// we can cast the positions buffer to a Vector3* because we know that the positions are stored as 3 floats per vertex 
		fastObjUInt obj_position_index = obj_mesh->indices[i].p;// the index of the position in the obj mesh's positions buffer
		mesh->positions[i] = obj_positions_buffer[obj_position_index];// copy the position from the obj mesh to our mesh
    }

    for (fastObjUInt i = 0; i < obj_mesh->index_count; i++)
    {
		Vector3* obj_normals_buffer = (Vector3*)obj_mesh->normals;// we can cast the normals buffer to a Vector3* because we know that the normals are stored as 3 floats per vertex 
		fastObjUInt obj_normal_index = obj_mesh->indices[i].n;// the index of the normal in the obj mesh's normals buffer
		mesh->normals[i] = obj_normals_buffer[obj_normal_index];// copy the normal from the obj mesh to our mesh
    }

    for (fastObjUInt i = 0; i < obj_mesh->index_count; i++)
    {
		Vector2* obj_texture_coordinates_buffer = (Vector2*)obj_mesh->texcoords;// we can cast the texture coordinates buffer to a Vector2* because we know that the texture coordinates are stored as 2 floats per vertex 
		fastObjUInt obj_texture_coordinate_index = obj_mesh->indices[i].t;// the index of the texture coordinate in the obj mesh's texture coordinates buffer
		mesh->texture_coordinates[i] = obj_texture_coordinates_buffer[obj_texture_coordinate_index];// copy the texture coordinate from the obj mesh to our mesh
    }

    fast_obj_destroy(obj_mesh);
}

void UnloadMesh(Mesh* mesh)
{
    
}

void Init(App* app);

int main()
{
    // we still need to render the texture coridanates and normals
    
    App app{};
    Init(&app);

    Mesh plane;
    Mesh head;
    LoadMesh(&plane, "./assets/meshes/plane.obj");
    LoadMesh(&head, "./assets/meshes/head.obj"); 
    
    GLuint mesh_vao = GL_NONE;
    GLuint mesh_positions_vbo = GL_NONE;
	GLuint mesh_normals_vbo = GL_NONE;
	GLuint mesh_texture_coordinates_vbo = GL_NONE;

	glGenVertexArrays(1, &mesh_vao);// Generate a vertex array object (VAO) to store the state of our vertex buffers

	glGenBuffers(1, &mesh_positions_vbo);// Generate a vertex buffer object (VBO) to store the positions of our mesh

	glGenBuffers(1, &mesh_normals_vbo);// Generate a vertex buffer object (VBO) to store the normals of our mesh

	glGenBuffers(1, &mesh_texture_coordinates_vbo);// Generate a vertex buffer object (VBO) to store the texture coordinates of our mesh

	glBindVertexArray(mesh_vao); // "Start recording vertex buffer state"

	glBindBuffer(GL_ARRAY_BUFFER, mesh_positions_vbo);// Bind the positions VBO to the GL_ARRAY_BUFFER target
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Tell OpenGL how to interpret the data in the positions VBO
	glBufferData(GL_ARRAY_BUFFER, plane.positions.size() * sizeof(Vector3), plane.positions.data(), GL_STATIC_DRAW); // Upload the data in the positions VBO to the GPU


	glBindBuffer(GL_ARRAY_BUFFER, mesh_normals_vbo);// Bind the normals VBO to the GL_ARRAY_BUFFER target
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Tell OpenGL how to interpret the data in the normals VBO
	glBufferData(GL_ARRAY_BUFFER, plane.normals.size() * sizeof(Vector3), plane.normals.data(), GL_STATIC_DRAW); // Upload the data in the normals VBO to the GPU 

	glBindBuffer(GL_ARRAY_BUFFER, mesh_texture_coordinates_vbo);// Bind the texture coordinates VBO to the GL_ARRAY_BUFFER target
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr); // Tell OpenGL how to interpret the data in the texture coordinates VBO
	glBufferData(GL_ARRAY_BUFFER, plane.texture_coordinates.size() * sizeof(Vector2), plane.texture_coordinates.data(), GL_STATIC_DRAW); // Upload the data in the texture coordinates VBO to the GPU

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);           


    glBindVertexArray(GL_NONE);                                 // "Stop recording vertex buffer state"

    GLuint vs_shader = LoadShader(GL_VERTEX_SHADER, "./assets/shaders/a3_mesh.vert");
    GLuint fs_shader = LoadShader(GL_FRAGMENT_SHADER, "./assets/shaders/a3_mesh.frag");
    GLuint shader_program = LoadProgram(vs_shader, fs_shader);


	

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(app.window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Matrix world = MatrixScale(0.5f, 0.5f, 0.5f) * MatrixRotateY(glfwGetTime() * 100.0f * DEG2RAD);

        glUseProgram(shader_program);
        GLint loc = glGetUniformLocation(shader_program, "u_transform");
		GLint loc2 = glGetUniformLocation(shader_program, "u_mode");// Get the location of the uniform variable u_mode in the shader program
		// set the uniform variable u_mode to the value of app.object_index


        glUniformMatrix4fv(loc, 1, GL_FALSE, MatrixToFloat(world));
		glUniform1i(loc2, app.object_index);// Set the value of the uniform variable u_mode to the value of app.object_index

		glBindVertexArray(mesh_vao);// Bind the VAO to use the state we recorded earlier

		glDrawArrays(GL_TRIANGLES, 0, plane.positions.size());// Draw the mesh using the positions VBO
		// normals and texture coordinates are not used in this draw call, but they are still bound to the VAO and can be used in the shader if needed so thats what we are going to do now
		// we can use the normals and texture coordinates in the shader by using the layout(location = 1) and layout(location = 2) qualifiers in the vertex shader to specify which attribute location to use for the normals and texture coordinates
		// we can also use the normals and texture coordinates in the fragment shader by using the in qualifier to specify which attribute location to use for the normals and texture coordinates


        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    App* app = (App*)glfwGetWindowUserPointer(window);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        // Bound our array to be [0..4]
        ++app->object_index %= 2;
		std::cout << "Object index: " << app->object_index << std::endl;
        

    }
}

void Init(App* app)
{
    assert(glfwInit() == GLFW_TRUE);

    // Request certain OpenGL features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef NDEBUG
    // Don't make a debug context if in release mode
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    app->window = glfwCreateWindow(1600, 900, "Graphics 1", nullptr, nullptr);
    assert(app->window != nullptr);

    glfwSetWindowUserPointer(app->window, app);
    glfwSetKeyCallback(app->window, KeyCallback);

    glfwMakeContextCurrent(app->window);
    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == true);

#ifdef NDEBUG
#else
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(DebugCallback, nullptr);
#endif
}

void APIENTRY DebugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}
