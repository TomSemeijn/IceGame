#ifdef NDEBUG
#define assert(condition) ((void)0)
#else
#define assert(condition) /*implementation defined*/
#endif

#include "CollisionSolve.h"
#include "Functions.h"
#include "CollisionInfo.h"

namespace reb {

	//initializes the jumptable with the initial functions registered
	std::map<std::pair<size_t, size_t>, CollisionSolve::ShapeFunction> CollisionSolve::s_jumpTable{
		{{typeid(Circle).hash_code(), typeid(Circle).hash_code()}, CollisionSolve::resolveCircleCircle},
		{{typeid(Circle).hash_code(), typeid(Convex).hash_code()}, CollisionSolve::resolveCircleConvex},
		{{typeid(Convex).hash_code(), typeid(Convex).hash_code()}, CollisionSolve::resolveConvexConvex},
		{{typeid(Concave).hash_code(), typeid(Circle).hash_code()}, CollisionSolve::resolveConcaveCircle},
		{{typeid(Concave).hash_code(), typeid(Convex).hash_code()}, CollisionSolve::resolveConcaveConvex},
		{{typeid(Concave).hash_code(), typeid(Concave).hash_code()}, CollisionSolve::resolveConcaveConcave}
	};

	//checks for AABB intersection
	bool CollisionSolve::AABBIntersect(Rect<float> a, Rect<float> b, Rect<float>& intersection) {
		//checks if there is an intersection and returns false is not
		if (b.left > a.getRight() || a.left > b.getRight()) { return false; }
		if (b.top > a.getBottom() || a.top > b.getBottom()) { return false; }
		//gets the intersection area of the aabb and sets it to the referenced intersection aabb
		float left = std::max(a.left, b.left);
		float top = std::max(a.top, b.top);
		float right = std::min(a.getRight(), b.getRight());
		float bottom = std::min(a.getBottom(), b.getBottom());
		intersection = Rect<float>(left, top, right - left, bottom - top);
		//returns true
		return true;
	}

	//registers a collision resolving function
	void CollisionSolve::registerFunction(TypePair types, ShapeFunction function) {
		s_jumpTable.emplace(std::make_pair(std::make_pair(types.first.hash_code(), types.second.hash_code()), function));
	}

	//calls a collision resolution function
	CollisionInfo CollisionSolve::resolveCollision(Rigidbody* a, Rigidbody* b, Rect<float>& intersection) {
		CollisionInfo info{ a, b };
		//tries with a first
		auto iter = s_jumpTable.find({ typeid(*a->getShape()).hash_code(), typeid(*b->getShape()).hash_code() });
		if (iter != s_jumpTable.end()) {
			return (iter->second)(
				info,
				a->getShape(),
				b->getShape(),
				*a->getOwner()->getTransform(),
				*b->getOwner()->getTransform(),
				intersection
				);
		}
		else {
			//tries with b first
			auto reversedInfo =
				s_jumpTable.at({
					typeid(*(b->getShape())).hash_code(),
					typeid(*(a->getShape())).hash_code() }
					)(
						info,
						b->getShape(),
						a->getShape(),
						*b->getOwner()->getTransform(),
						*a->getOwner()->getTransform(),
						intersection
						);
			reversedInfo.normal *= -1.0f;
			return reversedInfo;
		}
	}

