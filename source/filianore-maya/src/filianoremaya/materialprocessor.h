#ifndef _MATERIAL_PROCESSOR_H
#define _MATERIAL_PROCESSOR_H

#include <maya/MFnMesh.h>

#include "filianore/core/elemental.h"

using namespace filianore;

std::shared_ptr<Texture<PrincipalSpectrum>> GetColorNode(MPlug &mShaderObjectPlug);

std::shared_ptr<Material> ProcessMeshMaterials(MFnMesh &mMesh);

#endif