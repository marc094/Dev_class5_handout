#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"

#include "SDL/include/SDL.h"
#include "SDL_image/include/SDL_image.h"



j1Window::j1Window() : j1Module()
{
	window = NULL;
	screen_surface = NULL;
	name.create("window");
}

// Destructor
j1Window::~j1Window()
{
}
bool j1Window::Start()
{
	SDL_Surface* icon = IMG_Load("textures/icon.png");
	SDL_SetWindowIcon(window, icon);

	SDL_FreeSurface(icon);

	return true;
}
// Called before render is available
bool j1Window::Awake(pugi::xml_node& config)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_SHOWN;
		bool fullscreen = config.child("fullscreen").attribute("value").as_bool(false);
		bool borderless = config.child("borderless").attribute("value").as_bool(false);
		bool resizable = config.child("resizable").attribute("value").as_bool(false);
		bool fullscreen_window = config.child("fullscreen_window").attribute("value").as_bool(false);

		width = config.child("resolution").attribute("width").as_int(640);
		height = config.child("resolution").attribute("height").as_int(480);
		scale = config.child("resolution").attribute("scale").as_float(1.0f);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(fullscreen_window == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool j1Window::CleanUp(pugi::xml_node& config)
{
	LOG("Destroying SDL window and quitting all SDL systems");

	/*<resolution width = "1920" height = "1080" scale = "1.5f" / >
		<fullscreen value = "false" / >
		<borderless value = "true" / >
		<resizable value = "false" / >
		<fullscreen_window value = "false" / >*/
	config.child("resolution").attribute("width").set_value(width);
	config.child("resolution").attribute("height").set_value(height);
	config.child("resolution").attribute("scale").set_value(scale);

	int flags = SDL_GetWindowFlags(window);

	config.child("fullscreen").attribute("value").set_value(flags & SDL_WINDOW_FULLSCREEN);
	config.child("borderless").attribute("value").set_value(flags & SDL_WINDOW_BORDERLESS);
	config.child("resizable").attribute("value").set_value(flags & SDL_WINDOW_RESIZABLE);
	config.child("fullscreen_window").attribute("value").set_value(flags & SDL_WINDOW_FULLSCREEN_DESKTOP == SDL_WINDOW_FULLSCREEN_DESKTOP);

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void j1Window::SetTitle(const char* new_title)
{
	//title.create(new_title);
	SDL_SetWindowTitle(window, new_title);
}

void j1Window::GetWindowSize(uint& width, uint& height) const
{
	width = this->width;
	height = this->height;
}

float j1Window::GetScale() const
{
	return scale;
}