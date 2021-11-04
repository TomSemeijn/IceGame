#pragma once
#include <reb/Component.h>
#include <reb/Vector2.h>
#include <reb/Container.h>
#include <reb/Transform.h>

class DestroyOutBounds : public reb::Component {
private:
	reb::Rect<float> m_bounds;
public:
	//constructor
	DestroyOutBounds(const reb::Rect<float> bounds);

	//despawns the object if it's gone completely out of bounds (using the bounds from the transform)
	void update(float elapsed)override;

	//returns a copy of the component
	DestroyOutBounds* clone()const override;
};