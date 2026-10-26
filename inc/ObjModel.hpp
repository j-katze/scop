#pragma once

#ifndef ObjModel_hpp
#define ObjModel_hpp

#include "../include/glad/glad.h"

#include <cstddef>
#include <string>
#include <vector>
#include <cstdlib>

#include "vec3.hpp"
#include "vec2.hpp"

class ObjModel {
public:
    struct Vertex {
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
        Vec3 colour;
    };

    Vec3 centerVector;

    ObjModel() = default;
    ~ObjModel() { destroy(); }

    ObjModel(const ObjModel&) = delete;
    ObjModel& operator=(const ObjModel&) = delete;

    /* loads model from .obj file */
    bool load(const std::string& path);

    /* draw the model */
    void draw() const;

    /* default cube in case no object was loaded */
    void defaultCube();

private:
    unsigned int VAO_, VBO_;
    GLsizei vertexCount_ = 0;

    /* delete models vao and vbo */
    void destroy();

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

