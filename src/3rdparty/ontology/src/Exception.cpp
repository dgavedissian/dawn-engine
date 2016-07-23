// ----------------------------------------------------------------------------
// Exception.cpp
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// include files

#include <ontology/Exception.hpp>

namespace Ontology {

// ----------------------------------------------------------------------------
#ifdef ONTOLOGY_WITH_EXCEPTIONS
Exception::Exception(const std::string& msg) :
    runtime_error(msg)
{
}
#endif

} // namespace Ontology