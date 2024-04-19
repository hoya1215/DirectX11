#pragma once

#include "Headers.h"

#include "AnimationClip.h"
#include "MeshData.h"
#include "Vertex.h"



class GeometryGenerator {
  public:

    static vector<MeshData> ReadFromFile(std::string basePath,
                                         std::string filename,
                                         bool revertNormals = false);

    static auto ReadAnimationFromFile(string basePath, string filename,
                                      bool revertNormals = false)
        -> tuple<vector<MeshData>, AnimationData>;

    static void Normalize(const Vector3 center, const float longestLength,
                          vector<MeshData> &meshes, AnimationData &aniData);

    static MeshData MakeSquare(const float scale = 1.0f,
                               const Vector2 texScale = Vector2(1.0f));

    static MeshData MakeCircle(const float scale = 1.0f,
                               const Vector2 texScale = Vector2(1.0f));
    static MeshData MakeSquareGrid(const int numSlices, const int numStacks,
                                   const float scale = 1.0f,
                                   const Vector2 texScale = Vector2(1.0f));

    static MeshData MakeBridge(const int numStacks, const float scale = 1.0f,
                               const Vector2 texScale = Vector2(1.0f));

    static MeshData MakeBox(const float scale = 1.0f);
    static MeshData MakeCylinder(const float bottomRadius,
                                 const float topRadius, float height,
                                 int numSlices);
    static MeshData MakeSphere(const float radius, const int numSlices,
                               const int numStacks,
                               const Vector2 texScale = Vector2(1.0f));

    static MeshData MakeWireBBox(const Vector3 center, const Vector3 extent);

    static MeshData MakeGrass();
    static MeshData MakeTetrahedron();
    static MeshData MakeIcosahedron();
    static MeshData SubdivideToSphere(const float radius, MeshData meshData);
};

