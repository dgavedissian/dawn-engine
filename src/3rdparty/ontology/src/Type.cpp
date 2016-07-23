// ----------------------------------------------------------------------------
// Type.cpp
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// include files

#include "ontology/Type.hxx"

#ifdef __GNUG__
#   include <cstdlib>
#   include <memory>
#   include <cxxabi.h>

std::string demangleTypeName(const char* name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}

#else

// does nothing if not g++
std::string demangleTypeName(const char* name) {
    return name;
}

#endif