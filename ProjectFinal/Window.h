#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevey() { return muevey; }
	GLfloat getmuevez() { return muevez; }
	GLint getactivateCar() { return activeCar; }
	GLint getactivateTime() { return activeTime; }
	GLint getactivateDrone() { return activeDrone; }
	GLint getactivateLuces() { return activeLuces; }
	GLint getactivateEscaleras() { return activeEscaleras; }
	GLint getactivateCamara() { return activeCamara; }
	
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLboolean activeCar;
	GLboolean activeTime;
	GLboolean activeDrone;
	GLboolean activeLuces;
	GLboolean activeEscaleras;
	GLboolean activeCamara;
	GLfloat muevey;
	GLfloat muevez;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

