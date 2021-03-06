#include <sstream>
#include <type_traits>

#include <boost/variant/variant.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/boost_variant.hpp>

#include "common/Connection.h"
#include "common/components/Position.h"
#include "doctest.h"

#include <entityplus/entity.h>

static_assert(!std::is_copy_constructible<Connection>::value, "Should it be?");
static_assert(!std::is_copy_assignable<Connection>::value, "Should it be?");


TEST_CASE("Serialization") {
	std::stringstream ss; // any stream can be used
	{
	    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

	    Position pos;
	    pos.angularSpeed = 1;
	    pos.rotation = 4;
	    pos.center = {1, 2};
	    pos.speed = {-1, 0};
	    oarchive(pos); // Write the data to the archive
	  } // archive goes out of scope, ensuring all contents are flushed

	  {
	    cereal::BinaryInputArchive iarchive(ss); // Create an input archive

	    Position pos;
	    iarchive(pos); // Read the data from the archive
	    CHECK(pos.angularSpeed == 1);
	    CHECK((pos.speed == geometry::Vector{-1, 0}));
	  }
}

#if 0
/**
 * Empty serialization for empty structures
 */
template<typename Archive, typename T>
auto serialize(Archive &, T & x) -> typename std::enable_if<std::is_empty<T>::value>::type
{

}
#endif
struct S{};
struct T{};

TEST_CASE("Serialization of variant  with empty struct") {
	using V = boost::variant<S, T>;

	V v(S{});
	std::stringstream ss;
	cereal::BinaryOutputArchive oarchive(ss);
	oarchive(v);
}

TEST_CASE("Entity management") {
	using namespace entityplus;

	using entity_manager_t = entity_manager<component_list<int>, tag_list<>>;
	using entity_t = entity_manager_t::entity_t;

	entity_manager_t entityManager;

	for (int i = 0; i < 50; ++i) {
		entityManager.create_entity(i);
	}


	auto sum = 0;
	entityManager.for_each<int>(
			[&](entity_t /*ent*/, int i) {
			sum +=i;
			});
	CHECK(sum == 1225);
}



