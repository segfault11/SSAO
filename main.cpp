//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <GL/GLEW.h>
#include "Model.h"
#include "SSAORenderer.h"
#include "Timer.h"
#include <iostream>
//-----------------------------------------------------------------------------
static Model *gsModel;
static SSAORenderer *gsRenderer;
static GL::Camera *gsCamera;
static bool gsIsFreeView = false;
static float gsDx = 0.00f, gsDy = 0.0f;
static float gsElapsed;
//-----------------------------------------------------------------------------
#define WIDTH 1280
#define HEIGHT 800
//-----------------------------------------------------------------------------
static void rotateCamera (float xrel, float yrel)
{
	static float dMax = 5.0f;
	
	if (xrel > dMax)
	{
		xrel = dMax;
	}
	if (xrel < -dMax)
	{
		xrel = -dMax;
	}
	if (yrel > dMax)
	{
		yrel = dMax;
	}
	if (yrel < -dMax)
	{
		yrel = -dMax;
	}

	float ang = 30.0f/180.0f*M_PI;
	float upAng = xrel/dMax*ang*gsElapsed/1000.0f;
	float vAng = ang*yrel/dMax*1.0f/gsElapsed;
	gsCamera->Rotate(-upAng, -0.0f);
	gsRenderer->SetCamera(*gsCamera);
}
//-----------------------------------------------------------------------------
static void init () 
{
	gsModel = new Model("happybuddha.obj");
	gsRenderer = new SSAORenderer(*gsModel, WIDTH, HEIGHT);
	
	gsCamera = new GL::Camera
	(
		GL::Vector3f(0.0f, 0.0f, -1.1f),
		GL::Vector3f(0.0f, 0.0f, 1.0f),
		GL::Vector3f(0.0f, 0.1f, 0.0f),
		1280.0f/800.0f,
		60.0f,
		0.1f,
		10.0f
	);

	gsRenderer->SetCamera(*gsCamera);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}
//-----------------------------------------------------------------------------
static void draw ()
{
	gsRenderer->Draw();
	glFlush();
}
//-----------------------------------------------------------------------------
int main(int argc, const char *argv[])
{
    SDL_Window* mainWindow;
    SDL_GLContext mainContext;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mainWindow = SDL_CreateWindow
	(
		"SSAO", 
		SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
		1280, 
		800, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);
   
	mainContext = SDL_GL_CreateContext(mainWindow);
	SDL_GL_SetSwapInterval(1);
	
    glewExperimental = GL_TRUE; 

    if (GLEW_OK != glewInit())
    {
        printf("glewInit failed\n");
    }
    else
    {
        printf("glewInit succeded\n");
    }

	init();
	bool running = true;
	SDL_Event event;
	Timer t;
	t.Start();

	while (running)
	{

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_LEFT:
							gsCamera->Rotate(0.1f, 0.00f);
							gsRenderer->SetCamera(*gsCamera);
							break;
						case SDLK_RIGHT:
							gsCamera->Rotate(-0.01f, 0.00f);
							gsRenderer->SetCamera(*gsCamera);
							break;
						case 'f':
							gsIsFreeView = gsIsFreeView == true ? false : true;	
							if (gsIsFreeView == true)
							{
								SDL_ShowCursor(SDL_DISABLE);
								SDL_WarpMouseInWindow(mainWindow, 1280/2, 800/2);
							}
							else
							{
								SDL_ShowCursor(SDL_ENABLE);
							}
							break;
					}
					break;
		//		case SDL_KEYUP:
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_MOUSEBUTTONDOWN:
					//std::cout << event.motion.x << " " << event.motion.y << std::endl;
					break;
				case SDL_MOUSEMOTION:

					if (gsIsFreeView == true)
					{
						//std::cout << event.motion.x - WIDTH/2 << " " << event.motion.y - HEIGHT/2 << std::endl; 
						float dx = event.motion.x - WIDTH/2;
						float dy = event.motion.y - HEIGHT/2;	
						gsDx = dx == 0 ? gsDx : dx;
						SDL_WarpMouseInWindow(mainWindow, WIDTH/2, HEIGHT/2);
						//std::cout << event.motion.xrel << " " << event.motion.yrel << std::endl;
					}
					break;
			}

		}
		draw();
		SDL_GL_SwapWindow(mainWindow);
	
		rotateCamera(gsDx, 0.0f);


		//t.Stop();
		//gsElapsed = t.GetElapsed();
		//t.DumpElapsed();
	}	
	
	SDL_GL_DeleteContext(mainContext);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return 0;
}
//-----------------------------------------------------------------------------
