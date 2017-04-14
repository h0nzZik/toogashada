#ifndef COMMON_GENERICMANAGER_H_
#define COMMON_GENERICMANAGER_H_

// Standard
#include <map>
#include <memory>
#include <stdexcept>
#include <utility> // std::move
#include <vector>

// Boost
#include <boost/iterator/indirect_iterator.hpp>

template < typename T >
class GenericManager {
private:
	using Object = T;
	using Objects = std::vector<std::unique_ptr<Object>>;
	using ObjectMap = std::map<uint32_t, Object *>;

	public:
		Object & getObjectById(uint32_t id);

		uint32_t get_fresh_id();
		void insert(std::unique_ptr<Object> object);

		// Only constant iterator is needed

		using iterator = boost::indirect_iterator<typename Objects::iterator, Object>;
		using const_iterator = boost::indirect_iterator<typename Objects::const_iterator, Object>;

		iterator begin();
		iterator end();

		const_iterator begin() const;
		const_iterator end() const;

		const_iterator cbegin() const;
		const_iterator cend() const;


	private:
		Objects _objects;
		ObjectMap _objects_map;
		uint32_t _last_id = 0;
};

template < typename T >
typename GenericManager<T>::Object &
GenericManager<T>::getObjectById(uint32_t id) {
	return *_objects_map.at(id);
}

template < typename T >
uint32_t GenericManager<T>::get_fresh_id() {
	return _last_id++;
}

template < typename T >
void GenericManager<T>::insert(std::unique_ptr<Object> object) {
	Object * o = object.get();
	if (_objects_map[o->object_id()] != nullptr)
		throw std::logic_error(std::string("Object with id ") + std::to_string(o->object_id()) + "is already registered");

	_objects_map[o->object_id()] = o;
	_objects.push_back(std::move(object));
}

template < typename T >
typename GenericManager<T>::iterator
GenericManager<T>::begin(){
	return _objects.begin();
}

template < typename T >
typename GenericManager<T>::iterator
GenericManager<T>::end(){
	return _objects.end();
}

template < typename T >
typename GenericManager<T>::const_iterator
GenericManager<T>::begin() const {
	return cbegin();
}

template < typename T >
typename GenericManager<T>::const_iterator
GenericManager<T>::end() const {
	return cend();
}

template < typename T >
typename GenericManager<T>::const_iterator
GenericManager<T>::cbegin() const {
	return _objects.cbegin();
}

template < typename T >
typename GenericManager<T>::const_iterator
GenericManager<T>::cend() const {
	return _objects.cend();
}

#endif /* COMMON_GENERICMANAGER_H_ */
