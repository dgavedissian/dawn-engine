// ----------------------------------------------------------------------------
// Type.hxx
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_TYPE_HXX__
#define __ONTOLOGY_TYPE_HXX__

// ----------------------------------------------------------------------------
// include files

#include <string>

std::string demangleTypeName(const char* name);
template <class T>
std::string getTypeName();

#endif // __ONTOLOGY_TYPE_HXX__