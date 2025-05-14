#pragma once

void putPixel(SDL_Renderer* renderer, int x, int y)
{
	SDL_RenderDrawPoint(renderer, WIDTH / 2 + x, HEIGHT / 2 - y);
}


void drawLine(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, int x1, int y1, int x2, int y2)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, 0);
	SDL_RenderDrawLine(renderer, WIDTH / 2 + x1, HEIGHT / 2 - y1, WIDTH / 2 + x2, HEIGHT / 2 - y2);
}

void drawSquare(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, float xpos, float ypos, float width)
{
	drawLine(renderer, r, g, b, xpos - width, ypos + width, xpos + width, ypos + width);
	drawLine(renderer, r, g, b, xpos + width, ypos + width, xpos + width, ypos - width);
	drawLine(renderer, r, g, b, xpos + width, ypos - width, xpos - width, ypos - width);
	drawLine(renderer, r, g, b, xpos - width, ypos - width, xpos - width, ypos + width);
}

void drawCircle(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, float x, float y, int radius,int segments = 30) 
{
	int xpos = WIDTH / 2 + x;
	int ypos = HEIGHT / 2 - y;

	float angleStep = 2 * PI / segments;

	SDL_SetRenderDrawColor(renderer, r, g, b, 0);

	int x1 = xpos + radius;
	int y1 = ypos;

	for (int i = 1; i < segments+1; i++)
	{
		float theta = i * angleStep;

		int x2 = xpos + radius * cosf(theta);
		int y2 = ypos - radius * sinf(theta);

		if (i == segments) 
		{
			SDL_RenderDrawLine(renderer, x1, y1, xpos+radius, ypos);
		}
		else
		{
			SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
		}
		x1 = x2;
		y1 = y2;
	}

}

void drawPolygon(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, std::vector<Vector2*> vertices)
{
	for (int i = 0; i<vertices.size();i++)//draws a line from one vertex to another, ternary operator to draw from last point to first point
	{
		int f = (i == vertices.size() - 1) ? 0 : i + 1;//handles index error for last iteration draws from last vertex to first one
		drawLine(renderer, r, g, b, vertices[i]->x, vertices[i]->y, vertices[f]->x, vertices[f]->y);
	}
}