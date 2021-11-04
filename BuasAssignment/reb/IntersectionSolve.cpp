#include "IntersectionSolve.h"
#include "Functions.h"
#include "Circle.h"
#include "Convex.h"
#include "Concave.h"

namespace reb {

	//initializes the jumptable with the initial functions registered
	std::map<std::pair<size_t, size_t>, IntersectionSolve::ShapeFunction> IntersectionSolve::s_jumpTable{
		{{typeid(Circle).hash_code(), typeid(Circle).hash_code()}, IntersectionSolve::intersectCircleCircle},
		{{typeid(Circle).hash_code(), typeid(Convex).hash_code()}, IntersectionSolve::intersectCircleConvex},
		{{typeid(Convex).hash_code(), typeid(Convex).hash_code()}, IntersectionSolve::intersectConvexConvex},
		{{typeid(Circle).hash_code(), typeid(Concave).hash_code()}, IntersectionSolve::intersectCircleConcave},
		{{typeid(Convex).hash_code(), typeid(Concave).hash_code()}, IntersectionSolve::intersectConvexConcave},
		{{typeid(Concave).hash_code(), typeid(Concave).hash_code()}, IntersectionSolve::intersectConcaveConcave}
	};

	//registers an intersection function
	void IntersectionSolve::registerFunction(TypePair types, ShapeFunction function) {
		s_jumpTable.emplace(std::make_pair(std::make_pair(types.first.hash_code(), types.second.hash_code()), function));
	}

	//calls an intersection function
	bool IntersectionSolve::intersect(Rigidbody* a, Rigidbody* b, Rect<float>& intersection) {
		try {
			//tries with a first
			return
				s_jumpTable.at({
					typeid(*(a->getShape())).hash_code(),
					typeid(*(b->getShape())).hash_code() }
					)(
						a->getShape(),
						b->getShape(),
						*a->getOwner()->getTransform(),
						*b->getOwner()->getTransform(),
						intersection
						);
		}
		//catches out of range exception
		catch (std::out_of_range) {
			//tries with b first
			return
				s_jumpTable.at({
					typeid(*(b->getShape())).hash_code(),
					typeid(*(a->getShape())).hash_code() }
					)(
						b->getShape(),
						a->getShape(),
						*b->getOwner()->getTransform(),
						*a->getOwner()->getTransform(),
						intersection
						);
		}
	}

	//returns an intersection boolean for circle to circle
	bool IntersectionSolve::intersectCircleCircle(const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets cast shapes
		Circle* shapeA = (Circle*)a;
		Circle* shapeB = (Circle*)b;

		//gets world space center of both circles
		Vector2 worldCenterA = transA.getWorldSpace(shapeA->getCenter());

		Vector2 worldCenterB = transB.getWorldSpace(shapeB->getCenter());

		//returns true if the circles are close enough
		return (worldCenterA - worldCenterB).getLength() <= shapeA->getRadius() + shapeB->getRadius();
	}

	//returns an intersection boolean for circle to convex
	bool IntersectionSolve::intersectCircleConvex(const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets cast shapes
		Circle* shapeA = (Circle*)a;
		Convex* shapeB = (Convex*)b;

		//returns false if the convex doesnt intersect the intersection AABB
		if (!shapeB->intersectsAABB(intersection, transB)) { return false; }

		//gets the center of the circle in the local space of the convex
		Vector2 circleCenter = transB.getLocalSpace(transA.getWorldSpace(shapeA->getCenter()));
		
		//checks if any of the edges of the convex intersect with the circle
		const std::vector<Vector2>& verticesB = shapeB->getVertices();
		for (auto vert = verticesB.begin(); vert < verticesB.end(); vert++) {
			auto next = (vert + 1 < verticesB.end() ? vert + 1 : verticesB.begin());

			//gets the squared length of the current edge
			float sqLineLen = std::powf((*next - *vert).getLength(), 2.0f);
			//skips if the line has a length of zero
			if (sqLineLen == 0) { continue; }

			//projects the center of the circle to the current edge
			const float t = std::max(0.0f, std::min(1.0f, (circleCenter - *vert).dotProduct(*next - *vert) / sqLineLen));
			const Vector2 projection = *vert + (*next - *vert) * t;

			//gets the distance between the center of the circle and the projection to the edge
			float dist = (projection - circleCenter).getLength();
			
			//returns true if the distance between the center of the circle and the current edge is less than the radius of the circle
			if (dist < shapeA->getRadius()) { return true; }
		}

		//returns true if the center of the circle is within the convex
		return shapeB->withinShape(circleCenter);
	}

