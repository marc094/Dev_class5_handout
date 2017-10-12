#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Entities.h"
#include "p2Point.h"
#include "SDL\include\SDL.h"
#include "Animation.h"
#include "p2List.h"

#define ACCELERATION 0.5f
#define DECELERATION 0.1f

class j1Player :
	public j1Module
{
public:
	enum PlayerState {
		IDLE,
		JUMPING = 0x00000001,
		FALLING = 0x00000010,
		LANDING = 0x00000100,
		RUNNING = 0x00001000,
		CHANGING_LAYER = 0x00010000
	};

	j1Player();
	~j1Player();

	void Init();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

private:
	PlayerState state = IDLE;

	uint		jump_height;
	float		scale = 1.0f;
	fPoint		position;
	fPoint		speed_vector;
	bool		flipped;

	SDL_Texture* player_texture;
	Animation* current_animation = nullptr;
	p2List<Animation*> animation_list;
	Animation* idle;
	Animation* jumping;
	Animation* falling;
	Animation* landing;
	Animation* walking;
	Animation* changing_layers;

public:
	void Move();
	void Accelerate(int x = 0, int y = 0);
	void SelectAnim(fPoint speed_vector);
};

#endif