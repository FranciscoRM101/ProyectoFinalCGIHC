/*
Práctica 7: Iluminación 1 
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
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture octaedroTexture;


Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;


Model llanta;
Model coche;
Model ojos;
Model Parrilla1;
Model Parrilla2;
Model Parrilla3;
Model Parrilla4;
Model cofre;

Model cactus;

Model luminaria;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight pointLights2[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights2[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
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
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CrearOctaedro()
{

	unsigned int octaedro_indices[] = {
		// Caras superiores
		0,  1,  2,    // Cara 1
		3,  4,  5,    // Cara 2
		6,  7,  8,    // Cara 3
		9, 10, 11,    // Cara 4

		// Caras inferiores
		12, 13, 14,   // Cara 5
		15, 16, 17,   // Cara 6
		18, 19, 20,   // Cara 7
		21, 22, 23    // Cara 8
	};


	// Cada vertice: x, y, z, S, T, NX, NY, NZ


	GLfloat octaedro_vertices[] = {
		// ----- Caras superiores -----
		// Cara 1 top - derecha - frente
		 0.0f,  0.5f,  0.0f,   0.49f, 0.74f,    -0.577f,  -0.577f,  -0.577f,  //0
		 0.5f,  0.0f,  0.0f,   0.25f, 0.51f,    -0.577f,  -0.577f,  -0.577f,  //1
		 0.0f,  0.0f,  0.5f,   0.02f, 0.74f,    -0.577f,  -0.577f,  -0.577f,  //2

		 // Cara 2 top - frente - izquierda
		  0.0f,  0.5f,  0.0f,   0.52f, 0.26f,   0.577f,  -0.577f,  -0.577f,  //3
		  0.0f,  0.0f,  0.5f,   0.74f, 0.49f,   0.577f,  -0.577f,  -0.577f,  //4
		 -0.5f,  0.0f,  0.0f,   0.97f, 0.26f,   0.577f,  -0.577f,  -0.577f,  //5

		 // Cara 3 top - izquierda - atrás
		  0.0f,  0.5f,  0.0f,   0.49f, 0.27f,   0.577f,  -0.577f, 0.577f,  //6
		 -0.5f,  0.0f,  0.0f,   0.27f, 0.49f,   0.577f,  -0.577f, 0.577f,  //7
		  0.0f,  0.0f, -0.5f,   0.73f, 0.49f,   0.577f,  -0.577f, 0.577f,  //8

		  // Cara 4 top - atrás - derecha
		   0.0f,  0.5f,  0.0f,   0.48f, 0.76f,    -0.577f,  -0.577f, 0.577f,  //9
		   0.0f,  0.0f, -0.5f,   0.03f, 0.76f,    -0.577f,  -0.577f, 0.577f,  //10
		   0.5f,  0.0f,  0.0f,   0.25f, 0.98f,    -0.577f,  -0.577f, 0.577f,  //11

		   // ----- Caras inferiores -----
		   // Cara 5 bottom - frente - derecha
			0.0f, -0.5f,  0.0f,   0.51f, 0.74f,    -0.577f, 0.577f,  -0.577f,  //12
			0.0f,  0.0f,  0.5f,   0.97f, 0.74f,    -0.577f, 0.577f,  -0.577f,  //13
			0.5f,  0.0f,  0.0f,   0.75f, 0.51f,    -0.577f, 0.577f,  -0.577f,  //14

			// Cara 6 bottom - izquierda - frente
			 0.0f, -0.5f,  0.0f,   0.48f, 0.26f,   0.577f, 0.577f,  -0.577f,  //15
			-0.5f,  0.0f,  0.0f,   0.03f, 0.26f,   0.577f, 0.577f,  -0.577f,  //16
			 0.0f,  0.0f,  0.5f,   0.25f, 0.49f,   0.577f, 0.577f,  -0.577f,  //17

			 // Cara 7 bottom - atrás - izquierda
			  0.0f, -0.5f,  0.0f,   0.48f, 0.24f,   0.577f, 0.577f, 0.577f,  //18
			  0.0f,  0.0f, -0.5f,   0.25f, 0.02f,   0.577f, 0.577f, 0.577f,  //19
			 -0.5f,  0.0f,  0.0f,   0.03f, 0.24f,   0.577f, 0.577f, 0.577f,  //20

			 // Cara 8 bottom - derecha - atrás
			  0.0f, -0.5f,  0.0f,   0.5f, 0.74f,    -0.577f, 0.577f, 0.577f,  //21
			  0.5f,  0.0f,  0.0f,   0.73f, 0.51f,    -0.577f, 0.577f, 0.577f,  //22
			  0.0f,  0.0f, -0.5f,   0.27f, 0.51f,    -0.577f, 0.577f, 0.577f   //23
	};


	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(octaedro_vertices, octaedro_indices,
		24 * 8,
		24);
	meshList.push_back(octaedro);
}



void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	CrearOctaedro();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	octaedroTexture = Texture("Textures/octaedro.png");
	octaedroTexture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	

	coche = Model();
	coche.LoadModel("Models/carrop06.obj");


	llanta = Model();
	llanta.LoadModel("Models/llantap06.obj");

	ojos = Model();
	ojos.LoadModel("Models/ojoscars.obj");

	Parrilla1 = Model();
	Parrilla1.LoadModel("Models/parrillaa.obj");

	Parrilla2 = Model();
	Parrilla2.LoadModel("Models/parrilla2.obj");

	Parrilla3 = Model();
	Parrilla3.LoadModel("Models/parrilla3.obj");

	Parrilla4 = Model();
	Parrilla4.LoadModel("Models/parrilla4.obj");

	cofre = Model();
	cofre.LoadModel("Models/cofrep08obj.obj");

	
	luminaria = Model();
	luminaria.LoadModel("Models/luminaria1.obj");

	cactus = Model();
	cactus.LoadModel("Models/cactus.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	// contador luces tipo spot
	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	// Ejercicio 1
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		10.0f, 10.0f, 5.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.01f,
		15.0f);
	spotLightCount++;

	// Ejercicio 2 luz de adelante del coche
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		10.0f, 10.0f, 5.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.01f,
		25.0f);
	spotLightCount++;

	// Arreglo 2 para la luz trasera
	spotLights2[0] = spotLights[0];
	spotLights2[1] = spotLights[1];


	


	// Ejercicio 2 luz de atras del coche
	spotLights2[2] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		10.0f, 10.0f, 5.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.01f,
		25.0f);


	



	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Declaración de primer arreglo de luces puntuales
	//verde
	pointLights[0] = PointLight(0.0f, 1.0f, 0.0f,
		0.01f, 0.01f,
		0.0f, -1.0f, 10.0f,
		0.3f, 0.3f, 0.3f);
	pointLightCount++;
	
	// Ejercicio 3
	//// luz cactus
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 2.0f, 10.0f,
		1.0f, 0.0f, 0.2f);
	pointLightCount++;
	

	//Segundo arreglo  de luces puntuales blanca, verde, azul, roja
	//pointLights2[0] = pointLights[3];
	
	
	


	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
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

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		spotLights2[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);

		// Luz prendida
		if (mainWindow.getPrendia()) {
			shaderList[0].SetPointLights(pointLights, pointLightCount);
			printf("prendido\n");

		}
		else { // Luz apagada
			shaderList[0].SetPointLights(pointLights, pointLightCount-1);
			printf("apagado\n");
		}

		// luz deantera
		if (mainWindow.getlucesspot() < 0.5f) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
			//printf("primer arreglo de luces\n");
		}
		else { // luz trasera
			shaderList[0].SetSpotLights(spotLights2, spotLightCount);
			//printf("segundo arreglo de luces\n");
		}
	

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//// Octaedro
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(2.0f, 2.5f, -2.0f));
		//model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//octaedroTexture.UseTexture();
		//meshList[4]->RenderMesh();
	
		

		// luminaria
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(16.0f, 13.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.5f, 15.5f, 10.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		luminaria.RenderModel();


		
		// Coche base
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.30f, -0.5f));
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 0.5f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		coche.RenderModel();


		// Para que la luz tenga la posicion del coche
		spotLights[2].SetPos(glm::vec3(modelaux[3]) + glm::vec3(10.0f, -4.0f, 0.6f));
		spotLights2[2].SetPos(glm::vec3(modelaux[3]) + glm::vec3(-12.0f, -4.0f, 0.6f));


		// Llanta 1
		model = modelaux;
		model = glm::translate(model, glm::vec3(-6.2f, -2.2f, -2.8f));
		model = glm::scale(model, glm::vec3(4.2f, 4.2f, 4.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		llanta.RenderModel();


		// Llanta 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(-6.2f, -2.2f, 4.5f));
		model = glm::scale(model, glm::vec3(4.2f, 4.2f, 4.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		llanta.RenderModel();


		// Llanta 3
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.4f, -2.2f, -2.8f));
		model = glm::scale(model, glm::vec3(4.2f, 4.2f, 4.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		llanta.RenderModel();

		// Llanta 4
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.4f, -2.2f, 4.5f));
		model = glm::scale(model, glm::vec3(4.2f, 4.2f, 4.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		llanta.RenderModel();


		// Ojos
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.4f, 2.0f, 0.8f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		ojos.RenderModel();


		// Parrilla
		model = modelaux;
		model = glm::translate(model, glm::vec3(11.5f, -0.5f, 0.8f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Parrilla1.RenderModel();


		// Parrilla 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(11.2f, -1.85f, 0.8f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Parrilla2.RenderModel();


		// Parrilla 3
		model = modelaux;
		model = glm::translate(model, glm::vec3(10.5f, -1.85f, -2.3f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Parrilla3.RenderModel();


		// Parrilla 4
		model = modelaux;
		model = glm::translate(model, glm::vec3(10.5f, -1.85f, 3.8f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Parrilla4.RenderModel();




		// Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(5.4f, 1.45f, 0.8f));   
		model = glm::rotate(model, glm::radians(0.0f + mainWindow.getrotar()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		cofre.RenderModel();
		
		// Posicion de la luz en el cofre
		spotLights[1].SetPos(glm::vec3(
			model * glm::vec4(3.0f, 0.0f, 0.0f, 1.0f) 
		));
	
		spotLights2[1].SetPos(glm::vec3(
			model* glm::vec4(3.0f, 0.0f, 0.0f, 1.0f)
		));


		// cactus
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 10.0f));
		model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cactus.RenderModel();



		////Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//

		////blending: transparencia o traslucidez
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//AgaveTexture.UseTexture();
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh();
		//glDisable(GL_BLEND);

		glUseProgram(0);




		mainWindow.swapBuffers();
	}

	return 0;
}
