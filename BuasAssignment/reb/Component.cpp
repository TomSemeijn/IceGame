#include "Component.h"
#include "Container.h"

namespace reb {

	//removes the component from its owning object
	void Component::destroy() {
		if (hasOwner()) {
			getOwner()->removeComponent(this);
		}
	};

}