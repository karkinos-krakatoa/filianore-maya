#ifndef _MATERIAL_PROCESSOR_H
#define _MATERIAL_PROCESSOR_H

#include <maya/MFnMesh.h>

#include "filianore/core/elemental.h"

std::shared_ptr<filianore::Material> ProcessMeshMaterials(MFnMesh &mMesh);

#endif