#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Timer.h"


struct SDL_Texture;
class Window;
class Button;
class Sprite;
class Label;
class Slider;

class j1Scene : public j1Module
{
public:

	j1Scene();

	enum State {
		IN_GAME,
		IN_MENU,
		TRANSITION,
		END,
		INTRO
	}state = INTRO;

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start() override;

	// Called before all Updates
	bool PreUpdate() override;

	// Called each loop iteration
	bool Update(float dt) override;

	// Called before all Updates
	bool PostUpdate() override;

	// Called before quitting
	bool CleanUp(pugi::xml_node&) override;

	void ChangeScene(uint level);
	
	void CheckInput(float dt);

	void CheckEnd();

	bool Load(pugi::xml_node& data) override;

	bool Save(pugi::xml_node& data) const override;

	bool ret = true;

	void ChangeLifes();
	void SumCoin();

	Window* CreateSettingsWindow(int x, int y, SDL_Rect h_slider_bar, SDL_Rect h_slider_idle, SDL_Rect h_slider_hovered, SDL_Rect h_slider_pressed, SDL_Rect button_idle, SDL_Rect button_hover, SDL_Rect button_press);

	Window* credits_window = nullptr;
	Window* settings_window = nullptr;
	Slider* credits_slider = nullptr;
	Sprite* credits_text = nullptr;
	Slider* set_mus_slider = nullptr;
	bool credits_bool = false;
	bool settings_bool = false;
	uint jump_sound = 0;
	uint hit_sound = 0;
	uint change_sound = 0;
	uint level_sound = 0;
	uint coin_sound = 0;
	uint button_sound = 0;
	State current_state = state;
	float time;

	bool transitioninig = false;

private:
	uint level;
	uint max_level;
	p2DynArray<p2SString> xml_file_name;
	bool playing = false;
	SDL_Texture* menu_background = nullptr;
	SDL_Texture* title = nullptr;
	SDL_Texture* buttons = nullptr;
	SDL_Texture* credits_win = nullptr;
	SDL_Texture* settings_win = nullptr;
	SDL_Texture* credits_tex = nullptr;
	SDL_Texture* sliders = nullptr;
	SDL_Texture* logo = nullptr;
	SDL_Texture* logo_back = nullptr;
	p2List<Button*> menu_buttons;
	p2List<Sprite*> lives;
	SDL_Texture* lifes_sprite;
	j1Timer intro_time;
	Label* time_lab;
	Label* coin_lab;
	Label* firstlevel_lab;
};

void Game_start(int, ...);
void Game_continue(int, ...);
void Show_Credits(int, ...);
void Hide_Credits(int, ...);
void Hide_Settings(int, ...);
void Drag_Credits(int, ...);
void exit(int, ...);
void toMenu(int, ...);
void ShowSettings(int, ...);
void SetVolumeFX(int, ...);
void SetVolumeMusic(int, ...);
void Fps_30(int, ...);
void Fps_60(int, ...);
void Fps_Uncapped(int, ...);
void DoReload(int, ...);
void DoLoadInGame(int, ...);
void DoLoadMenu(int, ...);
void DoLoadEnd(int, ...);

#endif // __j1SCENE_H__