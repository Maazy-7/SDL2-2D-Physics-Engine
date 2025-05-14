#include <iostream>
#include <vector>
#include <stdlib.h>
#include <tuple>
#include "SDL.h"
#include "constants.h"
#include "Vector2.h"
#include "drawFunctions.h"
#include "Object.h"
#include "Spring.h"
#include "collision.h"


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool game_Is_Running = false;
bool debug = false;

float t1 = 0;

std::vector<Object*> objects;
std::vector<Spring*> springs;

int objectsTempSize = 0;


Object o1(Vector2(5, 0), 25, red,1);
bool constraint = false;
bool rot = false;
bool mousePressed = false;

Vector2 normal = Vector2();
int contCount = 1;

void addObject(Object object) 
{
	objects.push_back(&object);
}

Object* findObject(Object* object) 
{
	for (int i = 0; i < objects.size();i++) 
	{
		if (object == objects[i]) 
		{
			return objects[i];
		}
	}
}

bool initialize_window() 
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "Error initializing SDL" << std::endl;
		return false;
	}

	window = SDL_CreateWindow("Game!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	
	if (!window) 
	{
		std::cout << "Error initializing SDL window" << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	
	if (!renderer)
	{
		std::cout << "Error initializing SDL renderer" << std::endl;
		return false;
	}

	return true;

}

int s = 60;
float sqrt2 = sqrt(2);

//SPRING MASS MODEL SOFT BODY (AND IT ACTUALLY WORKS YAY!!!)
void spawnSquisy(Vector2 position,float restLength, float springConst, float damping,int n=6) 
{
	float sqrtRestLength = sqrt2 * restLength;
	Vector2 newPos = Vector2(position.x - n/2*s, position.y - n/2*s);

	std::vector<std::vector<Object*>> rows;
	

	for (int i = 0; i < n; i++) 
	{
		std::vector<Object*> columns;
		for (int j = 0; j < n; j++)
		{
			columns.push_back(new Object(Vector2(newPos.x + s * j, newPos.y + s * i), 15, red, 1,0.f,75.f));
			objects.push_back(columns.end()[-1]);
		}
		rows.push_back(columns);
	}
	
	//spawning springs
	//vertical springs
	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < n-1; j++) 
		{
			springs.push_back(new Spring(rows[j][i], rows[j+1][i], restLength, springConst, damping));
		}
	}

	//horizontal springs
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n - 1; j++)
		{
			springs.push_back(new Spring(rows[i][j], rows[i][j+1], restLength, springConst, damping));
		}
	}

	//diagonal springs starting at bottom right
	for (int i = 0; i < n-1; i++)
	{
		for (int j = 0; j < n-1; j++)
		{
			springs.push_back(new Spring(rows[i][j], rows[i+1][j+1], sqrtRestLength, springConst, damping));
		}
	}

	//diagonal springs starting at bottom left
	for (int i = n-1; i > 0; i--)
	{
		for (int j = 0; j < n - 1; j++)
		{
			springs.push_back(new Spring(rows[i][j], rows[i - 1][j + 1], sqrtRestLength, springConst, damping));
		}
	}

}

float damp = 5000;
float sprConst = 300;

Vector2 getMousePos(SDL_Event* event)
{
	return Vector2(event->motion.x - WIDTH / 2, -(event->motion.y - HEIGHT / 2));
}

bool hold = false;
Vector2 mousePos;
void process_input(float dt)
{
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) 
	{
		switch (event.type) 
		{
			case SDL_QUIT:
				game_Is_Running = false;
				break;
			
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) 
				{
					game_Is_Running = false;
				}
				if (event.key.keysym.sym == SDLK_SPACE) 
				{
					objects.push_back(new Object(Vector2(0, 0), 5, red,1));
				}
				if (event.key.keysym.sym == SDLK_RSHIFT && objects.size() - objectsTempSize > 0)
				{
					delete objects.back();
					objects.erase(objects.end()-1);
				}
				if (event.key.keysym.sym == SDLK_p) 
				{
					//objects.push_back(new Object(Vector2(-200, 500), 25, red, 1));
					//objects.push_back(new Object(Vector2(-200, 395), 75, red, 1,0.f,50));
					objects.push_back(new Object(mousePos,45,red,1,0,150));
				}
				break;
		}
		mousePos = getMousePos(&event);
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			//event.motion positions based of 0,0 being top left, subtracted WIDTH/2 and HEIGHT/2 from x and y respectively to get proper coordinates (0,0) in middle
			//objects.push_back(new Object(getMousePos(&event), 25, red, 3));
			//spawnSquisy(getMousePos(&event), s, 5000, 5000);
			mousePressed = true;
		}
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_MIDDLE)
		{
			//event.motion positions based of 0,0 being top left, subtracted WIDTH/2 and HEIGHT/2 from x and y respectively to get proper coordinates (0,0) in middle
			objects.push_back(new Object(getMousePos(&event), 25, red, 3));
		}
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
		{
			//event.motion positions based of 0,0 being top left, subtracted WIDTH/2 and HEIGHT/2 from x and y respectively to get proper coordinates (0,0) in middle
			objects.push_back(new Object(getMousePos(&event), 25, red, 4,45));
		}
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_X1)
		{
			//event.motion positions based of 0,0 being top left, subtracted WIDTH/2 and HEIGHT/2 from x and y respectively to get proper coordinates (0,0) in middle
			objects.push_back(new Object(getMousePos(&event), 25, red, 1));
		}
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			spawnSquisy(getMousePos(&event), s, 3000, 5000,5);
		}
	}
}

