#include "global.h"
#include "RawFile.h"
#include "shader.h"
using namespace std;
using namespace glm;

const int WIDTH = 1024, HEIGHT = 512;
const int GRID = 8;
const int LEVEL = 3;

RawFile rawFile;

struct Point {
    float x, y;
};

Point cursor;

GLuint vbo;

GLfloat tColor[][4] = {
        1, 0, 0, 1, //read
        0, 1, 0, 1, //green
        0, 0, 1, 1, //dark blue
        1, 0, 1, 1, //purple
        1, 1, 0, 1, //yellow
        0, 1, 1, 1 //light blue
};

vector<GLfloat> vertices;

Shader shader;

void init();
void drawFrame();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawGrid();
void buildGrid();
void setUniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

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

    buildGrid();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.attach(GL_VERTEX_SHADER, "/Users/willl/MyClipmap/vertex.txt");
	shader.attach(GL_FRAGMENT_SHADER, "/Users/willl/MyClipmap/fragment.txt");
	shader.link();
}

void buildGrid() {
//    freopen("/Users/willl/MyClipmap/mypoints.txt", "w", stdout);
    vertices.clear();
    for (int j = 0; j < GRID+1; j++)
        for (int i = 0; i < GRID+2; i++) {

            for (int k = 0; k < ((i == 0) ? 2 : 1); k++) {
                vertices.push_back((float)i / GRID);
                vertices.push_back(0);
                vertices.push_back((float)j / GRID);
//                printf("%d, %d, %d\n", i, 0, j);
            }

            j++;
            for (int k = 0; k < ((i == GRID + 1) ? 2 : 1); k++) {
                vertices.push_back(float(i) / GRID);
                vertices.push_back(0);
                vertices.push_back(float(j) / GRID);
//                printf("%d, %d, %d\n", i, 0, j);
            }
            j--;
        }
//    freopen("CON", "w", stdout);
}

void drawFrame() {
    glClearDepth(1.0f);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);

	static float viewPos[] = { 0, 0.3f, 0 };
	static float viewAngle;
    viewAngle = cursor.x / (float)4.0;
//    printf("%.2f\n", viewAngle);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(90.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.01f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(viewAngle, 0, 1, 0);
    glTranslatef(0, -viewPos[1], 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)0);
	shader.begin();
	drawGrid();
	shader.end();
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool frustumCull() {
    return false;
}

void drawGrid() {
    float scale = 1;
    for (int l = 0; l < LEVEL; l++) {
        setUniform4f("scale", scale, 1, scale, 1);
        setUniform4f("tColor", tColor[l][0], tColor[l][1], tColor[l][2], tColor[l][3]);
        for (int i = -2; i < 2; i++)
            for (int j = -2; j < 2; j++) {
                if (l != LEVEL - 1 && i != -2 && i != 1 && j != -2 && j != 1)
                    continue;
                vec3 offset(i, 0, j);
                setUniform4f("offset", offset.x, offset.y, offset.z, 0);
                if (frustumCull())
                    continue;
                glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
            }
        scale *= 0.5;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//    printf("enterMouseCallback: %.2lf %.2lf\n", xpos, ypos);
    cursor.x = (float)xpos;
    cursor.y = (float)ypos;
}

void setUniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
    GLint loc = glGetUniformLocation(shader.Program, name);
    if (loc == -1) {
        printf("Variable %s in shader not found\n", name);
    }
    glUniform4f(loc, v0, v1, v2, v3);
}