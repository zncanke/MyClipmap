#include "global.h"
#include "RawFile.h"
#include "shader.h"
#include "GeotiffFile.h"
using namespace std;
using namespace glm;

const int WIDTH = 1024, HEIGHT = 512;
const int GRID = 32;
const int LEVEL = 3;

RawFile rawFile;
GeotiffFile geotiffFile;

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
GLuint texHeightMap;

void init();
void drawFrame();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawGrid();
void buildGrid();
void setUniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void setUniformi(const char* name, GLint val);

void testProc() {
    mat4 mat0, mat1;
    glGetFloatv(GL_PROJECTION_MATRIX, &mat0[0][0]);
    glGetFloatv(GL_MODELVIEW_MATRIX, &mat1[0][0]);
    mat0 = mat0 * mat1;

    freopen(testOutput, "w", stdout);
    for (int i = 0; i < vertices.size(); i += 3) {
        vec4 pos(vertices[i], vertices[i+1], vertices[i+2], 1.0);
        vec4 offset(-1, -1, -1, 0);
        pos += offset;
        vec3 tpos(pos.x, pos.y, pos.z);
        tpos /= 2;
        tpos += 0.5;
        int x = (int)(tpos.x * 3601);
        int z = (int)(tpos.z * 3601);
        pos.y = geotiffFile.getHeightMap()[x * geotiffFile.getWidth() + z];
        if (pos.y != 0) {
            printf("not zero\n");
        }
        pos = mat0 * pos;
        printf("%d %d : %.2f\n", x, z, pos.y / pos.w);
    }
    freopen("CON", "w", stdout);
}

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

//    testProc();

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
//    rawFile.loadRawFile(rawFilePath);
//    freopen(mypointsPath, "w", stdout);
//    for (int i = 0; i < DATASIZE; i++)
//        for (int j = 0; j < DATASIZE; j++)
//            printf("(%d, %d):%d\n", i, j, rawFile.getRaw(i, j));

//    rawFile.generateHeightMap();

    geotiffFile.loadTiffFile(tiffFilePath);

    glGenTextures(1, &texHeightMap);
    glBindTexture(GL_TEXTURE_2D, texHeightMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, geotiffFile.getWidth(), geotiffFile.getHeight(),
                 0, GL_LUMINANCE, GL_FLOAT, geotiffFile.getHeightMap());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    buildGrid();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.attach(GL_VERTEX_SHADER, vertexShaderPath);
	shader.attach(GL_FRAGMENT_SHADER, fragmentShaderPath);
	shader.link();
}

void buildGrid() {
    //freopen(mypointsPath, "w", stdout);
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

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texHeightMap);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)0);
	shader.begin();
    setUniformi("texHeightMap", 0);
	drawGrid();
	shader.end();
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool frustumCull() {
    mat4 mat0, mat1;
    glGetFloatv(GL_PROJECTION_MATRIX, &mat0[0][0]);
    glGetFloatv(GL_MODELVIEW_MATRIX, &mat1[0][0]);
    mat0 = mat0 * mat1;
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

void setUniformi(const char* name, GLint val) {
    GLint loc = glGetUniformLocation(shader.Program, name);
    if (loc == -1) {
        cerr << "Variable " << name << " in shader not found" << endl;
    }
    glUniform1i(loc, val);
}