Object o2(Vector2(800, -150), 550, blue, 4, 45.f, 10.f, true);
Object o3(Vector2(0, -1350), 1000, green, 4, 45.f, 10.f, true);
Object o4(Vector2(-1400, 0), 1000, yellow, 4, 45.f, 10.f, true);
Object o6(Vector2(-200, 500), 150, red, 3, 0.f, 10.f, true);

//double pendulum :)
Object o7(Vector2(0, 0), 25, red, 1, 0.f, 100.f);//<- this is the swinging object in the middle
Object o8(Vector2(0, 200), 25, red, 1, 0.f, 100.f);

int xStringBall = -200;
int yStringBall = -30;

std::vector<Vector2> contactPoints;
void start() 
{
	objects.push_back(&o1);//code just doesnt work if the vector holding objects is empty, like what????
	objects.push_back(&o2);
	objects.push_back(&o3);
	objects.push_back(&o4);
	objects.push_back(&o6);
	objects.push_back(&o7);
	objects.push_back(&o8);
	objectsTempSize = objects.size();
}
int startDelay = 0;

void update(float dt)
{
	if (startDelay < 20) { startDelay++; return; }


	//accelerate
	for (int i = 1; i < objects.size(); i++)
	{
		objects[i]->accelerate(Vector2(0, -900.81f));
		//objects[i]->setAngularVelocity(180.f);
	}
	
	if (hold) 
	{
		objects[7]->position = mousePos;
	}

	//update spring forces
	for (int i = 0; i < springs.size(); i++) 
	{
		springs[i]->applySpringForce();
	}

	//update
	for (int i = 1; i < objects.size(); i++)
	{
		objects[i]->updateV(dt);
	}

	//checkCollisions
	/*for (int i = 1; i < objects.size(); i++)
	{
		if (pointCircleCollision(objects[i], mousePos) && mousePressed) 
		{
			objects[i]->position = mousePos;
		}
	}*/

	contactPoints.clear();

	for (int i = 1; i < objects.size(); i++) 
	{
		objects[i]->contactDraw = false;
	}

	for (int i = 1; i < objects.size()-1; i++)
	{
		for (int j = i+1; j < objects.size(); j++)
		{
			//circle vs other collision
			if (!objects[i]->polygon)//circle object
			{
				if (!objects[j]->polygon)//other circle object
				{
					if(circleCircleCollision(objects[i], objects[j]))
					{
						contactPoints.push_back(findCircleCircleContactPoints(objects[i], objects[j]));
					}
				}
				else //vertices > 2 meaning other polygon object
				{
					if (circlePolygonCollision(objects[i], objects[j])) 
					{
						contactPoints.push_back(findCirclePolygonContactPoints(objects[i], objects[j]));
					}
				}
			}

			//polygon vs other collision
			else //polyogn object
			{
				if (!objects[j]->polygon)//other circle object
				{
					if (circlePolygonCollision(objects[j], objects[i])) 
					{
						contactPoints.push_back(findCirclePolygonContactPoints(objects[j], objects[i]));
					}
				}
				else //vertices > 2 meaning other polygon object
				{
					if (polygonPolygonCollision(objects[i], objects[j],normal)) 
					{
						std::tuple<Vector2, Vector2> contacts = findPolygonPolygonContactPoints(objects[j],objects[i]);
						contactPoints.push_back(std::get<0>(contacts));
						if (std::get<0>(contacts) != std::get<1>(contacts)) 
						{ 
							contactPoints.push_back(std::get<1>(contacts));
							contCount = 2;
						}

						resolveCollisionRotation(objects[i], objects[j], normal, std::get<0>(contacts), std::get<1>(contacts), contCount);
						contCount = 1;

					}
				}
			}
		}
	}

	o7.applyDistanceJointConstraint(Vector2(xStringBall,yStringBall),200);
	o8.applyDistanceJointConstraint(o7.position, 200);
	

	//apply constraints
	/*for (int i = 1; i < objects.size(); i++)
	{
		objects[i]->applyCircleConstraint(Vector2(0,0), 400);
	}*/
}

void render() 
{
	SDL_SetRenderDrawColor(renderer, black, 0);
	SDL_RenderClear(renderer);
	
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);

	//drawCircle(renderer,red,0,0,400);//constraint circle
	//drawSquare(renderer,red,0,0,300);
	drawLine(renderer, red, xStringBall, yStringBall, o7.position.x, o7.position.y);
	drawLine(renderer, red, o7.position.x, o7.position.y, o8.position.x, o8.position.y);

	for (int i = 1; i < objects.size(); i++)
	{
		objects[i]->draw(renderer);
	}

	for (int i = 0; i < springs.size(); i++) 
	{
		//if (!springs[i]) { continue; }
		springs[i]->draw(renderer);
	}

	for (int i = 0; i < contactPoints.size(); i++) 
	{
		drawSquare(renderer,blue,contactPoints[i].x,contactPoints[i].y,5);
	}

	SDL_RenderPresent(renderer);
}

void destroy_window()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* argv[]) 
{
	game_Is_Running = initialize_window();
	start();

	int frameDelay = 1000 / 165;
	int frameTime;
	while (game_Is_Running) 
	{	
		Uint64 t2 = SDL_GetTicks64();
		float dt = (t2 - t1)/1000;
		t1 = t2;

		process_input(dt);

		for (int i = 0; i < 8; i++)
		{
			update(dt/8);
		}
		render();

		SDL_Delay(frameDelay);
	}

	destroy_window();


	return 0;
}