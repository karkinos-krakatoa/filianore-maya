#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <maya/MGlobal.h>
#include <maya/MItMeshPolygon.h>

#include "util.h"
#include "meshexporter.h"
#include "materialprocessor.h"

#include "filianore/shapes/triangle.h"
#include "filianore/core/primitive.h"

#include <map>

using namespace filianore;

using TriEntity = TriangleEntity;

MayaMesh::MayaMesh(MFnMesh &_mesh, MObject &mObject)
    : name(_mesh.name().asChar())
{
    MStatus status;

    std::shared_ptr<Material> material = ProcessMeshMaterials(_mesh);

    MDagPath mDag;
    MMatrix mMatrix;
    if (_mesh.getPath(mDag) == MS::kSuccess)
    {
        mMatrix = mDag.inclusiveMatrix();
    }

    MItMeshPolygon faceIt(mDag);

    MFloatArray uArray, vArray;
    _mesh.getUVs(uArray, vArray);

    uint numUvs = uArray.length();
    if (numUvs == 0)
    {
        uArray.append(0.0);
        vArray.append(0.0);
    }

    MPointArray triPoints;
    MIntArray triVtxIds;
    MIntArray faceVtxIds;

    for (faceIt.reset(); !faceIt.isDone(); faceIt.next())
    {
        int faceId = faceIt.index();
        int numTris;
        faceIt.numTriangles(numTris);
        faceIt.getVertices(faceVtxIds);

        MIntArray faceUVIndices;
        for (uint vtxId = 0; vtxId < faceVtxIds.length(); vtxId++)
        {
            int uvIndex;
            if (numUvs == 0)
            {
                faceUVIndices.append(0);
            }
            else
            {
                faceIt.getUVIndex(vtxId, uvIndex);
                faceUVIndices.append(uvIndex);
            }
        }

        for (int triId = 0; triId < numTris; triId++)
        {
            int faceRelIds[3];
            faceIt.getTriangle(triId, triPoints, triVtxIds);

            for (uint triVtxId = 0; triVtxId < 3; triVtxId++)
            {
                for (uint faceVtxId = 0; faceVtxId < faceVtxIds.length(); faceVtxId++)
                {
                    if (faceVtxIds[faceVtxId] == triVtxIds[triVtxId])
                    {
                        faceRelIds[triVtxId] = faceVtxId;
                    }
                }
            }

            uint uvId0 = faceUVIndices[faceRelIds[0]];
            uint uvId1 = faceUVIndices[faceRelIds[1]];
            uint uvId2 = faceUVIndices[faceRelIds[2]];

            MPoint p1 = triPoints[0] * mMatrix;
            StaticArray<float, 3> vtx1 = StaticArray<float, 3>((float)p1.x, (float)p1.y, (float)p1.z);
            MPoint p2 = triPoints[1] * mMatrix;
            StaticArray<float, 3> vtx2 = StaticArray<float, 3>((float)p2.x, (float)p2.y, (float)p2.z);
            MPoint p3 = triPoints[2] * mMatrix;
            StaticArray<float, 3> vtx3 = StaticArray<float, 3>((float)p3.x, (float)p3.y, (float)p3.z);

            TriEntity t1(vtx1, StaticArray<float, 3>(), false, StaticArray<float, 2>(uArray[uvId0], vArray[uvId0]));
            TriEntity t2(vtx2, StaticArray<float, 3>(), false, StaticArray<float, 2>(uArray[uvId1], vArray[uvId1]));
            TriEntity t3(vtx3, StaticArray<float, 3>(), false, StaticArray<float, 2>(uArray[uvId2], vArray[uvId2]));

            std::shared_ptr<Shape> triangle = std::make_shared<Triangle>(t1, t2, t3);
            std::shared_ptr<Primitive> prim = std::make_shared<GeometricPrimitive>(triangle, material, nullptr);

            primitives.emplace_back(prim);
        }
    }
}

std::vector<std::shared_ptr<Primitive>> MeshExporter::ExportPrimitives()
{
    std::vector<std::shared_ptr<Primitive>> prims;
    prims.resize(0);

    for (MItDag it; !it.isDone(); it.next())
    {
        MObject obj = it.currentItem();
        if (obj.apiType() == MFn::kMesh)
        {
            MFnMesh mesh(obj);
            MayaMesh mMesh(mesh, obj);

            std::vector<std::shared_ptr<Primitive>> primitivesFromMesh = mMesh.primitives;

            prims.insert(prims.end(), primitivesFromMesh.begin(), primitivesFromMesh.end());
        }
    }

    return prims;
}