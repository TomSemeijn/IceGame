#include "ColorFade.h"

namespace reb {

	namespace Particles {

		//sets inital color to the first in the vector when the component is added to a particle
		void ColorFade::onOwnerSet() {
			auto owner = getOwner();
			int a = owner->m_color.getA();
			owner->m_color = m_colors.front();
			owner->m_color.setA(a);
		}

		//updates the component to allow behavior to be added to the owning particle
		void ColorFade::update(float elapsed) {
			//verifies there is an owner
			if (hasOwner()) {
				//gets owner as variable
				auto owner = getOwner();

				//gets a [0 => 1] value for how far the particle is into its lifecycle
				float lifeFactor = owner->getTime() / owner->LIFETIME;

				//gets the floating point "index" of the current color
				int colorAmount = m_colors.size();
				float index = lifeFactor * ((float)colorAmount - 1.0f);

				//gets the percentage of the color at the floor and ceil index
				float ceilPercent = index - std::floor(index);
				float floorPercent = 1.0f - ceilPercent;

				auto firstColor = m_colors.data();

				//gets the final color
				int r, g, b;
				int floorIndex = (int)std::floor(index);
				int ceilIndex = (int)std::ceil(index);
				if (ceilIndex < colorAmount) {
					Color floorColor = *(firstColor + floorIndex);
					Color ceilColor = *(firstColor + ceilIndex);
					r = (int)((float)floorColor.getR() * floorPercent + (float)ceilColor.getR() * ceilPercent);
					g = (int)((float)floorColor.getG() * floorPercent + (float)ceilColor.getG() * ceilPercent);
					b = (int)((float)floorColor.getB() * floorPercent + (float)ceilColor.getB() * ceilPercent);
				}
				else {
					r = m_colors.back().getR();
					g = m_colors.back().getG();
					b = m_colors.back().getB();
				}

				//sets the color
				owner->m_color.setR(r);
				owner->m_color.setG(g);
				owner->m_color.setB(b);
			}
		}

		//returns a clone of the component
		ParticleComp* ColorFade::clone() {
			return new ColorFade(m_colors);
		}

	}

}