#pragma once

#include <boost/variant/variant.hpp>
#include <common/components/Shape.h>
#include <common/components/Position.h>

// 'AnyComponent' contains the same components as 'CompList'
// except it does not contain EntityID.
// It is in 'struct' to allow forward declarations.
struct AnyComponent {
	boost::variant<Shape, Position> component;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(component);
	}
};
