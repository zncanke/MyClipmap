#include "global.h"
#include "RawFile.h"
#include "shader.h"
using namespace std;

const int WIDTH = 1024, HEIGHT = 512;

RawFile rawFile;

GLuint vbo;
GLfloat vertices[] = { -0.5f, -0.2f, -0.5f,
						-0.5f, -0.2f, 0.5f,
						0.5f, -0.2f, 0.5f,
						0.5f, -0.2f, -0.5f,
						-0.5f, -0.2f, -0.5f,
						0.5f, -0.2f, 0.5f };
//GLfloat vertices[] = { -0.5f, -0.5f,0.0f,
//						-0.5f, 0.5f,0.0f,
//						0.5f,  0.5f,0.0f,
//						0.5f,  -0.5f,0.0f,
//						-0.5f, -0.5f,0.0f,
//						0.5f, 0.5f, 0.0f  };
Shader shader;

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

	glDeleteBuffers(1, &vbo);
	glfwTerminate();
    return 0;
}

void init() {
    rawFile.loadRawFile("data.raw");
    //for (int i = 0; i < SIZE; i++)
    //    for (int j = 0; j < SIZE; j++)
    //        printf("(%d, %d):%d\n", i, j, rawFile.getHeight(i, j));

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.attach(GL_VERTEX_SHADER, "vertex.txt");
	shader.attach(GL_FRAGMENT_SHADER, "fragment.txt");
	shader.link();
}

void drawFrame() {
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	static int viewPos[] = { 0, 0.2, 0 };
	static float viewAngle = 0.0f;
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.0001f, 1.0f);

	glTranslatef(0, -viewPos[1], 0);
	//glRotatef(viewAngle, 0, 1, 0);

	shader.begin();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	shader.end();
}