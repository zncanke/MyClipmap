#include "RawFile.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;

const int WIDTH = 1024, HEIGHT = 512;

void init();
void drawFrame();

int main() {
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Geometry Clipmap", nullptr, nullptr);
    if (window == nullptr) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = (GLboolean)true;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    init();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
        glfwSwapBuffers(window);
    }
    return 0;
}

void init() {
    RawFile rawFile;
    rawFile.loadRawFile("/Users/willl/Desktop/MyClipmap/data.raw");
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            printf("(%d, %d):%d\n", i, j, rawFile.getHeight(i, j));
}


void drawFrame() {
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


}