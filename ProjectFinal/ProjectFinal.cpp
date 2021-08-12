/*
Semestre 2021-2
Práctica : Iluminación
Cambios en el shader, en lugar de enviar la textura en el shader de fragmentos, enviaremos el finalcolor
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"


//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture dadoTexture;
Texture pisoTexture;
Texture Tagave;


Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
//Model EdificioA_M;
//Model arbol_M;
Model Letras_M;
//Model FI_M;
//Model AreaVerde_M;
Model Drone_M;
Model Helice_M;
Model RutaDrone_M;
Model RutaCarro_M;
Model PruebaCarro_M;
Model Carro_M;
Model Carro2_M;
Model Animacion_M;
Model Cuerpo_M;
Model Brazo_M;
Model Pierna_M;
Model Humo_M;
Model Persona_M;
Model CuerpoT_M;
Model Edificio_A_M;
Model Puente_M;
Model Edificio_B_M;
Model Edificio_C_M;
Model Edificio_E_M;
Model Piso_1_M;
Model Edificio_DF_M;
Model Piso_2_M;
Model Escaleras_M;
//Model Arboles_M;
Model Otros_M;

//Variables keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}




void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};


	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}



void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//void Tiempo(float contador,Skybox sky, Skybox sky2) {
//	if () {
//		skybox = Skybox(sky);
//	}
//	else {
//		skybox = Skybox(sky2);
//	}
//
//}

///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXavion = 2.0, posYavion = 25*10.0, posZavion = 0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 6;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float giroAvion;
	float giroAvionInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 6;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void) { //tecla L

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion;
	//no volatil, agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}

void resetElements(void) //Tecla 0
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto // barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("entro aquí\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//printf("se quedó aqui\n");
			//printf("max steps: %f", i_max_steps);
			//Draw animation
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
		}
	}

	if (keys[GLFW_KEY_G])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			//printf("movAvion_x es: %f\n", movAvion_x);
			//printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona H para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_H])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			printf("Ya puedes guardar otro frame presionando la tecla G'\n");
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += 1.0f;
			printf("movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			printf("Ya puedes modificar tu variable presionando la tecla 1\n");
		}
	}

}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CreateShaders();
	float giroHelice = 0.0f;
	float offsetDrone = 1.0f;
	float posYDrone = 1.0f;
	float posXDrone = 0.0f;
	float posZDrone = 0.0f;
	float DroneT = 0.0f;
	float giroDrone = 0.0f;
	float giroCar = 0.0f;
	float posZCar = 0.0f;
	float posXCar = 0.0f;
	float posYCar = 0.0f;
	float CarT = 0.0f;
	float posZEscaleras = 0.0f;
	float posXEscaleras = 0.0f;
	float posYEscaleras= 0.0f;
	float giroEscaleras = 0.0f;
	float EscalerasT = 0.0f;
	float LuzT = 0.0f;
	float offsetHumo = 1.0f;
	float posYHumo= 1.0f;
	float giroBrazos = 0.0f;
	float giroPiernas = 0.0f;
	float tiempo = 0.0f;
	
	float posZRode= 0.0f;
	float posXRode= 0.0f;
	float posYRode = 0.0f;
	float giroRode = 0.0f;

	float posZRode2 = 0.0f;
	float posXRode2 = 0.0f;
	float giroRode2 = 0.0f;

	float posZRode3 = 0.0f;
	float posXRode3 = 0.0f;
	float giroRode3 = 0.0f;

	float posZRode4 = 0.0f;
	float posXRode4 = 0.0f;
	float giroRode4 = 0.0f;

	float posZBrazo = 0.0f;
	float posXBrazo = 0.0f;
	float posYBrazo = 0.0f;
	float giroYBrazo = 0.0f;
	float giroXBrazo = 0.0f;
	float giroZBrazo = 0.0f;
	
	

	camera = Camera(glm::vec3(55 * 10.0f, 1 * 10.0f, 2 * 10.0f), glm::vec3(0.0f,   1.0f, 0.0f), -60.0f, 0.0f, 3.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado.tga");
	dadoTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();

	Helice_M = Model();
	Helice_M.LoadModel("Models/helice.obj");
	Drone_M = Model();
	Drone_M.LoadModel("Models/drone.obj");
	RutaDrone_M = Model();
	RutaDrone_M.LoadModel("Models/RutaDrone.obj");
	RutaCarro_M = Model();
	RutaCarro_M.LoadModel("Models/RutaCarro.obj");
	PruebaCarro_M = Model();
	PruebaCarro_M.LoadModel("Models/CarPrueba.obj");
	Carro_M = Model();
	Carro_M.LoadModel("Models/CarNot.obj");
	Carro2_M = Model();
	Carro2_M.LoadModel("Models/Car1.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/rueda.obj");
	Animacion_M = Model();
	Animacion_M.LoadModel("Models/AnimacionLuces.obj");
	Cuerpo_M = Model();
	Cuerpo_M.LoadModel("Models/cuerpoCompleto.obj");
	Brazo_M = Model();
	Brazo_M.LoadModel("Models/brazo.obj");
	Pierna_M = Model();
	Pierna_M.LoadModel("Models/pierna.obj");

	Humo_M = Model();
	Humo_M.LoadModel("Models/humo.obj");

	CuerpoT_M = Model();
	CuerpoT_M.LoadModel("Models/Cuerpo.obj");

	Persona_M = Model();
	Persona_M.LoadModel("Models/Persona.obj");
	
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/Black Hawk uh-60.3ds");
	
	/*FI_M = Model();
	FI_M.LoadModel("Models/Fi_V7Proto.obj");*/
	Edificio_A_M = Model();
	Edificio_A_M.LoadModel("Models/EdificioA.obj"); 

	Puente_M = Model();
	Puente_M.LoadModel("Models/Puente.obj");

	Edificio_B_M = Model();
	Edificio_B_M.LoadModel("Models/EdificioB.obj");

	Edificio_C_M = Model();
	Edificio_C_M.LoadModel("Models/EdificioC.obj");
	Edificio_E_M = Model();
	Edificio_E_M.LoadModel("Models/EdificioE.obj");

	Piso_1_M = Model();
	Piso_1_M.LoadModel("Models/Piso1.obj");

	Edificio_DF_M = Model();
	Edificio_DF_M.LoadModel("Models/EdificioDF.obj");

	Piso_2_M = Model();
	Piso_2_M.LoadModel("Models/Piso2.obj");

	Escaleras_M = Model();
	Escaleras_M.LoadModel("Models/Escaleras.obj");

	/*Arboles_M = Model();
	Arboles_M.LoadModel("Models/Arboles.obj");*/

	Otros_M = Model();
	Otros_M.LoadModel("Models/Otros.obj");

	
	//AreaVerde_M = Model();
	//AreaVerde_M.LoadModel("Models/reaVerdeProto.obj");

	Letras_M = Model();
	Letras_M.LoadModel("Models/letras.obj");

	Dado_M = Model();
	Dado_M.LoadModel("Models/dadoanimales.obj");


	std::vector<std::string> skyboxFaces;
	std::vector<std::string> skyboxFaces2;

		skyboxFaces.push_back("Textures/Skybox/rainbow_rt.png");
		skyboxFaces.push_back("Textures/Skybox/rainbow_lf.png");
		skyboxFaces.push_back("Textures/Skybox/rainbow_dn.png");
		skyboxFaces.push_back("Textures/Skybox/rainbow_up.png");
		skyboxFaces.push_back("Textures/Skybox/rainbow_bk.png");
		skyboxFaces.push_back("Textures/Skybox/rainbow_ft.png");
	
		skyboxFaces2.push_back("Textures/Skybox/sp3_rt.png");
		skyboxFaces2.push_back("Textures/Skybox/sp3_lf.png");
		skyboxFaces2.push_back("Textures/Skybox/sp3_dn.png");
		skyboxFaces2.push_back("Textures/Skybox/sp3_up.png");
		skyboxFaces2.push_back("Textures/Skybox/sp3_bk.png");
		skyboxFaces2.push_back("Textures/Skybox/sp3_ft.png");
	
	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//posición inicial del helicóptero
	glm::vec3 posblackhawk = glm::vec3(-20.0f, 6.0f, -1.0);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		2.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	
	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz de drone
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		-20.0f, 6.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	//Show de luces

	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 2.0f,
		-20.0f, 6.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights[3] = SpotLight(1.0f, 0.0f, 0.0f,
		0.0f, 2.0f,
		-20.0f, 6.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights[4] = SpotLight(0.0f, 1.0f, 0.0f,
		0.0f, 2.0f,
		-20.0f, 6.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights[5] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 2.0f,
		-20.0f, 6.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights[6] = SpotLight(0.0f, 1.0f, 0.0f,
		0.0f, 2.0f,
		-20.0f, 6.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights[7] = SpotLight(1.0f, 0.0f, 0.0f,
		0.0f, 2.0f,
		-20.0f, 6.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);

	//Luz del drone
	glm::vec3 DroneLight = glm::vec3(35*10.0f, 23*10.0f, -20*10.0f);
	spotLights[3].SetFlash(DroneLight, glm::vec3(0.0f, -1.0, 0.0f));
	//Show de luces
	glm::vec3 ShowLF = glm::vec3(30*10.0f, 0.1*10.0f, 5*10.0f);
	glm::vec3 ShowLF2 = glm::vec3(30 * 10.0f, 0.1*10.0f, 1* 10.0f);
	glm::vec3 ShowLF3 = glm::vec3(27 * 10.0f, 0.1*10.0f, 3 * 10.0f);
	spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
	spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
	spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
	spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
	spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
	spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
	
	//KEYFRAMES DECLARADOS INICIALES

	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].giroAvion = 0;


	KeyFrame[1].movAvion_x = 10.0f;
	KeyFrame[1].movAvion_y = 20.0f;
	KeyFrame[1].giroAvion = 0;


	KeyFrame[2].movAvion_x = 20.0f;
	KeyFrame[2].movAvion_y = 0.0f;
	KeyFrame[2].giroAvion = 0;


	KeyFrame[3].movAvion_x = 30.0f;
	KeyFrame[3].movAvion_y = -20.0f;
	KeyFrame[3].giroAvion = 0;

	/*	KeyFrame[4].movAvion_x = 3.0f;
		KeyFrame[4].movAvion_y = -2.0f;
		KeyFrame[4].giroAvion = 45.0f*/;

	KeyFrame[4].movAvion_x = 30.0f;
	KeyFrame[4].movAvion_y = -20.0f;
	KeyFrame[4].giroAvion = 180.0f;

	KeyFrame[5].movAvion_x = 0.0f;
	KeyFrame[5].movAvion_y = 0.0f;
	KeyFrame[5].giroAvion = 0;
	
	printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
	printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X");
	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		//Skybox
		if (tiempo > 1000 && tiempo < 1000.1) {
			skybox = Skybox(skyboxFaces2);
			}

		if (tiempo > 2000 && tiempo < 2000.1) {
			skybox = Skybox(skyboxFaces);
			tiempo = 0.0;
		}
		//Tiempo(mainWindow.getactivateTime(), skyboxFaces, skyboxFaces2);
		
		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//luz ligada a la cámara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//Animacion Show de Luces
		if (mainWindow.getactivateLuces() == true) {
			LuzT += 0.1;
		}
		else {
			LuzT = 0.0;
		}

		//Animacion Show de Luces
		glm::vec3 ShowLF = glm::vec3(30 * 10.0f, 3*10.0f, 5 * 10.0f);
		glm::vec3 ShowLF2 = glm::vec3(30 * 10.0f, 3*10.0f, 1 * 10.0f);
		glm::vec3 ShowLF3 = glm::vec3(27 * 10.0f, 3*10.0f, 3 * 10.0f);
		if (LuzT > 0.0 && LuzT<2*10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
			
		}
		if (LuzT >2*10 && LuzT<4*10) {

			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
		}
		if (LuzT>4*10 && LuzT<6*10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));

		}
		if (LuzT > 6* 10 && LuzT < 8 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
		}
		if (LuzT > 8*10 && LuzT < 10 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));

		}
		if (LuzT > 10* 10 && LuzT < 12 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		if (LuzT > 12 * 10 && LuzT < 14 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
		}
		if (LuzT > 14*10 && LuzT < 16 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		if (LuzT > 16 * 10 && LuzT < 18 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));

		}
		if (LuzT > 18* 10 && LuzT < 20 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
		}
		if (LuzT > 20*10 && LuzT < 22 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));

		}
		if (LuzT > 22 * 10 && LuzT < 24 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		if (LuzT > 24 * 10 && LuzT < 26 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
		}
		if (LuzT > 26*10 && LuzT < 28 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		if (LuzT > 28 * 10 && LuzT < 30 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
		}
		if (LuzT > 30 * 10 && LuzT < 32 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		if (LuzT > 32 * 10 && LuzT < 34 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
		}
		if (LuzT > 34 * 10 && LuzT < 38 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		if (LuzT > 38 * 10 && LuzT < 40 * 10) {

			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
		}
		if (LuzT > 40 * 10 && LuzT < 42 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		if (LuzT > 42 * 10 && LuzT < 44 * 10) {

			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
		}
		if (LuzT > 44 * 10 && LuzT < 46 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		if (LuzT > 46 * 10 && LuzT < 48 * 10) {

			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, 1.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 0.0, -1.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(-1.0f, 0.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(1.0f, 0.0, 0.0f));
		}
		if (LuzT > 48 * 10 && LuzT < 50 * 10) {
			spotLights[2].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[3].SetFlash(ShowLF, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[4].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[5].SetFlash(ShowLF2, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[6].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));
			spotLights[7].SetFlash(ShowLF3, glm::vec3(0.0f, 1.0, 0.0f));

		}
		//modelos para guardar datos
		glm::mat4 model(1.0);
		glm::mat4 modelDO(1.0);
		glm::mat4 modelH(1.0);
		glm::mat4 modelCO(1.0);

		//Contadores globales
		giroHelice += 0.1;
		tiempo += 0.1;
		//DroneT += 0.1f;
		
		//Piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		//agregar material al plano de piso
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Animación Drone
		posYDrone = sin(offsetDrone*toRadians);
		if (mainWindow.getactivateDrone() == true) {
			DroneT += 0.1;
		}
		else {
			DroneT = 0.0;
			offsetDrone = 0.0f;
			posYDrone = 0.0f;
			posXDrone = 0.0f;
			giroDrone = 0.0;
			posZDrone = 0.0f;
			glm::vec3 DroneLight = glm::vec3(35 * 10.0f, 23 * 10.0f, -20 * 10.0f);
			spotLights[3].SetFlash(DroneLight, glm::vec3(0.0f, -1.0, 0.0f));
		}
		
		//Drone
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35*10.0f, 23*10.0f, -20*10.0f));
		modelDO = model;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(0.0f, posYDrone, 0.0f));
		
		if (DroneT > 0.01f && DroneT < 50 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			posXDrone += 0.1f;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = -20 * 10.0f;
			DroneLight.x = (35 * 10.0f) + (-1*posXDrone);
			spotLights[1].SetFlash(DroneLight,glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(0.0f, posYDrone, posXDrone));
		}
		if (DroneT > 50 * 10.0f && DroneT < 51.75* 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			giroDrone += 0.5;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = -20 * 10.0f;
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(0.0f, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 51.75 * 10.0f && DroneT < 60 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			posZDrone += 0.1;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f)+(posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 60 * 10.0f && DroneT < 61.75 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			giroDrone += 0.5;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 61.75 * 10.0f && DroneT < 95 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			posXDrone += -0.1f;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 95 * 10.0f && DroneT < 96.75 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			giroDrone += -0.5;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 96.75 * 10.0f && DroneT < 130 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			posZDrone += 0.1f;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 130 * 10.0f && DroneT < 131.75 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			giroDrone += 0.5;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 131.75 * 10.0f && DroneT < 150 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			posXDrone += -0.1f;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 150 * 10.0f && DroneT < 151.75 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			giroDrone += 0.5;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 151.75 * 10.0f && DroneT < 195 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			posZDrone += -0.1f;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 195 * 10.0f && DroneT < 196.75 * 10.0f) {
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			giroDrone += 0.5;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = (-20 * 10.0f) + (posZDrone);
			DroneLight.x = (35 * 10.0f) + (-1 * posXDrone);
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posZDrone, posYDrone, posXDrone));
			model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (DroneT > 196.75*10) {
			posXDrone = 0.0f;
			posZDrone = 0.0f;
			giroDrone = 0.0f;
			offsetDrone += 0.5f;
			posYDrone += 0.5f;
			DroneT = 0.01;
			DroneLight.y = 23 * 10.0f;
			DroneLight.z = -20 * 10.0f;
			DroneLight.x = 35 * 10.0f;
			spotLights[1].SetFlash(DroneLight, glm::vec3(0.0f, -1.0f, 0.0f));
		}
		
		//model = glm::rotate(model, giroDrone* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(0.0f, posYDrone, posXDrone));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Drone_M.RenderModel();

		//Helices Drone
		model = modelDO;
		model = glm::translate(model, glm::vec3(0.0f, 0.15*10.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-3.7f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.8f));
		modelH = model;
		model = glm::translate(model, glm::vec3(-posXDrone, posYDrone, posZDrone));
		//model = glm::translate(model, glm::vec3(0.0f, posYDrone, 0.0f));
		model = glm::rotate(model, 50*giroHelice*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_M.RenderModel();

		model = modelH;
		model = glm::translate(model, glm::vec3(7.85f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.25f));
		model = glm::translate(model, glm::vec3(-posXDrone, posYDrone, posZDrone));
		//model = glm::translate(model, glm::vec3(0.0f, posYDrone, 0.0f));
		model = glm::rotate(model, 50 * giroHelice*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(posXDrone, posYDrone, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_M.RenderModel();

		model = modelH;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 7.75f));
		model = glm::translate(model, glm::vec3(-posXDrone, posYDrone, posZDrone));
		//model = glm::translate(model, glm::vec3(0.0f, posYDrone, 0.0f));
		model = glm::rotate(model, 50 * giroHelice*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(posXDrone, posYDrone, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_M.RenderModel();

		model = modelH;
		model = glm::translate(model, glm::vec3(7.85f, 0.0f, 7.75f));
		model = glm::translate(model, glm::vec3(-posXDrone, posYDrone, posZDrone));
		//model = glm::translate(model, glm::vec3(0.0f, posYDrone, 0.0f));
		model = glm::rotate(model, 50 * giroHelice*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(posXDrone, posYDrone, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helice_M.RenderModel();

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RutaDrone_M.RenderModel();*/

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::translate(model, glm::vec3(0.f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RutaCarro_M.RenderModel();*/

		//Animación Carro
		if (mainWindow.getactivateCar() == true) {
			CarT += 0.1;
		}
		else {
			CarT = 0.0;
			giroCar = 0.0f;
			posZCar = 0.0f;
			posXCar = 0.0f;
			posYCar = 0.0f;
			posZRode = 0.0f;
			posXRode = 0.0f;
			posYRode = 0.0f;
			giroRode = 0.0f;
			posZRode2 = 0.0f;
			posXRode2 = 0.0f;
			giroRode2 = 0.0f;
			posZRode3 = 0.0f;
			posXRode3 = 0.0f;
			giroRode3 = 0.0f;
			posZRode4 = 0.0f;
			posXRode4 = 0.0f;
			giroRode4 = 0.0f;
		}
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5*10.0f, 6*10.0f,35*10.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, 180 *toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		if (CarT > 0.1f && CarT < 2* 10.0f) {
			posZCar += 0.1f;
			model = glm::translate(model, glm::vec3(0.0f, 0.0, posZCar));
		}
		/*if (CarT > 2 * 10.0f && CarT < 3.75 * 10.0f) {
			posZCar += 0.1f;
			giroCar += 0.5f;
			posXCar += 0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}*/
		if (CarT > 3.75 * 10.0f && CarT <  5* 10.0f) {
			posXCar += 0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		/*if (CarT > 5 * 10.0f && CarT < 6.75 * 10.0f) {
			posZCar += 0.1f;
			giroCar += -0.5f;
			posXCar += 0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}*/
		if (CarT > 6.75 * 10.0f && CarT <  10.75* 10.0f) {
			posZCar += 0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		/*if (CarT > 10.75* 10.0f && CarT < 12.5* 10.0f) {
			//posZCar += 0.1f;
			giroCar += -0.5f;
			//posXCar += -0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}*/
		if (CarT > 12.5 * 10.0f && CarT < 15* 10.0f) {
			posXCar += -0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (CarT > 15* 10.0f) {
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_M.RenderModel();
		 
		//Carro 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5 * 10.0f, 6 * 10.0f, 35 * 10.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		if (CarT > 2 * 10.0f && CarT < 3.75 * 10.0f) {
			posZCar += 0.1f;
			giroCar += 0.5f;
			posXCar += 0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (CarT > 5 * 10.0f && CarT < 6.75 * 10.0f) {
			posZCar += 0.1f;
			giroCar += -0.5f;
			posXCar += 0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (CarT > 10.75* 10.0f && CarT < 12.5* 10.0f) {
			//posZCar += 0.1f;
			giroCar += -0.5f;
			//posXCar += -0.1f;
			model = glm::translate(model, glm::vec3(posXCar, 0.0, posZCar));
			model = glm::rotate(model, giroCar* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro2_M.RenderModel();

		//Animacion Ruedas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.9 * 10.0f, 6.4 * 10.0f, 33.5 * 10.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		if (CarT > 0.1f && CarT < 2 * 10.0f) {
			posZRode += -0.1f;
			giroRode += -5*1.0;
			model = glm::translate(model, glm::vec3(0.0f, 0.0, posZRode));
			model = glm::rotate(model, giroRode* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			
		}
		if (CarT > 3.75 * 10.0f && CarT < 5 * 10.0f) {
			posZRode = (10 * 0.12) -(posZCar);
			posXRode = (-10 * 0.6) -(posXCar);
			giroRode += -5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode, 0.0, posZRode));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, giroRode* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 6.75 * 10.0f && CarT < 10.75* 10.0f) {
			posZRode = -(posZCar);
			posXRode = -(posXCar);
			giroRode += -5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode, 0.0, posZRode));
			model = glm::rotate(model, giroRode* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 12.5 * 10.0f && CarT < 15 * 10.0f) {
			posZRode = (10 * 0.58) - (posZCar);
			posXRode = (10 * 0.18) - (posXCar);
			giroRode += -5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode, 0.0, posZRode));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, giroRode* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 15 * 10.0f) {
			model = glm::translate(model, glm::vec3(posXRode, 0.0, posZRode));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.1* 10.0f, 6.4 * 10.0f, 33.5 * 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		if (CarT > 0.1f && CarT < 2 * 10.0f) {
			posZRode2 += 0.1f;
			giroRode2 += 5 * 1.0;
			model = glm::translate(model, glm::vec3(0.0f, 0.0, posZRode2));
			model = glm::rotate(model, giroRode2* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		if (CarT > 3.75 * 10.0f && CarT < 5 * 10.0f) {
			posZRode2 = (-10 * 0.6)+(posZCar);
			posXRode2 = (10 * 0.18)+(posXCar);
			giroRode2 += 5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode2, 0.0, posZRode2));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, giroRode2* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 6.75 * 10.0f && CarT < 10.75* 10.0f) {
			posZRode2 = (posZCar);
			posXRode2 = (posXCar);
			giroRode2 += 5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode2, 0.0, posZRode2));
			model = glm::rotate(model, giroRode2* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 12.5 * 10.0f && CarT < 15 * 10.0f) {
			posZRode2 = (-10 * 0.12)+(posZCar);
			posXRode2 = (-10 * 0.6)+(posXCar);
			giroRode2 += 5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode2, 0.0, posZRode2));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, giroRode2* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 15 * 10.0f) {
			model = glm::translate(model, glm::vec3(posXRode2, 0.0, posZRode2));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.9 * 10.0f, 6.4 * 10.0f, 36.4 * 10.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		if (CarT > 0.1f && CarT < 2 * 10.0f) {
			posZRode3 += -0.1f;
			giroRode3 += -5 * 1.0;
			model = glm::translate(model, glm::vec3(0.0f, 0.0, posZRode3));
			model = glm::rotate(model, giroRode3* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		if (CarT > 3.75 * 10.0f && CarT < 5 * 10.0f) {
			posZRode3 = (-10 * 0.55) - (posZCar);
			posXRode3 = (10 * 0.15) - (posXCar);
			giroRode3 += -5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode3, 0.0, posZRode3));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, giroRode3* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 6.75 * 10.0f && CarT < 10.75* 10.0f) {
			posZRode3 = -(posZCar);
			posXRode3 = -(posXCar);
			giroRode3 += -5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode3, 0.0, posZRode3));
			model = glm::rotate(model, giroRode3* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 12.5 * 10.0f && CarT < 15 * 10.0f) {
			posZRode3 = (-10 * 0.12) -(posZCar);
			posXRode3 = (-10 * 0.56) -(posXCar);
			giroRode3 += -5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode3, 0.0, posZRode3));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, giroRode3* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 15 * 10.0f) {
			model = glm::translate(model, glm::vec3(posXRode3, 0.0, posZRode3));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.1 * 10.0f, 6.4 * 10.0f, 36.4 * 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		if (CarT > 0.1f && CarT < 2 * 10.0f) {
			posZRode4 += 0.1f;
			giroRode4 += 5 * 1.0;
			model = glm::translate(model, glm::vec3(0.0f, 0.0, posZRode4));
			model = glm::rotate(model, giroRode2* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		if (CarT > 3.75 * 10.0f && CarT < 5 * 10.0f) {
			posZRode4 = (10 * 0.11)+(posZCar);
			posXRode4 = (-10 * 0.55)+(posXCar);
			giroRode4 += 5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode4, 0.0, posZRode4));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, giroRode4* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 6.75 * 10.0f && CarT < 10.75* 10.0f) {
			posZRode4 = (posZCar);
			posXRode4 = (posXCar);
			giroRode4 += 5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode4, 0.0, posZRode4));
			model = glm::rotate(model, giroRode4* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 12.5 * 10.0f && CarT < 15 * 10.0f) {
			posZRode4 = (10 * 0.58)+(posZCar);
			posXRode4 = (10 * 0.13)+(posXCar);
			giroRode4 += 5 * 1.0;
			model = glm::translate(model, glm::vec3(posXRode4, 0.0, posZRode4));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, giroRode4* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (CarT > 15 * 10.0f) {
			model = glm::translate(model, glm::vec3(posXRode4, 0.0, posZRode4));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();
		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Animacion_M.RenderModel();*/

		//Animación Subiendo escaleras
		if (mainWindow.getactivateEscaleras() == true) {
			EscalerasT += 0.1;
		}
		else {
			EscalerasT = 0.0;
			giroEscaleras = 0.0f;
			posZEscaleras = 0.0f;
			posXEscaleras = 0.0f;
			posYEscaleras = 0.0f;
		}
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(6.75*10.f, 6.5*10.0f, -5.5*10.0f));
		model = glm::rotate(model, 180* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		if (EscalerasT > 0.01f && EscalerasT < 2* 10.0f) {
			posXEscaleras += 0.1f;
			posYEscaleras += 0.075f;
			model = glm::translate(model, glm::vec3(posXEscaleras,posYEscaleras,0.0f));
		}
		if (EscalerasT > 2*10.0f && EscalerasT < 2.75 * 10.0f) {
			posXEscaleras += 0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, 0.0f));
		}
		if (EscalerasT > 2.75 * 10.0f && EscalerasT < 4.5 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, 0.0f));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 4.5 * 10.0f && EscalerasT < 6 * 10.0f) {
			posZEscaleras += -0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 6 * 10.0f && EscalerasT < 7.75 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 7.75 * 10.0f && EscalerasT < 10 * 10.0f) {
			posXEscaleras += -0.1f;
			posYEscaleras += 0.075f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 10 * 10.0f && EscalerasT < 11 * 10.0f) {
			posXEscaleras += -0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 11 * 10.0f && EscalerasT < 12.75 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 12.75 * 10.0f && EscalerasT < 14.25* 10.0f) {
			posZEscaleras += 0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 14.25 * 10.0f && EscalerasT < 16 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 16 * 10.0f && EscalerasT < 18.25 * 10.0f) {
			posXEscaleras += 0.1f;
			posYEscaleras += 0.075f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 18.25 * 10.0f && EscalerasT < 19.25 * 10.0f) {
			posXEscaleras += 0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 19.25 * 10.0f && EscalerasT < 21 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 21* 10.0f && EscalerasT < 22.75* 10.0f) {
			posZEscaleras += -0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 22.75 * 10.0f && EscalerasT < 24.5 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 24.5 * 10.0f && EscalerasT < 26.75* 10.0f) {
			posXEscaleras += -0.1f;
			posYEscaleras += 0.075f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 26.75 * 10.0f && EscalerasT < 27.25 * 10.0f) {
			posXEscaleras += -0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 27.25 * 10.0f && EscalerasT < 29 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 29 * 10.0f && EscalerasT < 30.25* 10.0f) {
			posZEscaleras += 0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 30.25 * 10.0f && EscalerasT < 32 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 32 * 10.0f && EscalerasT < 34.25* 10.0f) {
			posXEscaleras += 0.1f;
			posYEscaleras += 0.075f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 34.25 * 10.0f && EscalerasT < 35.25 * 10.0f) {
			posXEscaleras += 0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 35.25 * 10.0f && EscalerasT < 37 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 37 * 10.0f && EscalerasT < 38.25* 10.0f) {
			posZEscaleras += -0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 38.25 * 10.0f && EscalerasT < 40.25 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 40.25 * 10.0f && EscalerasT < 42.75* 10.0f) {
			posXEscaleras += -0.1f;
			posYEscaleras += 0.075f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 42.75 * 10.0f && EscalerasT < 43.75 * 10.0f) {
			posXEscaleras += -0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 43.75 * 10.0f && EscalerasT < 45.5 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 45.5 * 10.0f && EscalerasT < 46.75* 10.0f) {
			posZEscaleras += 0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 46.75 * 10.0f && EscalerasT < 48.5 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 48.5 * 10.0f && EscalerasT < 50.75* 10.0f) {
			posXEscaleras += 0.1f;
			posYEscaleras += 0.075f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 50.75 * 10.0f && EscalerasT < 51.75 * 10.0f) {
			posXEscaleras += 0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 51.75 * 10.0f && EscalerasT < 53.75 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 53.75 * 10.0f && EscalerasT < 55* 10.0f) {
			posZEscaleras += -0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 55 * 10.0f && EscalerasT < 56.75 * 10.0f) {
			giroEscaleras += 0.5;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 56.75 * 10.0f && EscalerasT < 59* 10.0f) {
			posXEscaleras += -0.1f;
			posYEscaleras += 0.075f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 59 * 10.0f && EscalerasT < 60 * 10.0f) {
			posXEscaleras += -0.1f;
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (EscalerasT > 60 * 10.0f) {
			model = glm::translate(model, glm::vec3(posXEscaleras, posYEscaleras, posZEscaleras));
			model = glm::rotate(model, giroEscaleras* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		
		//model = modelCO;
		//model = glm::translate(model, glm::vec3(0.f, 8.0f, -2.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Brazo_M.RenderModel();

		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.f, 10.0f, 0.0f));
		////model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Pierna_M.RenderModel();


		
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.f, 10*10.0f, 2*10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5*-10.f, 10 * 10.0f, 2* 10.0f));
		model = glm::rotate(model, 180* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3*-10.f, 13.5 * 10.0f, 2 * 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8* -10.f, 13.5 * 10.0f, 2 * 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3 * -10.f, 17 * 10.0f, 2 * 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10 * -10.f, 17 * 10.0f, 3 * 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25 * 10.0f, 6.5*10.0f, -2* 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35 * 10.0f, 6.5*10.0f, -4 * 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15 * 10.0f, 10*10.0f, -1 * 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35 * 10.0f, 10*10.0f, -5 * 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25 * 10.0f, 10 * 10.0f, -5 * 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25 * 10.0f, 10 * 10.0f, -5 * 10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();
		
		if (mainWindow.getactivateCamara() == true) {
	
		
		//Camara anclada al avatar
		giroBrazos += 0.1;
		if (giroBrazos> 60.0) {
			giroBrazos = 0;
		}
		glm::vec3 AvatarCam = camera.getCameraPosition();

		AvatarCam.x += +10.0;
		AvatarCam.y += -10.0;
		AvatarCam.z += 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(AvatarCam));
		modelCO = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoT_M.RenderModel();

		
		model = modelCO;
		model = glm::translate(model, glm::vec3(-0.5f, 8.0f, -1.75f));
		model = glm::rotate(model, giroBrazos* toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Brazo_M.RenderModel();

		model = modelCO;
		model = glm::translate(model, glm::vec3(1.0f, 8.0f, 1.75f));
		model = glm::rotate(model, giroBrazos*-1* toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Brazo_M.RenderModel();

		model = modelCO;
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.5f));
		model = glm::rotate(model, giroBrazos*-1* toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pierna_M.RenderModel();

		model = modelCO;
		model = glm::translate(model, glm::vec3(1.5f, 4.0f, -0.5f));
		model = glm::rotate(model, giroBrazos* toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pierna_M.RenderModel();
		}
		else {
			
		}

		//Animacion humo
		offsetHumo += 0.5;
		posYHumo = sin(offsetHumo*toRadians);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(34.5*10.0f, 5*10.0f, 9.5*10.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::translate(model, glm::vec3(0.0f, posYHumo, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Humo_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(27 * 10.0f, 1*10.0f, 3 * 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Persona_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Edificio_A_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Puente_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Edificio_B_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Edificio_C_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Edificio_E_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piso_1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Edificio_DF_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piso_2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Escaleras_M.RenderModel();

		//Para un mejor movimiento se comentaron los arboles
		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arboles_M.RenderModel();*/

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Otros_M.RenderModel();

		//Helicoptero
		glm::vec3 posblackhawk;
		model = glm::mat4(1.0);
		posblackhawk = glm::vec3(posXavion + movAvion_x, posYavion + movAvion_y, posZavion);
		model = glm::translate(model, posblackhawk);
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (-90 + giroAvion) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Blackhawk_M.RenderModel();
		
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Tagave.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);*/


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
