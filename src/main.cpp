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

enum ShaderVertexAttribute : int
{
    ATTRIBUTE_POSITIONS,
    ATTRIBUTE_NORMALS,
    ATTRIBUTE_TCOORDS
};



struct App
{
    
    int attribute_color;
    int color_space;
    GLFWwindow* window;
    int object_index;
};

struct Mesh
{

    int index_count; 
    std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texture_coordinates;//texture coordinates

    GLuint vao;
    GLuint vbo_positions;
    GLuint vbo_tcoords;
    GLuint vbo_normals;
    //fastObjMesh cpu_mesh;
};

void LoadMesh(Mesh* mesh, const char* path)
{
    fastObjMesh* obj_mesh;
    obj_mesh = fast_obj_read(path);

    mesh->index_count = obj_mesh->index_count;
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

    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo_positions);
    glGenBuffers(1, &mesh->vbo_normals);
    glGenBuffers(1, &mesh->vbo_tcoords);
   
    // TODO -- generate vbo handles for mesh normals and mesh tcoords

    glBindVertexArray(mesh->vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_positions);
    glVertexAttribPointer(ATTRIBUTE_POSITIONS, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBufferData(GL_ARRAY_BUFFER, mesh->positions.size() * sizeof(Vector3), mesh->positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_tcoords);
    glVertexAttribPointer(ATTRIBUTE_TCOORDS, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBufferData(GL_ARRAY_BUFFER, mesh->texture_coordinates.size() * sizeof(Vector2), mesh->texture_coordinates.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(ATTRIBUTE_TCOORDS);


    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);
    glVertexAttribPointer(ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(Vector3), mesh->normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(ATTRIBUTE_NORMALS);



    

    



    // TODO -- describe vertex buffers for normals (1) and tcoords (2)

    glBindVertexArray(GL_NONE);

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
    Mesh meshes[2]; 
    LoadMesh(&meshes[0], "./assets/meshes/plane.obj");
    LoadMesh(&meshes[1], "./assets/meshes/head.obj");
    
                                 

    GLuint vs_shader = LoadShader(GL_VERTEX_SHADER, "./assets/shaders/a3_mesh.vert");
    GLuint fs_shader = LoadShader(GL_FRAGMENT_SHADER, "./assets/shaders/a3_mesh.frag");
    GLuint shader_program = LoadProgram(vs_shader, fs_shader);


	

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(app.window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Matrix world = MatrixScale(0.5f, 0.5f, 0.5f) * MatrixRotateY(glfwGetTime() * 100.0f * DEG2RAD);
        Matrix world = MatrixIdentity(); 
        //Matrix world = MatrixRotateY(180.0f * DEG2RAD);


        glUseProgram(shader_program);
        GLint loc_transform = glGetUniformLocation(shader_program, "u_transform");
        GLint loc_style = glGetUniformLocation(shader_program, "u_style");
        GLint loc_color_space = glGetUniformLocation(shader_program, "u_color_space");
        glUniformMatrix4fv(loc_transform, 1, GL_FALSE, MatrixToFloat(world));
        glUniform1i(loc_style, app.attribute_color);
        glUniform1i(loc_color_space, app.color_space);

        const Mesh& mesh = meshes[app.object_index];
        glBindVertexArray(mesh.vao);

        glDrawArrays(GL_TRIANGLES, 0, mesh.index_count);

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

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        // Bound our array to be [0..2]
        ++app->attribute_color %= 3;
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        // Bound our array to be [0..1]
        ++app->color_space %= 2;
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
