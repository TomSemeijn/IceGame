#ifndef REBOOTINTERSECTIONSOLVE_H
#define REBOOTINTERSECTIONSOLVE_H

#pragma once
#include <functional>
#include <map>
#include "Rigidbody.h"

namespace reb {

	//contains an extensible static jump table to find if two shapes are intersecting
	//these functions can be more lightweight than the collision resolving ones since only a boolean needs to be returned
	//also adds some default functions to this jump table
	class IntersectionSolve {
		//typedefs
		typedef std::pair<std::type_info, std::type_info> TypePair;
		typedef std::function <bool(const Shape*, const Shape*, Transform&, Transform&, Rect<float>&)> ShapeFunction;
	private:
		//the jumptable
		static std::map<std::pair<size_t, size_t>, ShapeFunction> s_jumpTable;
	public:
		//registers an intersection function
		static void registerFunction(TypePair types, ShapeFunction function);
		//calls an intersection function
		static bool intersect(Rigidbody* a, Rigidbody* b, Rect<float>& intersection);
	private:
		//returns an intersection boolean for circle to circle
		static bool intersectCircleCircle(const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//returns an intersection boolean for circle to convex
		static bool intersectCircleConvex(const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//returns an intersection boolean for circle to concave
		static bool intersectCircleConcave(const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//returns an intersection boolean for convex to convex
		static bool intersectConvexConvex(const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//returns an intersection boolean for convex to concave
		static bool intersectConvexConcave(const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//returns an intersection boolean for concave to concave
		static bool intersectConcaveConcave(const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
	};

}

#endif