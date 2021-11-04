#include "Iceblock.h"
#include <reb/ShadedSprite.h>
#include <reb/Concave.h>
#include "ContentIndex.h"
#include "LayerIndex.h"

using namespace reb;

//constructor
Iceblock::Iceblock(Level* level, ContentLoader& loader, Vector2 pos)
	:	reb::GameObj(new Transform(true, true, true, pos), BoundType::BOUNDS_RIGIDBODY),
		m_level(level)
{
	getTransform()->setScale(BASESCALE);
	setSprite(new ShadedSprite(loader.getContent(ContentIndex::ICEBLOCK)));
	setBody(new Rigidbody(new Concave(loader.getContent(ContentIndex::ICEBLOCK)), Material(2, 0, 0, 0), 80, true));
	setLayer(LayerIndex::ICELAYER);
	getSprite()->setOpacity(0.8);
};

//updates the iceblock
void Iceblock::lateUpdate(float elapsed) {
	//updates the heat
	if (m_level != nullptr) {
		m_heat = m_level->getHeat(this);
	}
	//scales according to the heat
	float newScale = getTransform()->getScale() + m_heat * SCALETOHEAT;
	getTransform()->setScale(std::fminf(std::fmaxf(newScale, MINSCALE), MAXSCALE));

	//loses the game if too small
	if (newScale <= MINSCALE) {
		m_level->lose();
	}
}

//returns the current heat value
float Iceblock::getHeat() {
	return m_heat;
}

//returns the current scale position in a [0 => 2] range 
//where [0 => 1] = [MINSCALE => BASESCALE] and [1 => 2] = [BASESCALE =>MAXSCALE]
float Iceblock::getScaleRange() {
	//gets real scale
	float currentScale = getTransform()->getScale();

	//if the scale is not above the basescale
	if (currentScale <= BASESCALE) {
		return (currentScale - MINSCALE) / (BASESCALE - MINSCALE);
	}
	//if the scale is above the basescale
	else {
		return 1 + (currentScale - BASESCALE) / (MAXSCALE - BASESCALE);
	}
}