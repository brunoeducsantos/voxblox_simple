cmake_minimum_required(VERSION 3.22)
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
find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
message(STATUS "Found Protobuf ${Protobuf_VERSION}")
message(STATUS "Protobuf include dirs: ${Protobuf_INCLUDE_DIRS}")
message(STATUS "Protobuf lib: ${Protobuf_LIBRARIES}")

# ----------------------
# Proto files
# ----------------------
set(PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/voxblox/Block.proto
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/voxblox/Layer.proto
)

# Output directory for generated protobuf files (inside build)
set(GENERATED_PROTO_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/voxblox)
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

# Generate C++ protobuf files into GENERATED_PROTO_DIR
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES} PROTOC_OUT_DIR ${GENERATED_PROTO_DIR})

# ----------------------
# voxblox_proto library
# ----------------------
add_library(voxblox_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})

# Build-time includes (private)
target_include_directories(voxblox_proto
    PRIVATE ${GENERATED_PROTO_DIR}
)

# Interface includes (for downstream projects)
target_include_directories(voxblox_proto
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(voxblox_proto PUBLIC ${Protobuf_LIBRARIES})

# Install generated protobuf headers
install(DIRECTORY ${GENERATED_PROTO_DIR}/
        DESTINATION include/voxblox
        FILES_MATCHING PATTERN "*.h")

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

# Include headers (build and install)
target_include_directories(voxblox PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${GENERATED_PROTO_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(voxblox PUBLIC Eigen3::Eigen glog::glog voxblox_proto ${Protobuf_LIBRARIES})

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
