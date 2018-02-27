// ----------------------------------------------------------------------------
// TypeContainers.hpp
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_TYPE_COMPARATOR_HPP__
#define __ONTOLOGY_TYPE_COMPARATOR_HPP__

// ----------------------------------------------------------------------------
// include files

#include <typeinfo>
#include <vector>
#include <set>
#include <map>
#include <memory>

namespace Ontology {

/*!
 * @brief Entity comparator for entry in std::set and std::map.
 * @note see http://stackoverflow.com/questions/8682582/what-is-type-infobefore-useful-for
 */
struct TypeComparator
{
    inline bool operator()(const std::type_info* a, const std::type_info* b) const
    {
        return a->before(*b);
    }
};

/// A set of std::type_info pointers (&typeid(T)).
typedef std::set<const std::type_info*, TypeComparator> TypeSet;

/// A map of std::type_info pointers (&typeid(T)) and a templated type.
template <class T>
struct GenericTypeMap
{
    typedef std::map<const std::type_info*, T, TypeComparator> TypeMap;
};
template <class T>
using TypeMap = typename GenericTypeMap<T>::TypeMap;

/// A TypeMap the second pair type being a unique_ptr of a templated type.
template <class T>
using TypeMapUniquePtr = typename GenericTypeMap< std::unique_ptr<T> >::TypeMap;

/// A TypeMap the second pair type being a shared_ptr of a templated type.
template <class T>
using TypeMapSharedPtr = typename GenericTypeMap< std::shared_ptr<T> >::TypeMap;

/// A vector of std::type_info pointers (&typeid(T))
typedef std::vector<const std::type_info*> TypeVector;

/// A pair of std::type_info pointers (&typeid(T)) and a templated type.
template <class T>
using TypePair = std::pair<const std::type_info*, T>;

/// A vector of templated TypePairs.
template <class T>
using TypeVectorPair = std::vector< TypePair<T> >;

/// A TypeVectorPair wrapping a templated smart pointer type
template <class SmartPtr>
struct TypeVectorPairSmartPtr : public TypeVectorPair<SmartPtr>
{
    typedef typename TypeVectorPair<SmartPtr>::iterator iterator;
    typedef typename TypeVectorPair<SmartPtr>::const_iterator const_iterator;

    /// Searches the vector for the specified type. (linear search)
    iterator find(const std::type_info* key)
    {
        iterator it = this->begin();
        for(; it != this->end(); ++it)
            if(it->first == key)
                return it;
        return it;
    }
    
    /// Searches the vector for the specified type. (linear search)
    const_iterator find(const std::type_info* key) const
    {
        const_iterator it = this->begin();
        for(; it != this->end(); ++it)
            if(it->first == key)
                return it;
        return it;
    }
};

/// A TypeVectorPair with the second pair type being a unique_ptr of a templated type.
template <class T>
using TypeVectorPairUniquePtr = TypeVectorPairSmartPtr< std::unique_ptr<T> >;

/// A TypeVectorPair with the second pair type being a shared_ptr of a templated type.
template <class T>
using TypeVectorPairSharedPtr = TypeVectorPairSmartPtr< std::shared_ptr<T> >;

} // namespace Ontology

#endif // __ONTOLOGY_TYPE_COMPARATOR_HPP__
