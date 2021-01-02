#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>

#include "meshexporter.h"

#include "filianore/shapes/triangle.h"
#include "filianore/core/primitive.h"

#include <map>

using Vec3 = filianore::StaticArray<float, 3>;
using TriEntity = filianore::TriangleEntity;

MayaMesh::MayaMesh(MFnMesh &_mesh)
    : name(_mesh.name().asChar())
{
    MDagPath mDag;
    MMatrix mMatrix;
    if (_mesh.getPath(mDag) == MS::kSuccess)
    {
        mMatrix = mDag.inclusiveMatrix();
    }

    MPointArray mVertices;
    _mesh.getPoints(mVertices);

    std::vector<Vec3> vertices;
    vertices.reserve(mVertices.length());
    for (unsigned int i = 0; i < mVertices.length(); i++)
    {
        MPoint point = mVertices[i] * mMatrix;
        vertices.emplace_back(Vec3((float)point.x, (float)point.y, (float)point.z));
    }

    MFloatVectorArray mNormals;
    _mesh.getNormals(mNormals);

    std::vector<Vec3> normals;
    normals.reserve(mNormals.length());
    for (unsigned int i = 0; i < mNormals.length(); i++)
    {
        normals.emplace_back(Vec3(mNormals[i].x, mNormals[i].y, mNormals[i].z));
    }

    MIntArray mTrianglesCount;
    MIntArray mVerticesIndices;
    _mesh.getTriangles(mTrianglesCount, mVerticesIndices);
    for (int polygon = 0; polygon < _mesh.numPolygons(); ++polygon)
    {
        MIntArray vertexList;
        MIntArray faceNormalList;

        _mesh.getPolygonVertices(polygon, vertexList);
        _mesh.getFaceNormalIds(polygon, faceNormalList);

        std::map<int, int> verticesToNormals;
        for (unsigned int i = 0; i < vertexList.length(); ++i)
        {
            verticesToNormals.emplace(vertexList[i], faceNormalList[i]);
        }

        for (int i = 0; i < mTrianglesCount[polygon]; ++i)
        {
            int vertexIds[3];
            _mesh.getPolygonTriangleVertices(polygon, i, vertexIds);

            Vec3 triV1 = Vec3(vertices[vertexIds[0]]);
            Vec3 triV2 = Vec3(vertices[vertexIds[1]]);
            Vec3 triV3 = Vec3(vertices[vertexIds[2]]);

            Vec3 triN1 = Vec3(normals[verticesToNormals[vertexIds[0]]]);
            Vec3 triN2 = Vec3(normals[verticesToNormals[vertexIds[1]]]);
            Vec3 triN3 = Vec3(normals[verticesToNormals[vertexIds[2]]]);

            TriEntity t1(triV1, triN1, true);
            TriEntity t2(triV2, triN2, true);
            TriEntity t3(triV3, triN3, true);

            std::shared_ptr<filianore::Shape> triangle = std::make_shared<filianore::Triangle>(t1, t2, t3);
            std::shared_ptr<filianore::Primitive> primitive = std::make_shared<filianore::GeometricPrimitive>(triangle, nullptr);

            primitives.emplace_back(primitive);
        }
    }
}

std::vector<std::shared_ptr<filianore::Primitive>> MeshExporter::ExportPrimitives()
{
    std::vector<std::shared_ptr<filianore::Primitive>> prims;
    prims.resize(0);

    for (MItDag it; !it.isDone(); it.next())
    {
        MObject obj = it.currentItem();
        if (obj.apiType() == MFn::kMesh)
        {
            MFnMesh mesh(obj);
            MayaMesh mMesh(mesh);

            std::vector<std::shared_ptr<filianore::Primitive>> primitivesFromMesh = mMesh.primitives;

            prims.insert(prims.end(), primitivesFromMesh.begin(), primitivesFromMesh.end());
        }
    }

    return prims;
}