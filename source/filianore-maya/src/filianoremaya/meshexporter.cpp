#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <maya/MGlobal.h>

#include "util.h"
#include "meshexporter.h"

#include "filianore/color/rgb.h"

#include "filianore/shapes/triangle.h"
#include "filianore/core/primitive.h"

#include "filianore/textures/constant.h"
#include "filianore/materials/lambert.h"

#include <map>

using namespace filianore;

using Vec3 = StaticArray<float, 3>;
using Vec2 = StaticArray<float, 2>;
using TriEntity = TriangleEntity;

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
            MStatus uvGetterStatus;

            Vec2 uv1(0.f, 0.f);
            Vec2 uv2(1.f, 0.f);
            Vec2 uv3(1.f, 1.f);
            float2 uv;
            MPoint pointForUV = MPoint(0.f, 0.f, 0.f);

            int vertexIds[3];
            _mesh.getPolygonTriangleVertices(polygon, i, vertexIds);

            Vec3 triV1 = Vec3(vertices[vertexIds[0]]);
            pointForUV = MPoint(triV1.x(), triV1.y(), triV1.z());
            _mesh.getUVAtPoint(pointForUV, uv, MSpace::kWorld);
            uv1 = Vec2(uv[0], uv[1]);

            Vec3 triV2 = Vec3(vertices[vertexIds[1]]);
            pointForUV = MPoint(triV2.x(), triV2.y(), triV2.z());
            _mesh.getUVAtPoint(pointForUV, uv, MSpace::kWorld);
            uv2 = Vec2(uv[0], uv[1]);

            Vec3 triV3 = Vec3(vertices[vertexIds[2]]);
            pointForUV = MPoint(triV3.x(), triV3.y(), triV3.z());
            _mesh.getUVAtPoint(pointForUV, uv, MSpace::kWorld);
            uv3 = Vec2(uv[0], uv[1]);

            Vec3 triN1 = Vec3(normals[verticesToNormals[vertexIds[0]]]);
            Vec3 triN2 = Vec3(normals[verticesToNormals[vertexIds[1]]]);
            Vec3 triN3 = Vec3(normals[verticesToNormals[vertexIds[2]]]);

            TriEntity t1(triV1, triN1, true, uv1);
            TriEntity t2(triV2, triN2, true, uv2);
            TriEntity t3(triV3, triN3, true, uv3);

            std::shared_ptr<Shape> triangle = std::make_shared<Triangle>(t1, t2, t3);

            RGBSpectrum col(0.5f);
            std::shared_ptr<Texture<RGBSpectrum>> tex = std::make_shared<ConstantTexture<RGBSpectrum>>(col);
            std::shared_ptr<Material> material = std::make_shared<LambertMaterial>(tex);

            std::shared_ptr<Primitive> primitive = std::make_shared<GeometricPrimitive>(triangle, material);

            primitives.emplace_back(primitive);
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
            MayaMesh mMesh(mesh);

            std::vector<std::shared_ptr<Primitive>> primitivesFromMesh = mMesh.primitives;

            prims.insert(prims.end(), primitivesFromMesh.begin(), primitivesFromMesh.end());
        }
    }

    return prims;
}