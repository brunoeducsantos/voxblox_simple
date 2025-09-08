cmake_minimum_required(VERSION 3.10)
project(voxblox_cloud_integration)

find_package(PCL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Protobuf REQUIRED)
find_package(glog REQUIRED)

# Voxblox (assuming you installed it)
find_package(voxblox REQUIRED)

add_executable(voxblox_cloud_integration main.cpp)

target_link_libraries(voxblox_cloud_integration
    voxblox::voxblox
    ${PCL_LIBRARIES}
    Eigen3::Eigen
        ${GLOG_LIBRARIES}
    protobuf::libprotobuf
)