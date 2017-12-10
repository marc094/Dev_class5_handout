#include "InterfaceElement.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Gui.h"



InterfaceElement::InterfaceElement() : enabled(true), scale(App->win->GetScale())
{
}


InterfaceElement::~InterfaceElement()
{
}


bool InterfaceElement::Enable(bool enable)
{
	this->enabled = enable;
	return this->enabled;
}

bool InterfaceElement::isEnabled()
{
	return enabled;
}

bool InterfaceElement::Start()
{
	return true;
}

bool InterfaceElement::PreUpdate()
{
	bool ret = true;

	for (p2List_item<InterfaceElement*>* current_element = elements.start;
		current_element != nullptr && ret == true;
		current_element = current_element->next)
	{
		ret = current_element->data->PreUpdate();
	}
	return ret;
}

bool InterfaceElement::PostUpdate()
{
	bool ret = true;
	ComputeAbsolutePos();
	rect.x = (-anchor_point.x * rect.w) + abs_pos.x;
	rect.y = (-anchor_point.y * rect.h) + abs_pos.y;

	for (p2List_item<InterfaceElement*>* current_element = elements.start;
		current_element != nullptr && ret == true;
		current_element = current_element->next)
	{
		ret = current_element->data->PostUpdate();
	}

	if (App->debug) {
		SDL_Rect r = content_rect;
		App->render->DrawQuad(rect, 0, 0, 255, 255, false, false);
		App->render->DrawQuad(r, 0, 0, 255, 255, false, false);
		App->render->DrawLine(r.x, r.y + r.h * anchor_point.y, r.x + r.w, r.y + r.h * anchor_point.y, 0, 0, 255, 255, false);
		App->render->DrawLine(r.x + r.w * anchor_point.x, r.y, r.x + r.w * anchor_point.x, r.y + r.h, 0, 0, 255, 255, false);
	}

	return ret;
}

bool InterfaceElement::CleanUp()
{
	return true;
}

SDL_Rect InterfaceElement::getRect() const
{
	return rect;
}

void InterfaceElement::SetContentRect(int x_margin, int y_margin)
{
	if (x_margin == INT_MAX) x_margin = (rect.w - content_rect.w) / 2;
	if (y_margin == INT_MAX) y_margin = (rect.h - content_rect.h) / 2;
	content_rect.x = rect.x + x_margin;
	content_rect.y = rect.y + y_margin;
	content_rect.w = rect.w - (2 * x_margin);
	content_rect.h = rect.h - (2 * y_margin);
}

SDL_Rect InterfaceElement::GetContentRect() const
{
	return content_rect;
}

int InterfaceElement::getPositionX() const
{
	return rel_pos.x;
}

int InterfaceElement::getPositionY() const
{
	return rel_pos.y;
}

void InterfaceElement::setPosition(int x, int y)
{
	rel_pos.x = x;
	rel_pos.y = y;
}

void InterfaceElement::setPositionX(int x)
{
	rel_pos.x = x;
}

void InterfaceElement::setPositionY(int y)
{
	rel_pos.y = y;
}

void InterfaceElement::setScale(float scale)
{
	this->scale = scale;
}

float InterfaceElement::getScale() const
{
	return scale;
}

InterfaceElement::Interfacetype InterfaceElement::getType() const
{
	return type;
}

void InterfaceElement::SetAnchor(float x, float y)
{
	anchor_point.x = x;
	anchor_point.y = y;
}

void InterfaceElement::GetAnchor(float & x, float & y) const
{
	x = anchor_point.x;
	y = anchor_point.y;
}

float InterfaceElement::GetAnchorX() const
{
	return anchor_point.x;
}

float InterfaceElement::GetAnchorY() const
{
	return anchor_point.y;
}

InterfaceElement * InterfaceElement::AddElement(InterfaceElement * elem)
{
	return elements.add(elem)->data;
}

void InterfaceElement::SetParent(InterfaceElement * parent)
{
	if (this->parent != nullptr)	// Erase the element from its previous parent if it already has one
	{
		int index = this->parent->elements.find(this);
		if (index > -1)
			this->parent->elements.del(this->parent->elements.At(index));
	}

	this->parent = parent;
	if (parent != nullptr) {
		parent->AddElement(this);
		App->gui->RemoveElement(this);
	}
	else
		App->gui->AddElement(this);
}

void InterfaceElement::SetFocus()
{
	p2List_item<InterfaceElement*>* curr = nullptr;

	if (parent == NULL)
		curr = App->gui->elements.start;
	else
		curr = parent->elements.start;

	while (curr != NULL)
	{
		curr->data->in_focus = false;
		curr = curr->next;
	}
	this->in_focus = true;
}

void InterfaceElement::ComputeAbsolutePos()
{
	if (parent != nullptr) {
		parent->SetContentRect();
		abs_pos.x = rel_pos.x + parent->content_rect.x;
		abs_pos.y = rel_pos.y + parent->content_rect.y;
	}
	else abs_pos = rel_pos;
	SetContentRect();
}
