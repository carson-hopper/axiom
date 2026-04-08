# Packet Code Generation
# 
# This module automatically generates packet C++ code from YAML schemas.
# Add new protocol versions by creating YAML files in schemas/ directory.

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(PACKET_SCHEMA_DIR "${CMAKE_SOURCE_DIR}/tools/codegen/schemas")
set(PACKET_GENERATOR "${CMAKE_SOURCE_DIR}/tools/codegen/generate_packets.py")
set(PACKET_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated/packets")

set(PROTOCOL_VERSIONS 775)

set(PACKET_GENERATED_HEADERS)
set(PACKET_GENERATED_SOURCES)

foreach(VERSION ${PROTOCOL_VERSIONS})
    set(SCHEMA_FILE "${PACKET_SCHEMA_DIR}/protocol_${VERSION}.yml")
    set(VERSION_OUTPUT_DIR "${PACKET_GENERATED_DIR}/${VERSION}")
    
    if(EXISTS ${SCHEMA_FILE})
        add_custom_command(
            OUTPUT ${VERSION_OUTPUT_DIR}/stamp
            COMMAND ${Python3_EXECUTABLE} ${PACKET_GENERATOR} ${SCHEMA_FILE} ${PACKET_GENERATED_DIR}
            COMMAND ${CMAKE_COMMAND} -E touch ${VERSION_OUTPUT_DIR}/stamp
            DEPENDS ${SCHEMA_FILE} ${PACKET_GENERATOR}
            COMMENT "Generating packets for protocol ${VERSION}..."
            VERBATIM
        )
        
        list(APPEND PACKET_GENERATED_HEADERS ${VERSION_OUTPUT_DIR}/stamp)
    endif()
endforeach()

add_custom_target(packet_codegen DEPENDS ${PACKET_GENERATED_HEADERS})

include_directories(${PACKET_GENERATED_DIR}/include)

file(GLOB_RECURSE PACKET_SOURCES "${PACKET_GENERATED_DIR}/src/*.cpp")
target_sources(axiom-server PRIVATE ${PACKET_SOURCES})
add_dependencies(axiom-server packet_codegen)
