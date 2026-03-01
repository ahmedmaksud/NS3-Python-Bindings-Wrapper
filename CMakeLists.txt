# Python Bindings Module CMakeLists.txt
# Author: Ahmed Maksud <amaks002@ucr.edu>
# Lab: SHINE Lab, Texas State University
# PI: Marcelo Menezes De Carvalho
# This module provides Python-C++ communication interface for NS3-AI integration

cmake_minimum_required(VERSION 3.10)

# Project information
set(PROJECT_AUTHOR "Ahmed Maksud")
set(PROJECT_EMAIL "amaks002@ucr.edu")
set(PROJECT_LAB "SHINE Lab, Texas State University")
set(PROJECT_PI "Marcelo Menezes De Carvalho")
set(PROJECT_DESCRIPTION "NS3-AI Python Binding Interface for Energy Harvesting Networks")

# Python Bindings Library Sources with pb- prefix
set(PYTHON_BINDINGS_SOURCES
    pb-core.cc
    pb-wrapper.cc
)

set(PYTHON_BINDINGS_HEADERS
    pb-core.h
    pb-wrapper.h
)

# Print author information
message(STATUS "========================================")
message(STATUS "  Python Bindings Module")
message(STATUS "  Author: ${PROJECT_AUTHOR}")
message(STATUS "  Email: ${PROJECT_EMAIL}")
message(STATUS "  Lab: ${PROJECT_LAB}")
message(STATUS "  PI: ${PROJECT_PI}")
message(STATUS "  Description: ${PROJECT_DESCRIPTION}")
message(STATUS "========================================")

# Build AI-controlled PowerCast simulation with Python bindings
set(pb_core_sources
    pb-core.cc
    ../powercast_hardware/ph-harvester-hardware.cc
    ../powercast_hardware/ph-deployment-helper.cc
)

set(pb_core_libraries
    ${libai}
    ${libcore}
    ${libnetwork}
    ${libwifi}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libflow-monitor}
    ${libpoint-to-point}
    ${libenergy}
)

build_lib_example(
    NAME pb_core_simulation
    SOURCE_FILES ${pb_core_sources}
    LIBRARIES_TO_LINK ${pb_core_libraries}
)

# Build wrapper demo using pb-wrapper functions
set(pb_wrapper_sources
    pb-wrapper-demo.cc
    pb-wrapper.cc
    ../powercast_hardware/ph-harvester-hardware.cc
    ../powercast_hardware/ph-deployment-helper.cc
)

build_lib_example(
    NAME pb_wrapper_demo
    SOURCE_FILES ${pb_wrapper_sources}
    LIBRARIES_TO_LINK ${pb_core_libraries}
)

# Python binding module using pybind11 with new naming
pybind11_add_module(pb_interface_py pb-interface.cc)
set_target_properties(pb_interface_py PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_NAME "pb_interface_py"
    PREFIX ""
    SUFFIX ".so"
)
target_link_libraries(pb_interface_py PRIVATE ${libai})

# Build Python interface along with C++ lib
# Note: Dependencies will be handled automatically by the build system

# Deploy Python files to build directory with new naming
add_custom_target(deploy_pb_python_bindings ALL
    # Copy Python AI core script
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/pb-core.py
    ${CMAKE_BINARY_DIR}/contrib/ai/examples/pb-wrapper/pb-core.py
    
    # Copy Python wrapper demo script
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/pb-wrapper-demo.py
    ${CMAKE_BINARY_DIR}/contrib/ai/examples/pb-wrapper/pb-wrapper-demo.py
    
    COMMENT "Deploying Python binding files by ${PROJECT_AUTHOR}"
    DEPENDS pb_interface_py
)

# Additional deployment for Python binding library
add_custom_command(TARGET pb_interface_py POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:pb_interface_py>
    ${CMAKE_BINARY_DIR}/contrib/ai/examples/pb-wrapper/
    COMMENT "Copying Python binding library to build directory by ${PROJECT_AUTHOR}"
)

# Create documentation target
add_custom_target(pb_docs
    COMMAND echo "========================================="
    COMMAND echo "Python Bindings Documentation"
    COMMAND echo "Author: ${PROJECT_AUTHOR}"
    COMMAND echo "Email: ${PROJECT_EMAIL}"
    COMMAND echo "Lab: ${PROJECT_LAB}"
    COMMAND echo "PI: ${PROJECT_PI}"
    COMMAND echo "========================================="
    COMMAND echo "Files created:"
    COMMAND echo "  - pb-core.h/cc: Main implementation with verbose logging"
    COMMAND echo "  - pb-wrapper.h/cc: Clean wrapper functions for integration"
    COMMAND echo "  - pb-interface.cc: Python bindings"
    COMMAND echo "  - pb-core.py: Python AI core script"
    COMMAND echo "  - pb-wrapper-demo.cc: C++ wrapper demonstration"
    COMMAND echo "  - pb-wrapper-demo.py: Python wrapper demonstration"
    COMMAND echo ""
    COMMAND echo "Executables built:"
    COMMAND echo "  - pb_core_simulation: Main simulation with verbose output"
    COMMAND echo "  - pb_wrapper_demo: Clean wrapper demonstration"
    COMMAND echo "========================================="
    COMMENT "Generating documentation by ${PROJECT_AUTHOR}"
)

# Export variables for use by parent CMakeLists
set(PYTHON_BINDINGS_SOURCES ${PYTHON_BINDINGS_SOURCES} PARENT_SCOPE)
set(PYTHON_BINDINGS_HEADERS ${PYTHON_BINDINGS_HEADERS} PARENT_SCOPE)
set(PYTHON_BINDINGS_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(PYTHON_BINDINGS_AUTHOR "${PROJECT_AUTHOR}" PARENT_SCOPE)

# Add compile definitions for author information
# Note: Compile definitions will be handled by the build system automatically

# Installation rules
install(FILES ${PYTHON_BINDINGS_HEADERS}
    DESTINATION include/ns3/ai/python-bindings
    COMPONENT development
)

install(TARGETS pb_interface_py
    LIBRARY DESTINATION lib/python-bindings
    COMPONENT runtime
)

install(FILES pb-core.py pb-wrapper-demo.py
    DESTINATION share/ns3/python-bindings
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    COMPONENT runtime
)