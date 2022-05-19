#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstdio>
#include <string>
#include <unistd.h>
#include "mesh.h"
#include "renderer.h"

#include <thread>
#include <chrono>

int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

#define MinResolution 5
#define MaxResolution 100
#define PI (3.14159265358979323846)
// states
static bool iswireframe = 0; // wire mode or face mode
static bool islight = 1;     // light mode or not
static bool toSimplify = 0;  // simplify or not
// parameters
static unsigned int numVertices = 0;
static unsigned int numFaces = 0;
static int vpixelResolution = 100;    // cut the mesh into vpixelResolution*vpixelResolution*vpixelResolution pieces
std::string loadedFileName = "teddy"; // default file name
std::string saveFileName = "teddy";   // default file name, used to save the unchanged mesh
// rotate
static float camRotate = 0;
static float lightRotate = 0;

void drawGui();
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
std::string getCurrentWorkingDirectory()
{
    char cCurrentPath[FILENAME_MAX];

    if (!getcwd(cCurrentPath, sizeof(cCurrentPath)))
    {
        return std::string();
    }

    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    std::string currentPath = std::string(cCurrentPath);
    int found = currentPath.find("build");
    currentPath = currentPath.substr(0, found);
    unsigned int i = 0;
    while (i < currentPath.size())
    {
        if (currentPath[i] == '\\')
        {
            currentPath[i] = '/';
        }
        ++i;
    }
    return currentPath;
}
void little_sleep(std::chrono::milliseconds us)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + us;
    do
    {
        std::this_thread::yield();
        // if (!awake) std::cout << "sleep" << std::endl;
        // else {std::cout << "-------- AWAKE" << std::endl; break;}
    } while (std::chrono::high_resolution_clock::now() < end);
}
int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mesh Simplification", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // get current working directory
    std::string currentPath = getCurrentWorkingDirectory();
    std::string path = std::string(currentPath) + "models/" + loadedFileName + ".off";
    // load mesh
    Mesh origmodel(path.c_str());
    Mesh processModel = origmodel;
    // compile shader
    Shader shader((std::string(currentPath) + "shader/VertexShader.glsl").c_str(), (std::string(currentPath) + "shader/FragmentShader.glsl").c_str());

    // create renderer
    Renderer renderer(shader.ID, processModel);
    // create ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    ImGui::GetIO().FontGlobalScale = 1 + float(SCR_WIDTH) / (1920);
    // loop
    while (!glfwWindowShouldClose(window))
    {
        // process input
        processInput(window);
        // check input in imgui
        if (loadedFileName != saveFileName)
        {
            saveFileName = loadedFileName;
            path = std::string(currentPath) + "model/" + loadedFileName + ".off";
            origmodel = Mesh(path.c_str());
            processModel = origmodel;
            vpixelResolution = 100;
            renderer.model = processModel;
            renderer.update();
        }
        if (toSimplify)
        {
            toSimplify = 0;
            processModel = origmodel;
            processModel.simplify(vpixelResolution);
            renderer.model = processModel;
            renderer.update();
        }
        // SwapBuffer

                // init render set
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        if (iswireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // render model
        shader.use();
        renderer.render(camRotate, lightRotate, islight);
        numVertices = processModel.get_vertex_count();
        numFaces = processModel.get_triangle_count();
        // draw ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        drawGui();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
        little_sleep(std::chrono::milliseconds(25));
    }
}
void drawGui()
{
    if (ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove))
    {
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::SetWindowSize(ImVec2(400, (float)SCR_HEIGHT));
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::Text(("Number of vertices : " + std::to_string(numVertices)).c_str());
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::Text(("Number of faces : " + std::to_string(numFaces)).c_str());
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("Grid resolution");
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.96f);
        ImGui::SliderInt("", &vpixelResolution, MinResolution, MaxResolution);

        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        {
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.2f);
            if (ImGui::Button("Simplify", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f)))
                toSimplify = true;
        }
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Checkbox("Wireframe", &iswireframe);
        ImGui::Dummy(ImVec2(0.0f, 3.0f));
        ImGui::Checkbox("Lighting", &islight);

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        ImGui::SetNextTreeNodeOpen(true);
        if (ImGui::CollapsingHeader("Rotation"))
        {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.77f);
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::SliderFloat("Model", &camRotate, 0, 2 * PI);
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::SliderFloat("Light", &lightRotate, 0, 2 * PI);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Load"))
            {
                if (ImGui::MenuItem("Teddy"))
                {
                    loadedFileName = "teddy";
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}