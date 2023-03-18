#ifndef _MESH_EXPORTER_H
#define _MESH_EXPORTER_H

#include <maya/MFnMesh.h>

#include <string>
#include <vector>

#include "filianore/core/elemental.h"

class MayaMesh {
public:
    MayaMesh() {
    }

    MayaMesh(MFnMesh &_mesh, MObject &mObject);

    std::string name;
    std::vector<std::shared_ptr<filianore::Primitive>> primitives;
};

class MeshExporter {
public:
    MeshExporter() {
    }

    std::vector<std::shared_ptr<filianore::Primitive>> ExportPrimitives();
};

#endif