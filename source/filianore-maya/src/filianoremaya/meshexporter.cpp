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

using Vec3 = StaticArray<float, 3>;
using Vec2 = StaticArray<float, 2>;
using TriEntity = TriangleEntity;

MayaMesh::MayaMesh(MFnMesh &_mesh)
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

    // Get all Vertices in Mesh
    MPointArray mVertices;
    _mesh.getPoints(mVertices);
    // Parse Maya Vertices to Filianore's
    std::vector<Vec3> vertices;
    vertices.reserve(mVertices.length());
    for (unsigned int i = 0; i < mVertices.length(); i++)
    {
        MPoint point = mVertices[i] * mMatrix;
        vertices.emplace_back(Vec3((float)point.x, (float)point.y, (float)point.z));
    }

    // Get UV sets
    MStringArray UVsets;
    status = _mesh.getUVSetNames(UVsets);

    MIntArray mTrianglesCount;
    MIntArray mVerticesIndices;
    _mesh.getTriangles(mTrianglesCount, mVerticesIndices);
    for (int polygon = 0; polygon < _mesh.numPolygons(); ++polygon)
    {
        MIntArray vertexList;
        MIntArray faceNormalList;

        _mesh.getPolygonVertices(polygon, vertexList);

        for (int i = 0; i < mTrianglesCount[polygon]; ++i)
        {
            MStatus uvGetterStatus;

            // Get Polygon Vertices
            int vertexIds[3];
            _mesh.getPolygonTriangleVertices(polygon, i, vertexIds);

            // Get Polygon UVs
            float u1, v1, u2, v2, u3, v3;
            _mesh.getPolygonUV(polygon, vertexIds[0], u1, v1, &UVsets[0]);
            _mesh.getPolygonUV(polygon, vertexIds[1], u2, v2, &UVsets[0]);
            _mesh.getPolygonUV(polygon, vertexIds[2], u3, v3, &UVsets[0]);

            TriEntity t1(vertices[vertexIds[0]], Vec3(), false, Vec2(u1, v1));
            TriEntity t2(vertices[vertexIds[1]], Vec3(), false, Vec2(u2, v2));
            TriEntity t3(vertices[vertexIds[2]], Vec3(), false, Vec2(u3, v3));

            std::shared_ptr<Shape> triangle = std::make_shared<Triangle>(t1, t2, t3);
            std::shared_ptr<Primitive> primitive = std::make_shared<GeometricPrimitive>(triangle, material, nullptr);

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