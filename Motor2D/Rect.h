#ifndef __RECT_H__
#define __RECT_H__

template<class TYPE>
struct Rect {
	TYPE x, y, w, h;

	SDL_Rect toSDL_Rect() {
		return { (int)x, (int)y, (int)w, (int)h };
	}

	Rect(const Rect &rect) {
		x = rect.x;
		y = rect.y;
		w = rect.w;
		h = rect.h;
	}

	Rect(const SDL_Rect &rect) {
		x = rect.x;
		y = rect.y;
		w = rect.w;
		h = rect.h;
	}

	Rect operator *(int n) const {
		Rect<TYPE> r(x * n, y * n, w * n, h * n);
		return r;
	}

	Rect operator *(float n) const {
		Rect<TYPE> r((TYPE)(x * n), (TYPE)(y * n), (TYPE)(w * n), (TYPE)(h * n));
		return r;
	}

	Rect operator +(const Rect<TYPE>& n) const {
		Rect<TYPE> r(x + n.x, y + n.y, w + n.w, h + n.h);
		return r;
	}

	Rect operator +(const p2Point<TYPE>& n) const {
		Rect<TYPE> r(x + n.x, y + n.y, w, h);
		return r;
	}

	Rect operator -(const p2Point<TYPE>& n) const {
		Rect<TYPE> r(x - n.x, y - n.y, w, h);
		return r;
	}

	Rect() {}
	Rect(TYPE x, TYPE y, TYPE w, TYPE h) : x(x), y(y), w(w), h(h) {}
	~Rect() {}
};
typedef Rect<int> iRect;
typedef Rect<float> fRect;

#endif