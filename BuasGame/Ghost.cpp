#include "Ghost.h"
#include <reb/ShadedSprite.h>
#include <reb/Concave.h>
#include "GhostAnimator.h"
#include "ContentIndex.h"

//constructor
Ghost::Ghost(reb::Scene& scene, reb::ContentLoader& content, reb::Vector2 pos, sf::RenderTarget& window, reb::Rect<float> sceneBounds)
	:
	reb::GameObj(new reb::Transform(true, false, false, pos), reb::GameObj::BoundType::BOUNDS_SPRITE), 
	m_window{ window },
	m_realPos{ pos }{
	//sets layer
	setLayer(reb::LayerIndex::GHOSTLAYER);

	//sets sprite
	setSprite(new reb::ShadedSprite(content.getContent(reb::ContentIndex::GHOST), reb::Vector2()));
	((reb::ShadedSprite*)getSprite())->setAlpha(5);

	//sets body
	setBody(new reb::Rigidbody(new reb::Concave(content.getContent(reb::ContentIndex::GHOST)), reb::Material(1, 0, 1, 0.9), 0, true));

	//adds camera
	m_camera = (Camera*)scene.addGameObj(new Camera(window));
	m_camera->setFollow(this);
	m_camera->setBounds(sceneBounds);

	//adds animator
	reb::Animator* ghostAnim = scene.addAnimator(new GhostAnimator(content));
	ghostAnim->addObject(this);

	//adds keypressed events for moving the player
	{
		scene.getSceneContext().addEvent(sf::Event::KeyPressed, new reb::Event(
			[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::W;
		},
			[this](sf::Event&, float) {
			holdUp = true;
		}
		));

		scene.getSceneContext().addEvent(sf::Event::KeyPressed, new reb::Event(
			[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::A;
		},
			[this](sf::Event&, float) {
			holdLeft = true;
		}
		));

		scene.getSceneContext().addEvent(sf::Event::KeyPressed, new reb::Event(
			[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::S;
		},
			[this](sf::Event&, float) {
			holdDown = true;
		}
		));

		scene.getSceneContext().addEvent(sf::Event::KeyPressed, new reb::Event(
			[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::D;
		},
			[this](sf::Event&, float) {
			holdRight = true;
		}
		));
	}

	//adds keyreleased events for moving the player
	{
		scene.getSceneContext().addEvent(sf::Event::KeyReleased, new reb::Event(
			[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::W;
		},
			[this](sf::Event&, float) {
			holdUp = false;
		}
		));

		scene.getSceneContext().addEvent(sf::Event::KeyReleased, new reb::Event(
			[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::A;
		},
			[this](sf::Event&, float) {
			holdLeft = false;
		}
		));

		scene.getSceneContext().addEvent(sf::Event::KeyReleased, new reb::Event(
			[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::S;
		},
			[this](sf::Event&, float) {
			holdDown = false;
		}
		));

		scene.getSceneContext().addEvent(sf::Event::KeyReleased, new reb::Event(
			[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::D;
		},
			[this](sf::Event&, float) {
			holdRight = false;
		}
		));
	}
}

//sets the position back to normal before any real updating takes place
void Ghost::earlyUpdate(float elapsed) {
	getTransform()->setPos(m_realPos);
}

//updates the camera
void Ghost::lateUpdate(float elapsed) {
	//keeps the old position
	m_realPos = getTransform()->getPos();

	//gets and copies the center of the ghost
	reb::Vector2 center = getTransform()->getCenter();
	reb::Vector2 newCenter = center;

	//makes the center fully divisible by the window size on each axis 
	//to avoid (most) floating point errors during fragmentation
	reb::Vector2 size = reb::Vector2(m_window.getSize().x, m_window.getSize().y);
	newCenter.compProduct(size);
	newCenter.x = std::round(newCenter.x);
	newCenter.y = std::round(newCenter.y);
	newCenter.compDiv(size);

	//sets the position to match this center
	getTransform()->move(newCenter - center);

	//updates the camera
	m_camera->updateCamera(elapsed);

}

//updates the movement and pushing status
void Ghost::update(float elapsed) {
	reb::Vector2 newSpeed{};
	if (holdLeft && !holdRight) {
		getTransform()->setFlipX(false);
		newSpeed.x = -1;
	}
	else if (holdRight && !holdLeft) {
		getTransform()->setFlipX(true);
		newSpeed.x = 1;
	}
	if (holdUp && !holdDown) {
		newSpeed.y = -1;
	}
	else if (holdDown && !holdUp) {
		newSpeed.y = 1;
	}
	if (newSpeed.getLength() > 0) {
		getBody()->setVelocity(newSpeed.setLength(speed));
	}
	else {
		getBody()->setVelocity(newSpeed);
	}
	pushing = false;
};

//makes the ghost only collide with ice
bool Ghost::canCollide(Container* other) {
	return other->getLayer() == reb::LayerIndex::ICELAYER || other->getLayer() == reb::LayerIndex::ICETERRAIN || !other->getBody()->SOLID;
};

//reacts when the ghost is pushing something
void Ghost::onTouch(Container* other, reb::Vector2 contactPoint, reb::Vector2 contactNormal) {
	auto bounds = getTransform()->getBounds();
	reb::Vector2 localContact = getTransform()->getLocalSpace(contactPoint);

	reb::Vector2 center{ bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f };
	reb::Vector2 halfCenter = center * 0.25f;

	reb::Vector2 a1{ bounds.left, bounds.top };
	reb::Vector2 b1{ bounds.left + bounds.width, bounds.top + bounds.height };

	reb::Vector2 a2{ bounds.left, bounds.top + bounds.height };
	reb::Vector2 b2{ bounds.left + bounds.width, bounds.top };

	a1.y += halfCenter.y;
	a2.y -= halfCenter.y;
	b1.y -= halfCenter.y;
	b2.y += halfCenter.y;

	b1 -= a1;
	b2 -= a2;

	float cross1 = b1.crossProd(localContact - a1);
	float cross2 = b2.crossProd(localContact - a2);

	if ((cross1 > 0 && cross2 < 0) || (cross1 < 0 && cross2 > 0)) {
		if (holdLeft || holdRight) {
			pushing = true;
		}
	}
};