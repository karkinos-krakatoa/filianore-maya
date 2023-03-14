#include <maya/MDagPath.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDag.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>

#include "materialprocessor.h"
#include "meshexporter.h"
#include "util.h"

#include "filianore/core/primitive.h"
#include "filianore/shapes/triangle.h"

#include <map>

MayaMesh::MayaMesh(MFnMesh &_mesh, MObject &mObject)
    : name(_mesh.name().asChar()) {
    MStatus status;

    std::shared_ptr<filianore::Material> material = ProcessMeshMaterials(_mesh);

    MDagPath mDag;
    MMatrix mMatrix;
    if (_mesh.getPath(mDag) == MS::kSuccess) {
        mMatrix = mDag.inclusiveMatrix();
    }

    MItMeshPolygon faceIt(mDag);

    MFloatVectorArray normals;
    _mesh.getNormals(normals, MSpace::kWorld);

    MFloatArray uArray, vArray;
    _mesh.getUVs(uArray, vArray);

    uint numUvs = uArray.length();
    if (numUvs == 0) {
        uArray.append(0.0);
        vArray.append(0.0);
    }

    MPointArray triPoints;
    MIntArray triVtxIds;
    MIntArray faceVtxIds;
    MIntArray faceNormalIds;

    for (faceIt.reset(); !faceIt.isDone(); faceIt.next()) {
        int faceId = faceIt.index();
        int numTris;
        faceIt.numTriangles(numTris);
        faceIt.getVertices(faceVtxIds);

        MIntArray faceUVIndices;
        faceNormalIds.clear();
        for (uint vtxId = 0; vtxId < faceVtxIds.length(); vtxId++) {
            faceNormalIds.append(faceIt.normalIndex(vtxId));
            int uvIndex;
            if (numUvs == 0) {
                faceUVIndices.append(0);
            } else {
                faceIt.getUVIndex(vtxId, uvIndex);
                faceUVIndices.append(uvIndex);
            }
        }

        for (int triId = 0; triId < numTris; triId++) {
            int faceRelIds[3];
            faceIt.getTriangle(triId, triPoints, triVtxIds, MSpace::kWorld);

            for (uint triVtxId = 0; triVtxId < 3; triVtxId++) {
                for (uint faceVtxId = 0; faceVtxId < faceVtxIds.length(); faceVtxId++) {
                    if (faceVtxIds[faceVtxId] == triVtxIds[triVtxId]) {
                        faceRelIds[triVtxId] = faceVtxId;
                    }
                }
            }

            // UVs
            uint uvId1 = faceUVIndices[faceRelIds[0]];
            filianore::Vector2f uv1 = filianore::Vector2f(uArray[uvId1], vArray[uvId1]);
            uint uvId2 = faceUVIndices[faceRelIds[1]];
            filianore::Vector2f uv2 = filianore::Vector2f(uArray[uvId2], vArray[uvId2]);
            uint uvId3 = faceUVIndices[faceRelIds[2]];
            filianore::Vector2f uv3 = filianore::Vector2f(uArray[uvId3], vArray[uvId3]);

            // Vertices
            MPoint p1 = triPoints[0];
            filianore::Vector3f vtx1 = filianore::Vector3f((float)p1.x, (float)p1.y, (float)p1.z);
            MPoint p2 = triPoints[1];
            filianore::Vector3f vtx2 = filianore::Vector3f((float)p2.x, (float)p2.y, (float)p2.z);
            MPoint p3 = triPoints[2];
            filianore::Vector3f vtx3 = filianore::Vector3f((float)p3.x, (float)p3.y, (float)p3.z);

            // Normals
            MFloatVector nn1 = normals[faceNormalIds[faceRelIds[0]]];
            filianore::Vector3f n1 = filianore::Vector3f((float)nn1.x, (float)nn1.y, (float)nn1.z);
            MFloatVector nn2 = normals[faceNormalIds[faceRelIds[1]]];
            filianore::Vector3f n2 = filianore::Vector3f((float)nn2.x, (float)nn2.y, (float)nn2.z);
            MFloatVector nn3 = normals[faceNormalIds[faceRelIds[2]]];
            filianore::Vector3f n3 = filianore::Vector3f((float)nn3.x, (float)nn3.y, (float)nn3.z);

            // Process for Filianore
            filianore::TriangleEntity t1(vtx1, n1, true, filianore::Vector3f(), false, uv1);
            filianore::TriangleEntity t2(vtx2, n2, true, filianore::Vector3f(), false, uv2);
            filianore::TriangleEntity t3(vtx3, n3, true, filianore::Vector3f(), false, uv3);

            std::shared_ptr<filianore::Shape> triangle = std::make_shared<filianore::Triangle>(t1, t2, t3);
            std::shared_ptr<filianore::Primitive> prim = std::make_shared<filianore::GeometricPrimitive>(triangle, material, nullptr);

            primitives.emplace_back(prim);
        }
    }
}

std::vector<std::shared_ptr<filianore::Primitive>> MeshExporter::ExportPrimitives() {
    std::vector<std::shared_ptr<filianore::Primitive>> prims;
    prims.resize(0);

    for (MItDag it; !it.isDone(); it.next()) {
        MObject obj = it.currentItem();
        if (obj.apiType() == MFn::kMesh) {
            MFnMesh mesh(obj);
            MayaMesh mMesh(mesh, obj);

            std::vector<std::shared_ptr<filianore::Primitive>> primitivesFromMesh = mMesh.primitives;

            prims.insert(prims.end(), primitivesFromMesh.begin(), primitivesFromMesh.end());
        }
    }

    return prims;
}