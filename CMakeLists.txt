cmake_minimum_required(VERSION 3.10)
project(voxblox VERSION 0.1.0 LANGUAGES CXX)
# ----------------------
# C++ standard
# ----------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ----------------------
# Dependencies
# ----------------------
find_package(Eigen3 REQUIRED)
set(CMAKE_PREFIX_PATH "/usr/local")
find_package(Protobuf REQUIRED)
find_package(glog REQUIRED)
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
    $<INSTALL_INTERFACE:include>                            # when used after installation
)
target_link_libraries(voxblox PUBLIC Eigen3::Eigen glog::glog
    protobuf::libprotobuf)
 
install(TARGETS voxblox
    EXPORT voxbloxTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

# Export targets to a cmake file
install(EXPORT voxbloxTargets
    FILE voxbloxTargets.cmake
    NAMESPACE voxblox::
    DESTINATION lib/cmake/voxblox
)

# Create a Config file for find_package
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