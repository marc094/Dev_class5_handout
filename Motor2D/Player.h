#pragma once
#include "Entity.h"
class Player :
	public Entity
{
public:
	Player();
	~Player();

	bool Awake();

	bool Start();
	// Called each loop iteration

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	void SelectAnim(fPoint speed_vect);

	bool isJumping() const { return is_jumping; }

	void setJumping(bool jump) { is_jumping = jump; grounded = !jump; }

	void SwapLayer();
	
	fPoint GetPosition() const { return position; }

	bool Load(pugi::xml_node& data);

	bool Save(pugi::xml_node& data) const;

	LayerID GetCurrentLayer();

	bool god_mode = false;

	uint jump_sound;
	uint hit_sound;
	uint change_sound;
	uint level_sound;

private:
	float		scale = 1.0f;
	bool		flipped = false;
	bool		is_jumping = false;
	float		aura_angle = 0.0f;

	p2List<Animation> animation_list;
	Animation jumping_anim;
	Animation falling_anim;
	Animation landing_anim;
	Animation changing_layers_anim;
	Animation god_mode_aura;

};

