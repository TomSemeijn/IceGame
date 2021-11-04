#include "CollisionInfo.h"
#include "Functions.h"

namespace reb {

	//sets all the values related to the materials of the rigidbodies
	void CollisionInfo::setMaterialInfo(float elapsed) {
		//gets materials
		Material matA = a->getMaterial();
		Material matB = b->getMaterial();

		//sets restitution to the smallest of the two
		restitution = std::min(matA.restitution, matB.restitution);

		//sets friction values to the square root of the two values multiplied
		staticFriction = std::sqrt(matA.staticFriction * matB.staticFriction);
		dynamicFriction = std::sqrt(matA.dynamicFriction * matB.dynamicFriction);

		//loops through all the contact points
		for (auto& point : contactPoints) {
			// Calculate radii from COM to contact
			Vector2 radiusA = point - a->getOwner()->getTransform()->getPos();
			Vector2 radiusB = point - b->getOwner()->getTransform()->getPos();

			float relativeVelScale =	(b->getVelocity() + radiusB.leftScalarCrossProd(b->getAngularVelocity()) -
										a->getVelocity() - radiusA.leftScalarCrossProd(a->getAngularVelocity())).getLength();

			// Determine if we should perform a resting collision or not
			// The idea is if the only thing moving this object is gravity,
			// then the collision should be performed without any restitution
			if (relativeVelScale < (a->getGravity() * elapsed).getLength() + EPSILON || relativeVelScale < (b->getGravity() * elapsed).getLength() + EPSILON) {
				restitution = 0.0f;
			}
		}
	}

	//applies the impulse necessary to separate the rigidbodies
	void CollisionInfo::addImpulse(float elapsed) {
		//loops through all contact points
		for (int i = 0; i < contactPoints.size(); i++) {

			//gets the radius from the center of mass to the current contact point for both bodies
			Vector2 radiusA = contactPoints[i] - (a->getOwner()->getTransform()->getWorldSpace(a->getShape()->getCOM()));
			Vector2 radiusB = contactPoints[i] - (b->getOwner()->getTransform()->getWorldSpace(b->getShape()->getCOM()));

			//gets the relative velocity according to: V = v + w cross r
			Vector2 relativeVel =
				b->getVelocity() + radiusB.leftScalarCrossProd(b->getAngularVelocity())
				-
				a->getVelocity() - radiusA.leftScalarCrossProd(a->getAngularVelocity());

			//gets the relative velocity on the normal
			float normalVel = relativeVel.dotProduct(normal);
			//corrects normal velocity if it points in the wrong direction
			if (normalVel > 0) { continue; }
			
			//gets impulse scalar according to j = (-(1 + e)((Va - Vb) * t)) / (1 / massA + 1 / massB + (rA cross t)^2 / Ia + (rB cross t)^2 / Ib)
			float impulseScalar = -(1 + restitution) * normalVel;
			float massDenom = a->getInverseMass() + b->getInverseMass() + std::pow(radiusA.crossProd(normal), 2) * a->getInverseInertia() + std::pow(radiusB.crossProd(normal), 2) * b->getInverseInertia();
			impulseScalar /= massDenom;

			//divides impulse scalar by amount of contact points
			impulseScalar /= contactPoints.size();

			//gets final impulse vector
			Vector2 impulse = normal * impulseScalar;
			
			//applies impulse to bodies
			a->applyImpulse(impulse * -1.0f, radiusA, elapsed);
			b->applyImpulse(impulse, radiusB, elapsed);

			//-------------------------friction--------------------------------------------------
			//gets relative velocity again (now including the previous impulse)
			relativeVel =
				b->getVelocity() + radiusB.leftScalarCrossProd(b->getAngularVelocity())
				-
				a->getVelocity() - radiusA.leftScalarCrossProd(a->getAngularVelocity());

			//gets the tangental vector of the normal
			Vector2 normalTangent = relativeVel - normal * relativeVel.dotProduct(normal);
			normalTangent.setLength(1);

			//gets the friction impulse scalar
			float frictionScalar = -relativeVel.dotProduct(normalTangent);
			frictionScalar /= massDenom;

			//divides the friction impulse scalar by the amount of contact points
			frictionScalar /= contactPoints.size();
			frictionScalar = std::abs(frictionScalar);

			//skip small and nan impulses
			if (std::fabsf(frictionScalar) <= EPSILON) { continue; }

			//gets the friction impulse
			Vector2 frictionImpulse;
			//uses coulombs law of Ff <= u * Fn with a static and dynamic friction value to simulate difficulty starting a motion
			if (frictionScalar < impulseScalar * staticFriction) {
				frictionImpulse = normalTangent * frictionScalar;
			}
			else {
				frictionImpulse = normalTangent * -impulseScalar * dynamicFriction;
			}
			
			//applies friction impulse
			a->applyImpulse(frictionImpulse * -1.0f, radiusA, elapsed);
			b->applyImpulse(frictionImpulse, radiusB, elapsed);
		}
	};

	//applies positional correction
	void CollisionInfo::positionalCorrection() {
		Vector2 correction = normal * SLOPCHECK * (std::max(penetration - SLOP, 0.0f) / (a->getInverseMass() + b->getInverseMass()));
		a->getOwner()->getTransform()->move(correction * a->getInverseMass() * -1.0f);
		b->getOwner()->getTransform()->move(correction * b->getInverseMass());
	}
};