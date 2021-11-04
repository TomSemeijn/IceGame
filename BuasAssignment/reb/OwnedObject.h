#ifndef REBOOTOWNEDOBJECT_H
#define REBOOTOWNEDOBJECT_H

#pragma once

namespace reb {

	//a template base class for classes that have owning objects they need access to
	template <typename OwnerType>
	class OwnedObject {

		//has the owning type as a friend so it can set itself as the owner
		friend OwnerType;

	private:
		//the owning object
		OwnerType* m_owner = nullptr;

	protected:
		//sets the owner of the object and calls onOwnerSet
		void setOwner(OwnerType* newOwner) {
			m_owner = newOwner;
			onOwnerSet();
		};

		//allows child classes to react when the owner is set
		virtual void onOwnerSet() {};

	public:
		//allows access to the owning object
		OwnerType * const getOwner()const { return m_owner; }

		//returns true if the object currently has an owner
		bool hasOwner()const { return m_owner != nullptr; }

		//removes the object from its owner
		virtual void destroy() = 0;

		//virtual destructor
		virtual ~OwnedObject() {};
	};

}

#endif