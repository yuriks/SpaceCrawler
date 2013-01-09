#pragma once
#include "Fixed.hpp"
#include "vec2.hpp"

typedef fixed24_8 PositionFixed;

struct Position {
	PositionFixed x, y;
};

inline Position mPosition(const PositionFixed x, const PositionFixed y) {
	Position pos = {x, y};
	return pos;
}

inline vec2 operator -(const Position a, const Position b) {
	return mvec2((a.x - b.x).toFloat(), (a.y - b.y).toFloat());
}

inline Position& operator +=(Position& p, const vec2 v) {
	p.x += PositionFixed(v.x);
	p.y += PositionFixed(v.y);
	return p;
}

inline Position& operator -=(Position& p, const vec2 v) {
	p.x -= PositionFixed(v.x);
	p.y -= PositionFixed(v.y);
	return p;
}

inline Position operator +(const Position p, const vec2 v) {
	return mPosition(p.x + PositionFixed(v.x), p.y + PositionFixed(v.y));
}

inline Position operator -(const Position p, const vec2 v) {
	return mPosition(p.x - PositionFixed(v.x), p.y - PositionFixed(v.y));
}
