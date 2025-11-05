cmake_minimum_required(VERSION 3.23)
project(voxblox VERSION 0.1.0 LANGUAGES CXX)
# ----------------------
# C++ standard
# ----------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------
# Dependencies
# ----------------------
find_package(Eigen3 REQUIRED)
find_package(glog REQUIRED)

# --- Protobuf ---
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# --- Define your proto files ---
set(PROTO_DEFNS
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/voxblox/Block.proto
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/voxblox/Layer.proto
)
# --- Set output directory for generated protobufs ---
set(GENERATED_PROTO_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

# --- Generate C++ sources from .proto ---
protobuf_generate_cpp(
    PROTO_SRCS PROTO_HDRS
    ${PROTO_DEFNS}
    PROTOC_OUT_DIR ${GENERATED_PROTO_DIR}
)

# --- Create the protobuf library ---
add_library(voxblox_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})

target_include_directories(voxblox_proto
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/generated
        ${CMAKE_CURRENT_BINARY_DIR}/generated/voxblox
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(voxblox_proto PUBLIC protobuf::libprotobuf)

# ----------------------
# Voxblox core library
# ----------------------
set(VOXBLOX_SRCS
    src/core/block.cc
    src/core/esdf_map.cc
    src/core/tsdf_map.cc
    src/integrator/esdf_integrator.cc
    src/integrator/esdf_occ_integrator.cc
    src/integrator/integrator_utils.cc
    src/integrator/intensity_integrator.cc
    src/integrator/tsdf_integrator.cc
    src/io/mesh_ply.cc
    src/io/sdf_ply.cc
    src/mesh/marching_cubes.cc
    src/utils/evaluation_utils.cc
    src/utils/layer_utils.cc
    src/utils/neighbor_tools.cc
    src/utils/protobuf_utils.cc
    src/utils/timing.cc
    src/utils/voxel_utils.cc
)

add_library(voxblox SHARED ${VOXBLOX_SRCS})
target_include_directories(voxblox PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # when building inside this source tree
    $<INSTALL_INTERFACE:include>                             # when used after installation
)

target_link_libraries(voxblox PUBLIC Eigen3::Eigen glog::glog
    protobuf::libprotobuf voxblox_proto)
 

# ----------------------
# Installation
# ----------------------
install(TARGETS voxblox voxblox_proto
    EXPORT voxbloxTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

# ----------------------
# CMake package config
# ----------------------
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/voxbloxConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/voxbloxConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/voxbloxConfig.cmake"
    INSTALL_DESTINATION lib/cmake/voxblox
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/voxbloxConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/voxbloxConfigVersion.cmake"
    DESTINATION lib/cmake/voxblox
)

install(EXPORT voxbloxTargets
    FILE voxbloxTargets.cmake
    NAMESPACE voxblox::
    DESTINATION lib/cmake/voxblox
)