	//returns an intersection boolean for circle to concave
	bool IntersectionSolve::intersectCircleConcave(const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets cast shape of B
		Concave* shapeB = (Concave*)b;

		//returns true if the circle intersects with any of the convexes within the concave
		const std::vector<Convex*>& convexesB = shapeB->getConvexes();
		for (auto& conv : convexesB) {
			if (intersectCircleConvex(a, conv, transA, transB, intersection)) { return true; }
		}

		//returns false if no intersection was found
		return false;
	}

	//returns an intersection boolean for convex to convex
	bool IntersectionSolve::intersectConvexConvex(const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets cast shapes
		Convex* shapeA = (Convex*)a;
		Convex* shapeB = (Convex*)b;

		//returns false if either convex doesnt intersect with the intersection AABB
		if (!shapeA->intersectsAABB(intersection, transA) || !shapeB->intersectsAABB(intersection, transB)) { return false; }

		//gets references to vertices of a and b
		const std::vector<Vector2>& vertsA = shapeA->getVertices();
		const std::vector<Vector2>& vertsB = shapeB->getVertices();

		//continues with pointers to the shape with the lowest amount of vertices and the shape with the most
		bool leastIsA = vertsA.size() < vertsB.size();
		Convex* leastVerts = (leastIsA ? shapeA : shapeB);
		Convex* mostVerts = (leastIsA ? shapeB : shapeA);

		//also gets the transforms in terms of least and most
		Transform& leastTrans = (leastIsA ? transA : transB);
		Transform& mostTrans = (leastIsA ? transB : transA);

		//gets a reference to the vertices of the shape with the most and a copy of those of the shape with the least
		std::vector<Vector2> leastVertices{ leastVerts->getVertices() };
		const std::vector<Vector2>& mostVertices = mostVerts->getVertices();

		//puts the shape with the least vertices into the local space of the most
		Vector2 leastPos = leastTrans.getPos();
		Vector2 leastCenter = leastTrans.getCenter();
		float leastRotation = leastTrans.getRotation();
		Vector2 mostPos = mostTrans.getPos();
		Vector2 mostCenter = mostTrans.getCenter();
		Vector2 mostWorldCenter = mostCenter + mostPos;
		float invMostRotation = -mostTrans.getRotation();
		for (auto& vert : leastVertices) {
			vert = mostTrans.getLocalSpace(leastTrans.getWorldSpace(vert));
		}

		//puts the intersection AABB into the local space of the convex with the most vertices
		intersection = intersection - mostPos;

		//checks for edge intersection
		for (auto mostVert = mostVertices.begin(); mostVert < mostVertices.end(); mostVert++) {
			auto mostNext = (mostVert + 1 < mostVertices.end() ? mostVert + 1 : mostVertices.begin());

			for (auto leastVert = leastVertices.begin(); leastVert < leastVertices.end(); leastVert++) {
				auto leastNext = (leastVert + 1 < leastVertices.end() ? leastVert + 1 : leastVertices.begin());

				//returns true if the lines intersect
				if (linesIntersect(*mostVert, *mostNext, *leastVert, *leastNext)) { return true; }

			}
		}
		
		//returns true if any vertex of either shape is within the other to cover a fully submerged intersection
		if (mostVerts->withinShape(leastVertices.front())) { return true; }

		//puts a vertex of the shape with the most vertices in the local space of the one with the least to check the same
		Vector2 anyMostVert = leastTrans.getLocalSpace(mostTrans.getWorldSpace(mostVertices.front()));
		if (leastVerts->withinShape(anyMostVert)) { return true; }

		//returns false because no intersection was found
		return false;
	}

	//returns an intersection boolean for convex to concave
	bool IntersectionSolve::intersectConvexConcave(const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets cast shape of B
		Concave* shapeB = (Concave*)b;

		//returns true if any convex within the concave intersects with the convex
		const std::vector<Convex*> convexesB = shapeB->getConvexes();
		for (auto& conv : convexesB) {
			//creates a copy of the intersection rectangle because the convex vs convex check modifies it
			Rect<float> intersectCopy{ intersection };
			if (intersectConvexConvex(a, conv, transA, transB, intersectCopy)) { return true; }
		}

		//returns false because no intersection was found
		return false;
	}

	//returns an intersection boolean for concave to concave
	bool IntersectionSolve::intersectConcaveConcave(const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets cast shapes
		Concave* shapeA = (Concave*)a;
		Concave* shapeB = (Concave*)b;

		//returns true if any convex of a intersects with a convex of b
		const std::vector<Convex*> convexesA = shapeA->getConvexes();
		const std::vector<Convex*> convexesB = shapeB->getConvexes();
		for (auto& convA : convexesA) {
			for (auto& convB : convexesB) {
				//creates a copy of the intersection rectangle because the convex vs convex check modifies it
				Rect<float> intersectCopy{ intersection };
				if (intersectConvexConvex(convA, convB, transA, transB, intersectCopy)) { return true; }
			}
		}

		//returns false because no intersection was found
		return false;
	}

}