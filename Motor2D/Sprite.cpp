#include "Sprite.h"
#include "j1Render.h"

Sprite::Sprite(uint _x, uint _y, SDL_Texture* _tex, SDL_Rect _anim, bool _enabled) : InterfaceElement()
{
	type = SPRITE;
	tex = _tex;
	rel_pos.x = (int)_x;
	rel_pos.y = (int)_y;
	initial_pos = rel_pos;

	enabled = next_frame_enabled = _enabled;

	if (_anim.w != 0 && _anim.h != 0)
	{
		idle_anim = _anim;
	}
	else
	{
		idle_anim.x = 0;
		idle_anim.y = 0;
		assert(SDL_QueryTexture(tex, NULL, NULL, &idle_anim.w, &idle_anim.h) == 0);
	}

	rect.w = idle_anim.w;
	rect.h = idle_anim.h;

	content_rect = rect;

	current_anim = &idle_anim;
}


Sprite::~Sprite()
{
}

bool Sprite::Update(float dt)
{
	if (current_anim == nullptr)
		return false;

	if (culled && parent != nullptr) {
		SDL_Rect actual_anim_rect = { 0, 0, 0, 0 };
		actual_anim_rect = *current_anim;
		actual_anim_rect.x += result_rect.x - rect.x;
		actual_anim_rect.y += result_rect.y - rect.y;
		actual_anim_rect.w += result_rect.w - rect.w;
		actual_anim_rect.h += result_rect.h - rect.h;

		App->render->BlitGui(tex, result_rect.x, result_rect.y, &actual_anim_rect);
	}
	else App->render->BlitGui(tex, rect.x, rect.y, current_anim);

	bool ret = InterfaceElement::Update(dt);
	return ret;
}
