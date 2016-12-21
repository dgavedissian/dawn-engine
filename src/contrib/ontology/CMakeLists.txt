project (ontologylib)

###############################################################################
# generate build configuration
###############################################################################

# translate option settings to variables that can be replaced
# in the configuration file
if (WITH_THREAD)
    set (ONTOLOGY_WITH_THREAD 1)
endif ()
if (BUILD_SHARED)
    set (ONTOLOGY_SHARED 1)
else ()
    set (ONTOLOGY_STATIC 1)
endif ()
if (WITH_EXCEPTIONS)
    set (ONTOLOGY_WITH_EXCEPTIONS 1)
elseif (WITH_ASSERT)
    set (ONTOLOGY_WITH_ASSERT 1)
else ()
endif ()

# generate config file
configure_file (
    "${CMAKE_CURRENT_LIST_DIR}/include/ontology/Config.hpp.in"
    "${CMAKE_CURRENT_LIST_DIR}/include/ontology/Config.hpp"
)

###############################################################################
# defines
###############################################################################

add_definitions (-DONTOLOGY_BUILDING_LIBRARY)

###############################################################################
# source and header files
###############################################################################

set (ontology_CONFIG_IN
    "include/ontology/Config.hpp.in"
)

set (ontology_HEADERS
    "include/ontology/Config.hpp"
    "include/ontology/Component.hpp"
    "include/ontology/Entity.hpp"
    "include/ontology/Entity.hxx"
    "include/ontology/EntityManager.hpp"
    "include/ontology/EntityManagerInterface.hpp"
    "include/ontology/EntityManagerListener.hpp"
    "include/ontology/Exception.hpp"
    "include/ontology/ListenerDispatcher.hpp"
    "include/ontology/ListenerDispatcher.hxx"
    "include/ontology/System.hpp"
    "include/ontology/System.hxx"
    "include/ontology/SystemManager.hpp"
    "include/ontology/SystemManager.hxx"
    "include/ontology/Type.hpp"
    "include/ontology/Type.hxx"
    "include/ontology/TypeContainers.hpp"
    "include/ontology/World.hpp"
    "include/ontology/Ontology.hpp"
)

set (ontology_SOURCES
    "src/Component.cpp"
    "src/Entity.cpp"
    "src/EntityManager.cpp"
    "src/EntityManagerListener.cpp"
    "src/Exception.cpp"
    "src/System.cpp"
    "src/SystemManager.cpp"
    "src/Type.cpp"
    "src/World.cpp"
)

###############################################################################
# create target
###############################################################################

if (BUILD_SHARED)
    add_library (ontology SHARED ${ontology_CONFIG_IN} ${ontology_HEADERS} ${ontology_SOURCES})
else ()
    add_library (ontology STATIC ${ontology_CONFIG_IN} ${ontology_HEADERS} ${ontology_SOURCES})
endif ()

###############################################################################
# handle multithreading option - boost is required if enabled
###############################################################################

if (WITH_THREAD)
    find_package (Boost REQUIRED COMPONENTS
        system
        thread
    )

    target_link_libraries (ontology
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
    )

    # work around gcc bug on unix where -pthread doesn't link against
    # pthread and -fopenmp doesn't link against libgomp
    if (UNIX)
        target_link_libraries (ontology pthread gomp)
    endif ()
endif (WITH_THREAD)

###############################################################################
# install targets
###############################################################################

install (TARGETS ontology DESTINATION "lib")
install (FILES ${ontology_HEADERS} DESTINATION "include/ontology")

