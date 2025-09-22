#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#include <voxblox/core/tsdf_map.h>
#include <voxblox/integrator/tsdf_integrator.h>
#include <voxblox/utils/timing.h>
#include <voxblox/mesh/mesh_integrator.h>
#include <voxblox/io/mesh_ply.h>
using namespace voxblox;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <cloud.pcd>" << std::endl;
        return -1;
    }

    // --- Load PCL cloud ---
// Use PointXYZRGB to load color information
pcl::PointCloud<pcl::PointXYZRGB>::Ptr pcl_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

if (pcl::io::loadPCDFile<pcl::PointXYZRGB>(argv[1], *pcl_cloud) == -1) {
    PCL_ERROR("Couldn't read the PCD file\n");
    return -1;
}    std::cout << "Loaded " << pcl_cloud->size() << " points." << std::endl;

    // --- Create TSDF Map ---
    TsdfMap::Config tsdf_config;
    tsdf_config.tsdf_voxel_size = 0.003;
    
 // --- Integrator config ---
voxblox::TsdfIntegratorBase::Config integrator_config;
integrator_config.default_truncation_distance = 4.0*tsdf_config.tsdf_voxel_size;
// --- Create the map ---
std::shared_ptr<voxblox::TsdfMap> tsdf_map_ =
    std::make_shared<voxblox::TsdfMap>(tsdf_config);
    // Simple integrator (merged rays)
    std::unique_ptr<TsdfIntegratorBase> integrator(
        new FastTsdfIntegrator(integrator_config,tsdf_map_ ->getTsdfLayerPtr()));

    // --- Convert PCL -> voxblox Pointcloud ---
    Pointcloud voxblox_cloud;
    Colors colors;  // empty (if you have RGB, fill it)
    colors.reserve(pcl_cloud->size());
    voxblox_cloud.reserve(pcl_cloud->size());
    for (const auto& pt : pcl_cloud->points) {
        voxblox_cloud.emplace_back(pt.x, pt.y, pt.z);
         // Add color (convert from uint8_t)
        voxblox::Color color(static_cast<uint8_t>(pt.r),
                         static_cast<uint8_t>(pt.g),
                         static_cast<uint8_t>(pt.b));
        colors.push_back(color);
    }

    // --- Camera / sensor pose ---
    Transformation T_G_C;   
    T_G_C.setIdentity();

    // --- Integrate cloud ---
    std::cout << "INtegrating cloud" <<std::endl;
    integrator->integratePointCloud(T_G_C, voxblox_cloud, colors);

    std::cout << "Integration done into TSDF layer!" << std::endl;

// Create mesh layer
std::shared_ptr<MeshLayer> mesh_layer =
    std::make_shared<MeshLayer>(tsdf_map_->block_size());
    voxblox::MeshIntegratorConfig mesh_config;
voxblox::MeshIntegrator<voxblox::TsdfVoxel> mesh_integrator(mesh_config,
                                                           tsdf_map_->getTsdfLayerPtr(),mesh_layer.get());

// Update the mesh
mesh_integrator.generateMesh(true,true);

// Save mesh to .ply
voxblox::outputMeshLayerAsPly("mesh_output.ply", true, *mesh_layer);

    return 0;
}
