#include "j1Collision.h"
#include "SDL\include\SDL.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Entities.h"

j1Collision::j1Collision()
{
	name.create("collision");
}


j1Collision::~j1Collision()
{
}

bool j1Collision::Start() {
	scale = App->win->GetScale();
	return true;
}

bool j1Collision::PostUpdate() {
	if (App->debug) {
		BlitDebugColliders();
	}
	return true;
}

bool j1Collision::Checkcollisions(const LayerID collidertype, const iRect rect_frame, const fPoint position, fPoint& speed_vector, float delta_time) const
{
	bool grounded = false;
	p2List_item<MapLayer*>* map_layer = nullptr;
	for (map_layer = App->map->data.layers.start; map_layer != NULL; map_layer = map_layer->next)
	{
		if (map_layer->data->layer == collidertype)
			break;
	}

	iRect object_rect = rect_frame;
	object_rect.x = (int)(position.x) - (int)(App->render->camera.x);
	object_rect.y = (int)(position.y) - (int)(App->render->camera.y);
	//object_rect = object_rect * scale;

	iPoint p1(object_rect.x + object_rect.w / 2, object_rect.y + object_rect.h / 2);

	for (p2List_item<Collider*>* collider = map_layer->data->layer_colliders.start; collider != NULL; collider = collider->next)
	{
		iRect aux = collider->data->rect;
		aux.x -= (int)(App->render->camera.x * map_layer->data->parallax_speed);
		aux.y -= (int)(App->render->camera.y * map_layer->data->parallax_speed);
		//aux = aux * scale;

		iPoint p2(aux.x + aux.w / 2, aux.y + aux.h / 2);
		if (p1.DistanceNoSqrt(p2) > MAX_DISTANCE_COLLIDER_CULLING * scale)
			continue;

		SetSpVecToCollisions(aux, object_rect, speed_vector, grounded, delta_time);
	}

	return grounded;
}

void j1Collision::BlitDebugColliders() const
{
	for (p2List_item<MapLayer*>* map_layer = App->map->data.layers.start; map_layer != NULL; map_layer = map_layer->next)
	{
		if (map_layer->data->layer != App->entities->player.current_layer)
			continue;

		for (p2List_item<Collider*>* collider = map_layer->data->layer_colliders.start; collider != NULL; collider = collider->next)
		{
			iRect aux(collider->data->rect);
			App->render->DrawQuad(aux.toSDL_Rect(), 0, 255, 0, 128, map_layer->data->parallax_speed, true, true, true);
		}
	}

	for (p2List_item <BaseEnemy*>* enemy = App->entities->Enemies.start; enemy != nullptr; enemy = enemy->next)
	{
		if (enemy->data->current_layer != App->entities->player.current_layer)
			continue;

		if (enemy->data->type == BaseEnemy::Type::LARVA)
		{
			iRect aux((int)enemy->data->position.x, (int)enemy->data->position.y, enemy->data->collider.w, enemy->data->collider.h);
			App->render->DrawQuad(aux.toSDL_Rect(), 255, 0, 0, 128, App->map->data.layers[enemy->data->current_layer + 1]->parallax_speed);

			iRect aux_cube = App->entities->larva_cube.frames[App->entities->larva_cube.getFrameIndex()].rect;
			aux_cube = iRect((int)enemy->data->position.x, (int)enemy->data->position.y, aux_cube.w, aux_cube.h);
			App->render->DrawQuad(aux_cube.toSDL_Rect(), 0, 255, 0, 128, App->map->data.layers[enemy->data->current_layer + 1]->parallax_speed);
		}
		else {
			iRect aux((int)enemy->data->position.x, (int)enemy->data->position.y, enemy->data->collider.w, enemy->data->collider.h);
			App->render->DrawQuad(aux.toSDL_Rect(), 255, 0, 0, 128, 1/*App->map->data.layers[enemy->data->current_layer]->parallax_speed*/);
		}
	}

	iRect aux((int)App->entities->player.position.x, (int)App->entities->player.position.y, App->entities->player.collider.w, App->entities->player.collider.h);
	App->render->DrawQuad(aux.toSDL_Rect(), 0, 0, 255, 128, 1.0f);

}

void j1Collision::SetSpVecToCollisions(const iRect collider1, const iRect collider2, fPoint &speed_vector, bool& grounded, float delta_time) const
{
	fPoint delta_applied_spd_vec = speed_vector * delta_time;
	if (collider2.x + collider2.w + delta_applied_spd_vec.x > collider1.x && collider2.x + delta_applied_spd_vec.x < collider1.x + collider1.w
		&& collider2.y + collider2.h + delta_applied_spd_vec.y > collider1.y && collider2.y + delta_applied_spd_vec.y < collider1.y + collider1.h) //there's contact
	{
		if (collider2.x < collider1.x + collider1.w && collider2.x + collider2.w > collider1.x) //collider2 is in x-axis collision with collider1
		{
			if (collider2.y + delta_applied_spd_vec.y < collider1.y + collider1.h && collider2.y + delta_applied_spd_vec.y > collider1.y && delta_applied_spd_vec.y < 0)
			{
				speed_vector.y = 0;
			}
			else if (collider2.y + collider2.h + delta_applied_spd_vec.y > collider1.y && delta_applied_spd_vec.y >= 0)
			{
				speed_vector.y = 0;
				grounded = true;
			}
		}
		if (collider2.y < collider1.y + collider1.h && collider2.y + collider2.h > collider1.y) //collider2 is in x-axis collision with collider1
		{
			if (collider2.x + delta_applied_spd_vec.x < collider1.x + collider1.w && collider2.x + delta_applied_spd_vec.x > collider1.x && delta_applied_spd_vec.x < 0)
			{
				speed_vector.x = 0;
			}
			else if (collider2.x + collider2.w + delta_applied_spd_vec.x > collider1.x && delta_applied_spd_vec.x >= 0)
			{
				speed_vector.x = 0;
			}
		}
	}
}

bool j1Collision::DoCollide(iRect collider1, iRect collider2) const
{
	SDL_Rect result;
	if ((bool)SDL_IntersectRect(&collider1.toSDL_Rect(), &collider2.toSDL_Rect(), &result))
		return true;
	return false;
}