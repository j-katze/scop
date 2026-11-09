#ifndef OBJ_MODEL_HPP
#define OBJ_MODEL_HPP

#include "glad.h"

#include "vec3.hpp"
#include "vec2.hpp"
#include "mtl_material.hpp"

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

class ObjModel {
public:
    struct Vertex {
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
        Vec3 colour;
        /* attributes loaded from .mtl file */
        Vec3 materialColour;
    };

    ObjModel();
    ~ObjModel();

    ObjModel(const ObjModel&) = delete;
    ObjModel& operator=(const ObjModel&) = delete;

    const unsigned int& getVAO() const;
    const unsigned int& getVBO() const;
    const GLsizei&      getVertexCount() const;

    /* delete models vao and vbo */
    void destroy();

    /* loads model from .obj file */
    bool load(const std::string& path);

    /* draw the model */
    // void draw() const;

    /* default cube in case no object was loaded */
    bool defaultCube();

private:
    std::string                        name_;
    std::map<std::string, MtlMaterial> materials_;
    std::vector<Vertex>                vertices_;

    unsigned int                       VAO_;
    unsigned int                       VBO_;
    GLsizei                            vertexCount_;
    Vec3                               centerVector_;

    bool uploadMeshToGpu();

    void addCubeFace(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3&d,
                     const Vec3& normal,
                     const Vec3& vertexColourA, const Vec3& vertexColourB,
                     const Vec3& vertexColourC, const Vec3& vertexColourD,
                     const Vec3& materialColour);

    /* extract position, texture coordinate and normal from a face */
    static bool parseFaceVertex(const std::string& token, int& position, int& texcoord, int& normal);

    /* vertices, tex coords and normals can be referenced either by their absolute position:  
     * 1 = first defined vertex, N = Nth defined vertex  
     * or by their position relative to the end of the list of defined vetices:  
     * -1 = last defined vertex, -N = vertex count - N defined vertex */
    static bool convertIndex(int index, std::size_t count, std:: size_t& result);

    /* stores position, texture coordinate, and normal values in vertex
     * using array of read in values and indexes pointing to those values */
    static bool storeVec3(const std::vector<float>& values, int index, Vec3& vertexComponent);

    /* same as storeVec3 but for uv coordinates which only have two values */
    static bool storeVec2(const std::vector<float>& values, int index, Vec2& vertexComponent);

    /* uses cross product to calculate a tris normal, then normalizes it */
    static void calculateNormal(const Vec3& a, const Vec3& b, const Vec3& c, Vec3& out);

    /* assigns a random colour to a tri */
    static void assignRandomTriangleColour(Vertex& a, Vertex& b, Vertex& c);

    static Vec3 centerObject(std::vector<Vertex>& vertices);

    static void generatePlanarUVs(std::vector<Vertex>& vertices);
};

#endif
