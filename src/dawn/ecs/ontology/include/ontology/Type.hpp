// ----------------------------------------------------------------------------
// Type.hpp
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_TYPE_HPP__
#define __ONTOLOGY_TYPE_HPP__

// ----------------------------------------------------------------------------
// include files

#include "Type.hxx"

template <class T>
std::string getTypeName()
{
    return std::string(demangleTypeName(typeid(T).name()));
}

#endif // __ONTOLOGY_TYPE_HPP__