	//resolves circle to circle collision
	CollisionInfo CollisionSolve::resolveCircleCircle(CollisionInfo& info, const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {

		//gets cast pointers to the shapes of the rigidbodies
		Circle* const shapeA = (Circle*)a;
		Circle* const shapeB = (Circle*)b;

		//gets the radii of the circles
		float radiusA = shapeA->getRadius() * transA.getScale();
		float radiusB = shapeB->getRadius() * transB.getScale();
		//gets the center positions of the circles
		Vector2 centerA = transA.getWorldSpace(shapeA->getCenter());
		Vector2 centerB = transB.getWorldSpace(shapeB->getCenter());

		//gets the vector of the line between the centers of the two circles (which would be the normal of a collision if there is one)
		Vector2 normal = centerB - centerA;
		//gets the distance
		float distance = normal.getLength();
		//gets the sum of the two radii
		float sumRadius = radiusA + radiusB;

		//checks if there is a collision
		if (distance < sumRadius) {
			//if the circles have the same center
			if (distance == 0.0f) {
				//sets penetration to the radius of a
				info.penetration = radiusA;

				//sets the normal to (1, 0) because that might as well be the default direction in this rare case
				info.normal = Vector2(1, 0);

				//sets the contact point to the center of a
				info.contactPoints.push_back(centerA);
			}
			//if the circles have a different center
			else {
				//sets the penetration value
				info.penetration = sumRadius - distance;

				//sets the normal value to a normalized version of the previously found vector
				info.normal = normal / distance;

				//sets the contactPoint of the collision
				info.contactPoints.push_back(info.normal * radiusA + centerA);
			}
		}

		//returns the collisionInfo object
		return info;
	}

	//resolves circle to Convex collision
	CollisionInfo CollisionSolve::resolveCircleConvex(CollisionInfo& info, const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {

		//gets cast pointers to the shapes of the rigidbodies
		Circle* const shapeA = (Circle*)a;
		Convex* const shapeB = (Convex*)b;

		//gets const references to the vertices and normals of the Convex
		const std::vector<Vector2>& vertices = shapeB->getVertices();
		const std::vector<Vector2>& normals = shapeB->getNormals();

		//gets the center of the circle relative to the Convex's local space
		Vector2 circleWorld = transA.getWorldSpace(shapeA->getCenter());
		Vector2 circleCenter = transB.getLocalSpace(circleWorld);
		float circleRadiusWorld = shapeA->getRadius() * transA.getScale();
		float circleRadiusLocalB = circleRadiusWorld / transB.getScale();

		//initializes variables for closest edge
		float separation;
		Vector2 closestEdge1;
		Vector2 closestEdge2;
		Vector2 closestEdgeNormal;
		bool started = false;
		//loops through all vertices of the Convex to find the closest edge
		for (int vertIndex = 0; vertIndex < vertices.size(); vertIndex++) {
			//calculates separation with edge
			float currentSeparation = normals[vertIndex].dotProduct(circleCenter - vertices[vertIndex]);
			//quits if separation is more than the radius of the circle (in which case the edge is not intersecting with the cicle)
			if (currentSeparation > circleRadiusLocalB) {
				return info;
			}
			//checks if separation is higher than the current one (or this is the first intersecting edge found)
			if (!started || currentSeparation > separation) {
				//sets edge information
				separation = currentSeparation;
				closestEdge1 = vertices[vertIndex];
				closestEdge2 = vertices[(vertIndex + 1 < vertices.size() ? vertIndex + 1 : 0)];
				closestEdgeNormal = normals[vertIndex];
				//sets started boolean to true
				started = true;
			}
		}
		//quits if no intersecting edge was found
		if (!started) {
			return info;
		}

		//writeLn("closest edge ", closestEdge1, " ", closestEdge2);
		//keeps a Vector2(0, 0) for normal rotations
		Vector2 origin(0, 0);
		//if the circle's center is not within the Convex
		if (!(separation < 0)) {

			//sets penetration to the radius of the circle minus the separation
			info.penetration = circleRadiusLocalB - separation;

			//finds vonoroi region
			float vertex1 = (circleCenter - closestEdge1).dotProduct(closestEdge2 - closestEdge1);
			float vertex2 = (circleCenter - closestEdge2).dotProduct(closestEdge1 - closestEdge2);
			//closest to vertex1 region
			if (vertex1 <= 0) {

				//if the circle is close enough to have an intersection
				if ((circleCenter - closestEdge1).getLength() < circleRadiusLocalB) {

					//finds normal as a vector from the vertex to the middle of the circle
					Vector2 collisionNormal = closestEdge1 - circleCenter;
					//sets the normal in worldspace to info
					info.normal = transB.getWorldNormal(collisionNormal).setLength(1);

					//turns the vertex to worldspace
					closestEdge1 = transB.getWorldSpace(closestEdge1);
					//sets the vertex as the contact point
					info.contactPoints.push_back(closestEdge1);

				}

			}//closest to vertex2 region
			else if (vertex2 <= 0) {

				//if the circle is close enough to have an intersection
				if ((circleCenter - closestEdge2).getLength() < circleRadiusLocalB) {

					//finds normal as a vector from the vertex to the middle of the circle
					Vector2 collisionNormal = closestEdge2 - circleCenter;
					//sets the normal in worldspace to info
					info.normal = transB.getWorldNormal(collisionNormal).setLength(1);

					//turns the vertex to worldspace
					closestEdge2 = transB.getWorldSpace(closestEdge2);
					//sets the vertex as the contact point
					info.contactPoints.push_back(closestEdge2);

				}

			}//closest to edge region
			else {
				//if the circle is close enough to have an intersection
				if ((circleCenter - closestEdge1).dotProduct(closestEdgeNormal) < circleRadiusLocalB) {

					//sets the world space edge normal as the normal in the collision info
					info.normal = transB.getWorldNormal(closestEdgeNormal) * -1.0f;

					//sets the contact point using the found normal
					info.contactPoints.push_back(info.normal * circleRadiusWorld + circleWorld);
				}

			}
		}
		//if the circle's center is within the Convex
		else {

			//sets the world space edge normal as the normal in the collision info
			info.normal = transB.getWorldNormal(closestEdgeNormal) * -1.0f;

			//sets the contact point using the found normal
			info.contactPoints.push_back(info.normal * circleRadiusWorld + circleWorld);

			//sets the penetration as the radius of the circle
			info.penetration = circleRadiusWorld;
		}
		//returns collisionInfo
		return info;
	}

	//finds index and penetration of the axis with the least penetration between both Convexs
	int CollisionSolve::findLeastPenetrating(float& penetration, Convex* a, Convex* b, Transform& transA, Transform& transB) {
		//prepares intial values
		penetration = 0;
		int axisIndex = -1;
		Vector2 origin(0, 0);

		float rotationA = transA.getRotation();
		float rotationB = transB.getRotation();
		auto& verticesA = a->getVertices();
		auto& verticesB = b->getVertices();
		auto& normalsA = a->getNormals();

		size_t vertAmountA = verticesA.size();
		size_t vertAmountB = verticesB.size();
		const Vector2* firstB = verticesB.data();
		const Vector2* firstA = verticesA.data();
		const Vector2* firstNormA = normalsA.data();

		//loops through all edges of a
		for (int k = 0; k < vertAmountA; k++) {
			//gets the inverted normal of the current edge of a in local space of b
			Vector2 edgeNormal = transB.getLocalNormal(transA.getWorldNormal(*(firstNormA + k))) * -1.0f;
			//gets support point for the normal in b
			int supportIndex = -1;
			float projection = 0;
			for (int i = 0; i < vertAmountB; i++) {
				float currentProjection = (firstB + i)->dotProduct(edgeNormal);
				if (currentProjection > projection || supportIndex == -1) {
					projection = currentProjection;
					supportIndex = i;
				}
			}
			//gets current vertex of a in local space of b
			Vector2 vertA = transB.getLocalSpace(transA.getWorldSpace(*(firstA + k)));
			//gets penetration distance
			float penDist = edgeNormal.dotProduct(vertA - *(firstB + supportIndex));
			//sets new penetration distance if its bigger than the last or there hasnt been one set yet
			if (penDist > penetration || axisIndex == -1) {
				penetration = penDist;
				axisIndex = k;
				//early out when a positive penetration was found
				if (penetration > 0) { return axisIndex; }
			}
		}
		//writeLn("penetration: ", penetration);

		//returns the found axis index
		return axisIndex;
	}

	//from https://github.com/tutsplus/ImpulseEngine/blob/master/Collision.cpp 
	//(slightly edited to match this program and be more readable)
	//used to clip the line segment formed by the referenced edge1 and edge2 to only include points within
	//the upper and lower limits set by the upper and lower vertex of another face along the given normal
	int CollisionSolve::clip(Vector2 normal, float c, Vector2& edge1, Vector2& edge2) {

		int sp = 0;
		Vector2 out[2] = {
		  edge1,
		  edge2
		};

		// Retrieve distances from each endpoint to the line
		// d = ax + by - c
		float d1 = normal.dotProduct(edge1) - c;
		float d2 = normal.dotProduct(edge2) - c;

		// If negative (behind plane) clip
		if (d1 <= 0.0f) out[sp++] = edge1;
		if (d2 <= 0.0f) out[sp++] = edge2;

		// If the points are on different sides of the plane
		if (d1 * d2 < 0.0f) // less than to ignore -0.0f
		{
			// Push interesection point
			float alpha = d1 / (d1 - d2);
			out[sp] = edge1 + (edge2 - edge1) * alpha;
			++sp;
		}

		// Assign our new converted values
		edge1 = out[0];
		edge2 = out[1];

		assert(sp != 3);

		return sp;
	}

	//resolves Convex to Convex collision
	CollisionInfo CollisionSolve::resolveConvexConvex(CollisionInfo& info, const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {

		//gets a cast pointer to the shapes of the rigidbodies
		Convex* const shapeA = (Convex*)a;
		Convex* const shapeB = (Convex*)b;

		//finds axes of least penetration
		float penetrationA;
		int axisA = findLeastPenetrating(penetrationA, shapeA, shapeB, transA, transB);
		if (penetrationA >= 0) { return info; }

		float penetrationB;
		int axisB = findLeastPenetrating(penetrationB, shapeB, shapeA, transB, transA);
		if (penetrationB >= 0) { return info; }

		//differentiates between reference and incident shapes
		Convex* reference;
		Convex* incident;
		bool incidentIsB;
		int referenceIndex;
		if (penetrationA > penetrationB * 0.95f + penetrationA * 0.01f) {
			reference = shapeA;
			incident = shapeB;
			incidentIsB = true;
			referenceIndex = axisA;
		}
		else {
			reference = shapeB;
			incident = shapeA;
			incidentIsB = false;
			referenceIndex = axisB;
		}
		Transform& referenceTrans = (incidentIsB ? transA : transB);
		Transform& incidentTrans = (incidentIsB ? transB : transA);

		//gets the normal of the reference edge in the local space of the incident shape
		const std::vector<Vector2>& incidentNormals = incident->getNormals();
		const std::vector<Vector2>& referenceNormals = reference->getNormals();
		Vector2 refNormal = incidentTrans.getLocalNormal(referenceTrans.getWorldNormal(referenceNormals[referenceIndex]));

		//finds the edge of the incident shape with the normal most opposite to that of the reference edge (this would be the incident edge)
		int incidentIndex = -1;
		float opposite = 0;
		size_t incidentSize = incidentNormals.size();
		auto firstIncidentNormal = incidentNormals.data();
		for (int k = 0; k < incidentSize; k++) {
			float currentDot = refNormal.dotProduct(*(firstIncidentNormal + k));
			if (opposite > currentDot || incidentIndex == -1) {
				incidentIndex = k;
				opposite = currentDot;
			}
		}

		//gets vertices of the incident and reference edges
		const std::vector<Vector2>& incidentVerts = incident->getVertices();
		Vector2 incidentEdge1 = incidentVerts[incidentIndex];
		Vector2 incidentEdge2 = incidentVerts[(incidentIndex + 1 < incidentVerts.size() ? incidentIndex + 1 : 0)];
		const std::vector<Vector2>& referenceVerts = reference->getVertices();
		Vector2 referenceEdge1 = referenceVerts[referenceIndex];
		Vector2 referenceEdge2 = referenceVerts[(referenceIndex + 1 < referenceVerts.size() ? referenceIndex + 1 : 0)];

		//puts the found vertices in worldspace
		if (incidentIsB) {
			incidentEdge1 = transB.getWorldSpace(incidentEdge1);
			incidentEdge2 = transB.getWorldSpace(incidentEdge2);
			referenceEdge1 = transA.getWorldSpace(referenceEdge1);
			referenceEdge2 = transA.getWorldSpace(referenceEdge2);
		}
		else {
			incidentEdge1 = transA.getWorldSpace(incidentEdge1);
			incidentEdge2 = transA.getWorldSpace(incidentEdge2);
			referenceEdge1 = transB.getWorldSpace(referenceEdge1);
			referenceEdge2 = transB.getWorldSpace(referenceEdge2);
		}

		//writeLn("Intersection:");
		//writeLn("Incident: A = ", incidentEdge1, ", B = ", incidentEdge2);
		//writeLn("Reference: A = ", referenceEdge1, ", B = ", referenceEdge2);
		//writeLn();

		//gets the world space normal of the reference edge
		Vector2 planeNormal = (referenceEdge2 - referenceEdge1).setLength(1);
		Vector2 referenceNormal = referenceTrans.getWorldNormal(referenceNormals[referenceIndex]);

		//gets the top and bottom limits of the reference face
		float refC = referenceNormal.dotProduct(referenceEdge1);
		float top = -(planeNormal.dotProduct(referenceEdge1));
		float bottom = planeNormal.dotProduct(referenceEdge2);

		//clips the incident edge to only include points within range of the reference edge
		if (clip(planeNormal * -1.0f, top, incidentEdge1, incidentEdge2) < 2) { return info; }
		if (clip(planeNormal, bottom, incidentEdge1, incidentEdge2) < 2) { return info; }

		//sets the collison normal (flips if Convex B is the reference shape)
		info.normal = (incidentIsB ? referenceNormal : (referenceNormal * -1.0f));

		//sets contact points to those of the incident edge that are behind the reference edge
		int contactAmount = 0;
		float separation = referenceNormal.dotProduct(incidentEdge1) - refC;
		if (separation <= 0) {
			info.contactPoints.push_back(incidentEdge1);
			info.penetration = -separation;
			contactAmount++;
		}
		separation = referenceNormal.dotProduct(incidentEdge2) - refC;
		if (separation <= 0) {
			info.contactPoints.push_back(incidentEdge2);
			info.penetration += -separation;
			contactAmount++;
			info.penetration /= contactAmount;
		}

		//returns collisionInfo
		return info;
	}

	//resolves concave to circle collision
	CollisionInfo CollisionSolve::resolveConcaveCircle(CollisionInfo& info, const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets the cast concave shape
		Concave* concave = (Concave*)a;
		Rect<float> dummy{};

		//gets the collision info object with the lowest penetration
		for (auto& conv : concave->getConvexes()) {
			if (AABBIntersect(conv->getAABB(transA), intersection, dummy)) {
				CollisionInfo currentInfo{ info.b, info.a };
				resolveCircleConvex(currentInfo, b, conv, transB, transA, dummy);
				/*
				if (currentInfo.contactPoints.size() > 0 && (currentInfo.penetration > info.penetration)) {
					info.penetration = currentInfo.penetration;
					info.normal = currentInfo.normal;
					info.contactPoints = currentInfo.contactPoints;
				}*/
				if (currentInfo.contactPoints.size() > 0) {
					info.normal += currentInfo.normal * -1.0f * currentInfo.penetration;
					info.penetration += currentInfo.penetration;
					info.contactPoints.insert(info.contactPoints.end(), currentInfo.contactPoints.begin(), currentInfo.contactPoints.end());
				}
			}
		}
		if (info.contactPoints.size() > 0) {
			info.normal.setLength(1);
			info.penetration /= info.contactPoints.size();
		}

		return info;
	}

	//resolves concave to convex collision
	CollisionInfo CollisionSolve::resolveConcaveConvex(CollisionInfo& info, const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets the cast concave shape
		Concave* concave = (Concave*)a;
		Rect<float> dummy{};
		//gets the collision info object with the lowest penetration
		for (auto& conv : concave->getConvexes()) {
			if (AABBIntersect(conv->getAABB(transA), intersection, dummy)) {
				CollisionInfo currentInfo{ info.a, info.b };
				resolveConvexConvex(currentInfo, conv, b, transA, transB, dummy);
				/*
				if (currentInfo.contactPoints.size() > 0 && (currentInfo.penetration > info.penetration)) {
					info.penetration = currentInfo.penetration;
					info.normal = currentInfo.normal;
					info.contactPoints = currentInfo.contactPoints;
				}*/
				if (currentInfo.contactPoints.size() > 0) {
					info.normal += currentInfo.normal * currentInfo.penetration;
					info.penetration += currentInfo.penetration;
					info.contactPoints.insert(info.contactPoints.end(), currentInfo.contactPoints.begin(), currentInfo.contactPoints.end());
				}
			}
		}
		if (info.contactPoints.size() > 0) {
			info.normal.setLength(1);
			info.penetration /= info.contactPoints.size();
		}
		return info;
	}

	//resolves concave to concave collision
	CollisionInfo CollisionSolve::resolveConcaveConcave(CollisionInfo& info, const Shape* a, const Shape* b, Transform& transA, Transform& transB, Rect<float>& intersection) {
		//gets the cast concave shapes
		Concave* concaveA = (Concave*)a;
		Concave* concaveB = (Concave*)b;
		Rect<float> dummy{};

		int infoAmount = 0;

		//gets the collision info object with the lowest penetration
		for (auto& conv : concaveA->getConvexes()) {
			if (AABBIntersect(conv->getAABB(transA), intersection, dummy)) {
				for (auto& otherConv : concaveB->getConvexes()) {
					if (AABBIntersect(otherConv->getAABB(transB), intersection, dummy)) {
						CollisionInfo currentInfo{ info.a, info.b };
						resolveConvexConvex(currentInfo, conv, otherConv, transA, transB, dummy);
						/*
						if (currentInfo.contactPoints.size() > 0 && (currentInfo.penetration > info.penetration || info.contactPoints.size() == 0)) {
							info.normal = currentInfo.normal;
							info.penetration = currentInfo.penetration;
							info.contactPoints = currentInfo.contactPoints;
						}
						*/
						if (currentInfo.contactPoints.size() > 0) {
							info.normal += currentInfo.normal * currentInfo.penetration;
							info.penetration += currentInfo.penetration;
							infoAmount++;
							info.contactPoints.insert(info.contactPoints.end(), currentInfo.contactPoints.begin(), currentInfo.contactPoints.end());
						}
					}
				}
			}
		}
		if (info.contactPoints.size() > 0) {
			info.normal.setLength(1);
			info.penetration /= infoAmount;
		}
		return info;
	}

}