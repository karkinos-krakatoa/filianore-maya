#ifndef _MATERIAL_PROCESSOR_H
#define _MATERIAL_PROCESSOR_H

#include <maya/MFnMesh.h>

#include "filianore/core/elemental.h"

using namespace filianore;

std::shared_ptr<Material> ProcessMeshMaterials(MFnMesh &mMesh);

//std::shared_ptr<Material> CreateDefaultLambertMaterial();

#endif