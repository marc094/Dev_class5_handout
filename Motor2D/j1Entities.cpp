#include "j1Entities.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"

j1Entities::j1Entities() : j1Module()
{
	name.create("entities");
}


j1Entities::~j1Entities()
{
}

bool j1Entities::Awake(pugi::xml_node& conf)
{
	bool ret = true;
	LOG("Loading Module Entities");

	pugi::xml_parse_result result = animations.load_file(conf.child("animations").child_value());

	if (result == NULL) {
		LOG("Could not load map xml file %s. pugi error: %s", conf.child("animations").child_value(), result.description());
		ret = false;
	}

	if (ret) {
		pugi::xml_node doc_node = animations.child("animations");
		ground_enemy_node = doc_node.child("Enemies").child("ground");
		boxer_enemy_node = doc_node.child("Enemies").child("boxer");
		player_node = doc_node.child("player");

		exclamation.PushBack({ 0,36,3,8 });
	}
	return ret;
}
bool j1Entities::Start()
{	
	enemy_texture = App->tex->Load(animations.first_child().child("spritesheet").attribute("path").as_string());
	

	Add_Enemy(BaseEnemy::GROUND, { 1000,1005 }, COLLIDER_FRONT_LAYER);
	Add_Enemy(BaseEnemy::BOXER, { 900,830 }, COLLIDER_BACK_LAYER);

	on_collision = false;
	return true;
}

bool j1Entities::Update(float dt)
{
	p2List_item<BaseEnemy*>* current_enemy = Enemies.start;
	while (current_enemy != NULL)
	{
		current_enemy->data->Update(dt);
		current_enemy = current_enemy->next;
	}
	return true;
}
bool j1Entities::CleanUp()
{
	App->tex->UnLoad(enemy_texture);
	enemy_texture = nullptr;
	Enemies.clear();

	return true;
}

void j1Entities::Add_Enemy(BaseEnemy::Type type, fPoint position, ColliderType layer)
{
	BaseEnemy* aux = new BaseEnemy();
	aux->position = position;
	aux->state = Entity::IDLE;
	aux->speed_vect = { 0,0 };
	aux->currentLayer = layer;

	pugi::xml_node current_node;

	if (type == BaseEnemy::GROUND)
	{
		current_node = ground_enemy_node;
	}
	else if (type == BaseEnemy::BOXER)
	{
		current_node = boxer_enemy_node;
	}

	for (pugi::xml_node animation : current_node.child("animationInfo").children()) {

		Animation aux_anim;

		aux_anim.name = animation.name();
		aux_anim.speed = animation.attribute("duration").as_float();
		aux_anim.loop = animation.attribute("loop").as_bool();

		for (pugi::xml_node frames : animation.children())
		{
			SDL_Rect aux_rect{ frames.attribute("x").as_int(), frames.attribute("y").as_int(), frames.attribute("h").as_int(), frames.attribute("h").as_int() };
			aux_anim.PushBack(aux_rect);
		}

		if (!strcmp(aux_anim.name, "idle"))
			aux->idle_anim = aux_anim;
		else if (!strcmp(aux_anim.name, "walking"))
			aux->moving_anim = aux_anim;
		else if (!strcmp(aux_anim.name, "alert"))
			aux->alert_anim = aux_anim;
	}

	

	aux->gravity = current_node.child("enemyInfo").attribute("gravity").as_bool();
	aux->current_animation = &aux->idle_anim;
	aux->collider = aux->current_animation->GetCurrentFrame().rect;

	Enemies.add(aux);
}

void j1Entities::Move(fPoint& position, fPoint& speed_vector) const {


	position.x += speed_vector.x;
	position.y += speed_vector.y;


	speed_vector.x = REDUCE_TO(speed_vector.x, 0, DECELERATION * 2);
	//speed_vector.y = REDUCE_TO(speed_vector.y, 0, DECELERATION);

}

void j1Entities::Accelerate(fPoint& speed_vector, float x, float y) const {
	speed_vector.x += (x) / (1.0f / ACCELERATION);
	speed_vector.y += (y) / (1.0f / ACCELERATION);

	//speed_vector->x = CLAMP(speed_vector.x, -5, 5);
	//speed_vector->y = CLAMP(speed_vector.y, -10, 10);

}