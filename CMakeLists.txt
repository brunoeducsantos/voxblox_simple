cmake_minimum_required(VERSION 3.10)
project(voxblox)

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
find_package(Protobuf )
set(CMAKE_PREFIX_PATH "/usr/local")
find_package(glog REQUIRED)
# ----------------------
find_package(GTest REQUIRED)
# Proto files
# ----------------------
set(PROTO_FILES
    proto/voxblox/Block.proto
    proto/voxblox/Layer.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})


# ----------------------
# Voxblox proto library
# ----------------------
add_library(voxblox_proto ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(voxblox_proto PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}   # where .pb.h are generated
    ${Protobuf_INCLUDE_DIRS}
)
target_link_libraries(voxblox_proto PUBLIC ${Protobuf_LIBRARIES})

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
    ${EIGEN3_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GENERATED_DIR}  # Include generated proto headers
    ${kindr}
    )
target_link_libraries(voxblox PUBLIC Eigen3::Eigen )
 