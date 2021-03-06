#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <chrono>

using namespace std::chrono;

//Screen dimension constants
int w;
int h;

// !
int originX;
int originY;
float pixelDensity;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

int map(int n, int a1, int a2, int b1, int b2) {
	return (n - a1) * (b2 - b1) / (a2 - a1) + b1;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );


				SDL_GetRendererOutputSize(gRenderer, &w, &h);

				originX = floor(2 * w / 3);
				originY = floor(h / 2);
				pixelDensity = 4.0 / w;

				printf( "window dimensions: %d, %d\n", w, h );
				printf( "origin: %d, %d\n", originX, originY );
				printf( "pixelDensity: %e\n", pixelDensity );
			}
		}
	}

	return success;
}

void close()
{
	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void draw() {
	float ca; // c = ca + cb * i
	float cb;
	float za; // z = za + zb * i
	float zb;
	float zaa; // to store za while it's being reassigned
	int n;
	int brightness;
	const int MAX_ITERATIONS = 100;

	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			ca = (x - originX) * pixelDensity;
			cb = (y - originY) * pixelDensity;
			za = ca;
			zb = cb;
			n = 0;

			for (int k = 0; k < MAX_ITERATIONS; k++) {
				zaa = za;
				za = za * za - zb * zb + ca;
				zb = 2 * zaa * zb + cb;

				if (za * za + zb * zb > 4) {
					break;
				}

				n++;
			}

			if (n == MAX_ITERATIONS) {
				brightness = 0;
			}
			else {
				brightness = map(n, 0, MAX_ITERATIONS, 0, 255);
			}

			SDL_SetRenderDrawColor(gRenderer, brightness, brightness, brightness, 255);
			SDL_RenderDrawPoint(gRenderer, x, y);
		}
	}

	//Update screen
	SDL_RenderPresent(gRenderer);
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		draw();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>( t2 - t1 ).count();

    printf("duration: %d\n", duration / 1000);

		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent( &e ) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				}
				else if (e.type == SDL_MOUSEWHEEL) {
					originX -= floor(e.wheel.x * pixelDensity * 10000);
					originY += floor(e.wheel.y * pixelDensity * 10000);

					draw();
				}
				else if (e.type == SDL_MULTIGESTURE) {
					printf("%i, %f\n", e.mgesture.numFingers, e.mgesture.dDist);
				}

			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
