#ifndef __j1COLLISION__
#define __j1COLLISION__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"
#include "p2Point.h"

enum ColliderType {
	COLLIDER_PLAYER = 1,
	COLLIDER_BACK_LAYER,
	COLLIDER_FRONT_LAYER,
	COLLIDER_ENEMY,
	COLLIDER_NONE
};
struct Collider
{
	Rect rect;
	ColliderType collidertype;

};

class j1Collision :
	public j1Module
{
public:
	j1Collision();
	~j1Collision();

	void Init();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);

	// Called before the first frame
	virtual bool Start();

	// Called each loop iteration
	virtual bool PreUpdate();

	// Called each loop iteration
	virtual bool Update(float dt);

	// Called each loop iteration
	virtual bool PostUpdate();

	// Called before quitting
	virtual bool CleanUp();

	virtual bool Load(pugi::xml_node&);

	virtual bool Save(pugi::xml_node&) const;
	void Checkcollisions(ColliderType collidertype, Rect rect_frame, fPoint position , fPoint* speed_vector);
	bool CheckEnemyCollisions(const Rect collider, const  Rect entity);
private:
	void SetSpVecToCollisions(const Rect collider, const  Rect entity, fPoint &speed_vector);
	int scale;
};
#endif