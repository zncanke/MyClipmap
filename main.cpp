#include "global.h"
#include "RawFile.h"
#include "shader.h"
using namespace std;

const int WIDTH = 1024, HEIGHT = 512;
const int TSIZE = 8;

RawFile rawFile;

struct Point {
    float x, y;
};

Point cursor;

GLuint vbo;

GLfloat vertices[] = {
        -0.5f,  0.0f, -0.5f,  //0.0f, 1.0f,
        0.5f,  0.0f, -0.5f,  //1.0f, 1.0f,
        0.5f,  0.0f,  0.5f,  //1.0f, 0.0f,
        0.5f,  0.0f,  0.5f,  //1.0f, 0.0f,
        -0.5f,  0.0f,  0.5f,  //0.0f, 0.0f,
        -0.5f,  0.0f, -0.5f,  //0.0f, 1.0f
};

Shader shader;

void init();
void drawFrame();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

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
    glfwSetCursorPosCallback(window, mouse_callback);

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

	glDeleteBuffers(1, &vbo);
	glfwTerminate();
    return 0;
}

void init() {
    rawFile.loadRawFile("/Users/willl/MyClipmap/data.raw");
    //for (int i = 0; i < SIZE; i++)
    //    for (int j = 0; j < SIZE; j++)
    //        printf("(%d, %d):%d\n", i, j, rawFile.getHeight(i, j));

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.attach(GL_VERTEX_SHADER, "/Users/willl/MyClipmap/vertex.txt");
	shader.attach(GL_FRAGMENT_SHADER, "/Users/willl/MyClipmap/fragment.txt");
	shader.link();
}

void drawFrame() {
    glClearDepth(1.0f);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);

	static int viewPos[] = { 0, 1, 0 };
	static float viewAngle;
    viewAngle = cursor.x / (float)4.0;
//    printf("%.2f\n", viewAngle);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(90.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.01f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(viewAngle, 0, 1, 0);
    glTranslatef(0, -0.2f, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)0);
	shader.begin();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	shader.end();
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//    printf("enterMouseCallback: %.2lf %.2lf\n", xpos, ypos);
    cursor.x = (float)xpos;
    cursor.y = (float)ypos;
}