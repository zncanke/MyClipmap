#include "global.h"
#include "RawFile.h"
#include "shader.h"
#include "GeotiffFile.h"
#include "skybox.h"
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
Point currentPos;

GLuint vbo;

GLfloat tColor[][4] = {
		1, 0, 0, 1, //red
		0, 1, 0, 1, //green
        0, 0, 1, 1, //dark blue
        1, 0, 1, 1, //purple
        1, 1, 0, 1, //yellow
        0, 1, 1, 1, //light blue
		0.72f, 0.72f, 0.72f, 1 // grey
};

vector<GLfloat> vertices;

#define texNum 4

Shader shader;
GLuint texHeightMap, tex[texNum];

Skybox skybox;

const string texName[] = { "mud", "grass", "stone", "ice", "detail" };

void init();
void drawFrame();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow *windows, int key, int scancode, int action, int mode);
void drawGrid();
void buildGrid();
void setUniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void setUniformi(const char* name, GLint val);
GLuint genTexture(int width, int height, int type0, int type1, int type2, unsigned char * data);

void testProc() {
    mat4 mat0, mat1;
	mat0 = translate(mat0, vec3(0, -0.3f, 0));
	//vec4 tv(1, 1, 1, 1);
	//tv = mat0 * tv;
	mat1 = perspective(90.0f, 2.0f, 0.01f, 100.0f);
	mat0 = mat1 * mat0;

    freopen(testOutput, "w", stdout);
    for (int i = 0; i < vertices.size(); i += 3) {
        vec4 pos(vertices[i], vertices[i+1], vertices[i+2], 1.0);
        vec4 offset(-1, 0, -1, 0);
        pos += offset;
        vec3 tpos(pos.x, pos.y, pos.z);
        tpos /= 2;
        tpos += 0.5;
        int x = (int)(tpos.x * 3601);
        int z = (int)(tpos.z * 3601);
		if (x == 0 && z == 1800) {
			x = x;
		}
		pos = mat0 * pos;
		pos.y = geotiffFile.getHeightMap()[x * geotiffFile.getWidth() + z];
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
	glfwSetKeyCallback(window, key_callback);

    glewExperimental = (GLboolean)true;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    init();

    //testProc();

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

	currentPos.x = geotiffFile.getHeight() / 2;
	currentPos.y = geotiffFile.getWidth() / 2;

	texHeightMap = genTexture(geotiffFile.getWidth(), geotiffFile.getHeight(), GL_LUMINANCE, 
		GL_LUMINANCE, GL_FLOAT, (unsigned char*)geotiffFile.getHeightMap());

	for (int i = 0; i < texNum; i++) {
		string filePath = currentPath + texName[i] + ".jpg";
		int w, h;
		unsigned char* image = SOIL_load_image(filePath.c_str(), &w, &h, 0, SOIL_LOAD_RGB);
		tex[i] = genTexture(w, h, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

    buildGrid();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.attach(GL_VERTEX_SHADER, vertexShaderPath);
	shader.attach(GL_FRAGMENT_SHADER, fragmentShaderPath);
	shader.link();

	skybox.init();
}

GLuint genTexture(int width, int height, int type0, int type1, int type2, unsigned char * data) {
	GLuint ret;

	glGenTextures(1, &ret);
	glBindTexture(GL_TEXTURE_2D, ret);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, type0, width, height, 0, type1, type2, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return ret;
}

void buildGrid() {
    //freopen(mypointsPath, "w", stdout);
    vertices.clear();
    for (int j = 0; j < GRID+1; j++)
		for (int i = 0; i < GRID + 2; i++) {

			for (int k = 0; k < ((i == 0) ? 2 : 1); k++) {
				//			for (int k = 0; k < 1; k++) {
				vertices.push_back((float)i / GRID);
				vertices.push_back(0);
				vertices.push_back((float)j / GRID);
				//                printf("%d, %d, %d\n", i, 0, j);
			}

			j++;
			for (int k = 0; k < ((i == GRID + 1) ? 2 : 1); k++) {
				//		for (int k = 0; k < 1; k++) {
				vertices.push_back((float)i / GRID);
				vertices.push_back(0);
				vertices.push_back((float)j / GRID);
				//                printf("%d, %d, %d\n", i, 0, j);
			}
			j--;
		}
	for (int i = 0; i < 4; i++) {
		int tmp;
		tmp = (i < 2) ? 1 : 0;
		for (int j = 0; j < 2; j++) {
			vertices.push_back((float)(GRID + tmp) / GRID);
			vertices.push_back(0);
			vertices.push_back((float)GRID / GRID);
		}
	}

	//T-junction removal
	float x, y;
	x = y = GRID;
	for (int i = 0; i < GRID * 2; x -= 0.5, i++) {
		vertices.push_back((float)x / GRID);
		vertices.push_back(0);
		vertices.push_back((float)y / GRID);
	}
	vertices.push_back((float)x / GRID);
	vertices.push_back(0);
	vertices.push_back((float)y / GRID);
	for (int i = 0; i < GRID * 2; y -= 0.5, i++) {
		vertices.push_back((float)x / GRID);
		vertices.push_back(0);
		vertices.push_back((float)y / GRID);
	}
	vertices.push_back((float)x / GRID);
	vertices.push_back(0);
	vertices.push_back((float)y / GRID);
	for (int i = 0; i < GRID * 2; x += 0.5, i++) {
		vertices.push_back((float)x / GRID);
		vertices.push_back(0);
		vertices.push_back((float)y / GRID);
	}
	vertices.push_back((float)x / GRID);
	vertices.push_back(0);
	vertices.push_back((float)y / GRID);
	for (int i = 0; i < GRID * 2; y += 0.5, i++) {
		vertices.push_back((float)x / GRID);
		vertices.push_back(0);
		vertices.push_back((float)y / GRID);
	}
//    freopen("CON", "w", stdout);
}

void drawFrame() {
    glClearDepth(1.0f);
    glClearColor(0.22f, 0.46f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //  glEnable(GL_CULL_FACE);
   // glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);


	static float viewPos[] = { 0, 0.5f, 0 };
	static float viewAngle;
    viewAngle = cursor.x / (float)4.0;
//    printf("%.2f\n", viewAngle);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(90.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.01f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	//glRotatef(30.0f, 1, 0, 0);
    glRotatef(viewAngle, 0, 1, 0);

	glTranslatef(0, viewPos[1], 0);
	skybox.render();

    glTranslatef(0, -2 * viewPos[1], 0);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texHeightMap);

	for (int i = 0; i < texNum; i++) {
		//glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D, tex[i]);
	}

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)0);
	shader.begin();
	drawGrid();
	shader.end();
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool frustumCull(vec4 scale, vec4 offset) {
    mat4 mat0, mat1;
    glGetFloatv(GL_PROJECTION_MATRIX, &mat0[0][0]);
    glGetFloatv(GL_MODELVIEW_MATRIX, &mat1[0][0]);
    mat0 = mat0 * mat1;
	int xs, xb, ys, yb, zs, zb;
	xs = xb = ys = yb = zs = zb = 0;
	for (int i = 0; i <= 1; i++)
		for (int j = 0; j <= 1; j++) 
			for (int k = 0; k <= 1; k++) {
				vec4 pos = scale * (offset + vec4(i, geotiffFile.getMaxheight() * k, j, 1));
				pos = mat0 * pos;
				xs = (pos.x < -pos.w) ? xs + 1 : xs;
				xb = (pos.x > pos.w) ? xb + 1 : xb;
				ys = (pos.y < -pos.w) ? ys + 1 : ys;
				yb = (pos.y > pos.w) ? yb + 1 : yb;
				zs = (pos.z < -pos.w) ? zs + 1 : zs;
				zb = (pos.z > pos.w) ? zb + 1 : zb;
			}
	return xs == 8 || xb == 8 || ys == 8 || yb == 8 || zs == 8 || zb == 8;
}

void drawGrid() {
	setUniformi("texHeightMap", 0);
	for (int i = 0; i < texNum; i++)
		setUniformi(texName[i].c_str(), i + 1);
	setUniform4f("seg", geotiffFile.getMaxheight() / 5.0f, 0, 0, 0);
	setUniform4f("currentPos", currentPos.x / geotiffFile.getHeight(), 0, currentPos.y / geotiffFile.getWidth(), 0);
	//setUniform4f("tColor", tColor[6][0], tColor[6][1], tColor[6][2], 1);
    float ratio = 1;
    for (int l = 0; l < LEVEL; l++) {
		vec4 scale(ratio, 1, ratio, 1);
        setUniform4f("scale", scale.x, 1, scale.z, 1);
        //setUniform4f("tColor", tColor[l][0], tColor[l][1], tColor[l][2], tColor[l][3]);
		int c = 0;
        for (int i = -2; i < 2; i++)
            for (int j = -2; j < 2; j++) {
                if (l != LEVEL - 1 && i != -2 && i != 1 && j != -2 && j != 1)
                    continue;
                vec4 offset(i, 0, j, 0);
                setUniform4f("offset", offset.x, offset.y, offset.z, 0);
				if (frustumCull(scale, offset)) {
					c++;
					continue;
				}
                glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);
            }
		//printf("%d\n", c);
        ratio *= 0.5;
    }
}

const float delta = 10.0f;

void key_callback(GLFWwindow *windows, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		currentPos.x = (currentPos.x + delta >= geotiffFile.getHeight()) ?
			geotiffFile.getHeight() - 1 : currentPos.x + delta;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		currentPos.x = (currentPos.x - delta < 0) ?
			0 : currentPos.x - delta;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		currentPos.y = (currentPos.y + delta >= geotiffFile.getWidth()) ?
			geotiffFile.getWidth() - 1 : currentPos.y + delta;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		currentPos.y = (currentPos.y - delta < 0) ?
			0 : currentPos.y - delta;
	}
	if (action == GLFW_PRESS)
		printf("Current Position: %.0f %.0f\n", currentPos.x, currentPos.y);
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