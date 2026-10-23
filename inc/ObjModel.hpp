#pragma once

#ifndef ObjModel_hpp
#define ObjModel_hpp

#include "../include/glad/glad.h"

#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>

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

    ObjModel() = default;
    ~ObjModel() { destroy(); }

    ObjModel(const ObjModel&) = delete;
    ObjModel& operator=(const ObjModel&) = delete;

    /* loads model from .obj file */
    bool load(const std::string& path) {
        /* destroy previous model when loading new one, currently loading a new model is not implemented */
        destroy();

        /* open the file for reading */
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Could not open OBJ file: " << path << std::endl;
            return false;
        }

        std::vector<float> positions;
        std::vector<float> texcoords;
        std::vector<float> normals;
        std::vector<Vertex> vertices;
        bool hasUVs = false;

        std::string line;

        while (std::getline(file, line)) {
            std::istringstream stream(line);
            std::string type;
            stream >> type;

            /* skipping empty lines and comments */
            if (type.empty() || type[0] == '#') {
                continue;
            }

            /* v = vertex */
            /* vertices have x, y, z, w values e.g.: v 0.123 0.234 0.345 1.0; w is optional and controls the scale of the vertex, this program igonores it */
            if (type == "v") {
                float x, y, z;

                if (stream >> x >> y >> z) {
                    positions.insert(positions.end(), {x, y, z});
                }
            }

            /* vt = vertex texture coordinate */
            /* tex coords consist of u, v, w values e.g: vt 0.500 1 [0]; w is for control points of rational trimming curves according to wikipedia*/
            /* i dont know enough about blender to understand what that means or if the w value is supposed to be inside square brackets or not */
            /* but this program ignores it */
            else if (type == "vt") {
                float u, v;

                if (stream >> u >> v) {
                    texcoords.insert(texcoords.end(), {u, v});
                }
            }

            /* vn = vertex normal */
            /* normals have x, y, z values e.g: vn 0.707 0.000 0.707 */
            else if (type == "vn") {
                float x, y, z;

                if (stream >> x >> y >> z) {
                    normals.insert(normals.end(), {x, y, z});
                }
            }

            /* f = face */
            /* faces are defined references to previously read positions, tex coords, and normals */
            /* there are several possible ways in which a face line can be structured: */
            /* vertices only: f v1 v2 v3 */
            /* vertices and tex coords: f v1/vt1 v2/vt2 v3/vt3 */
            /* vertices, tex coords and normals: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 */
            /* vertices and normals but no tex coords: f v1//vn1 v2//vn2 v3//vn3 */
            else if (type == "f") {
                std::vector<Vertex> face;
                std::string token;
                bool faceIsValid = true;

                while (stream >> token) {
                    Vertex vertex{};
                    int positionIndex = 0;
                    int texcoordIndex = 0;
                    int normalIndex = 0;

                    /* extracting parameters from token and making sure face is useable */
                    if (!parseFaceVertex(token, positionIndex, texcoordIndex, normalIndex)) {
                        faceIsValid = false;
                        break;
                    }
                    /* storing positions in vertex */
                    if (!storeVec3(positions, positionIndex, vertex.position)) {
                        faceIsValid = false;
                        break;
                    }
                    /* if tex coords are supplied store them in the vertex */
                    if (texcoordIndex != 0) {
                        if (storeVec2(texcoords, texcoordIndex, vertex.uv)) {
                            hasUVs = true;
                        }
                    }
                    /* if a normal is supplied store it in the vertex */
                    if (normalIndex != 0) {
                        storeVec3(normals, normalIndex, vertex.normal);
                    }
                    /* adding the vertex to the face */
                    face.push_back(vertex);
                }

                /* only continue if the face is properly formatted */
                if (!faceIsValid || face.size() < 3) {
                    std::cerr << "Skipping invalid OBJ face." << std::endl;
                    continue;
                }

                /* triangulating the face */
                /* vertices of a face are listed in clockwise or counterclockwise order */
                /* this way of triangulating does not take into consideration concave polygons, polygons whose edges cross and non planar polygons */
                for (std::size_t i = 1; i + 1 < face.size(); ++i) {
                    Vertex a = face[0];
                    Vertex b = face[i];
                    Vertex c = face[i + 1];

                    /* generate normal for this face if at least one of the faces vertices is missing them */
                    if (a.normal.isZero() || b.normal.isZero() || c.normal.isZero()) {
                        Vec3 normal;

                        calculateNormal(a.position, b.position, c.position, normal);

                        if (a.normal.isZero()) {
                            a.normal = normal;
                        }
                        if (b.normal.isZero()) {
                            b.normal = normal;
                        }
                        if (c.normal.isZero()) {
                            c.normal = normal;
                        }
                    }

                    assignRandomTriangleColour(a, b, c);

                    vertices.push_back(a);
                    vertices.push_back(b);
                    vertices.push_back(c);
                }
            }
        }

        /* error in case no valid faces could be created */
        if (vertices.empty()) {
            std::cerr << "OBJ contains no useable faces: " << path << std::endl;
            return false;
        }

        if (!hasUVs) {
            generatePlanarUVs(vertices);
        }

        centerObject(vertices);

        vertexCount_ = static_cast<GLsizei>(vertices.size());

        /* creating vertex array and buffer */
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        /* binding the vao and vbo, making them active */
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        /* copying the vertices from the cpu to the gpu */
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);

        /* configuring attribute location 0 as position */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(0);

        /* configuring attribute location 1 for the normals */
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);

        /* configuring attribute location 2 for texture coordinates */
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
        glEnableVertexAttribArray(2);

        /* configuring attribute location 3 for vertex colours */
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, colour)));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
        return true;
    }

    /* draw the model */
    void draw() const {
        if (VBO_ == 0) {
            return;
        }

        glBindVertexArray(VAO_);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
        glBindVertexArray(0);
    }

private:
    unsigned int VAO_, VBO_;
    GLsizei vertexCount_ = 0;

    // delete models vao and vbo
    void destroy() {
        if (VBO_ != 0) {
            glDeleteBuffers(1, &VBO_);
        }
        if (VAO_ != 0) {
            glDeleteVertexArrays(1, &VAO_);
        }
        VAO_ = 0;
        VBO_ = 0;
        vertexCount_ = 0;
    }

    /* extract position, texture coordinate and normal from a face */
    static bool parseFaceVertex(const std::string& token, int& position, int& texcoord, int& normal) {
        position = texcoord = normal = 0;

        std::size_t firstSlash = token.find('/');
        /* if there is no / token is on vertex only format */
        if (firstSlash == std::string::npos) {
            position = std::atoi(token.c_str());

            /* return true if position is non zero */
            return position != 0;
        }

        position = std::atoi(token.substr(0, firstSlash).c_str());

        std::size_t secondSlash = token.find('/', firstSlash + 1);
        /* if there is no second / the token is in v/vt format */
        if (secondSlash == std::string::npos) {
            texcoord = std::atoi(token.substr(firstSlash + 1).c_str());
        }

        /* if there is a second / token could be in v//vn or v/vt/vn format */
        else {
            if (secondSlash > firstSlash + 1) {
                texcoord = std::atoi(token.substr(firstSlash + 1, secondSlash - firstSlash - 1).c_str());
            }

            normal = std::atoi(token.substr(secondSlash + 1).c_str());
        }

        return position != 0;
    }

    /* vertices, tex coords and normals can be referenced either by their absolute position */
    /* 1 = first defined vertex, N = Nth defined vertex */
    /* or by their position relative to the end of the list of defined vetices */
    /* -1 = last defined vertex, -N = vertex count - N defined vertex */
    static bool convertIndex(int index, std::size_t count, std:: size_t& result) {
        /* if the index is positive we convert it to being zero indexed */
        if (index > 0) {
            result = static_cast<std::size_t>(index - 1);
        }

        /* if the index is negative add it to to count (C + -I == C - I) */
        else if (index < 0) {
            long converted = static_cast<long>(count) + index;
            /* if count - index is negative the index points to a non existent position */
            if (converted < 0) {
                return false;
            }
            result = static_cast<std::size_t>(converted);
        }
        else {
            return false;
        }

        /* return true if result is smaller than count */
        return result < count;
    }

    /* stores position, texture coordinate, and normal values in vertex using array of read in values and indexes pointing to those values */
    static bool storeVec3(const std::vector<float>& values, int index, Vec3& vertexComponent) {
        std::size_t convertedIndex;
        const std::size_t count = values.size() / 3;

        if (!convertIndex(index, count, convertedIndex)) {
            return false;
        }

        for (int i = 0; i < 3; ++i) {
            vertexComponent[i] = values[convertedIndex * 3 + i];
        }

        return true;
    }

    /* same as storeVec3 but for uv coordinates which only have two values */
    static bool storeVec2(const std::vector<float>& values, int index, Vec2& vertexComponent) {
        std::size_t convertedIndex;
        const std::size_t count = values.size() / 2;

        if (!convertIndex(index, count, convertedIndex)) {
            return false;
        }

        for (int i = 0; i < 2; ++i) {
            vertexComponent[i] = values[convertedIndex * 2 + i];
        }

        return true;
    }

    /* uses cross product to calculate a tris normal, then normalizes it */
    static void calculateNormal(const Vec3& a, const Vec3& b, const Vec3& c, Vec3& out) {
        /* building two edges of the triangle */
        const Vec3 ab = {b[0] - a[0], b[1] - a[1], b[2] - a[2]};
        const Vec3 ac = {c[0] - a[0], c[1] - a[1], c[2] - a[2]};

        out = cross(ab, ac);
        out = normalize(out);
    }

    /* assigns a random colour to a tri */
    static void assignRandomTriangleColour(Vertex& a, Vertex& b, Vertex& c) {
        static std::mt19937 rng{std::random_device{}()};
        static std::uniform_real_distribution<float> colourValue(0.2f, 0.7f);

        // const float colour[3] = {colourValue(rng), colourValue(rng), colourValue(rng)};

        const float grey = colourValue(rng);
        Vec3 colour{grey, grey, grey};

        a.colour = colour;
        b.colour = colour;
        c.colour = colour;
    }

    static void centerObject(std::vector<Vertex>& vertices) {
        float minpos[3] = { vertices[0].position[0], vertices[0].position[1], vertices[0].position[2] };
        float maxpos[3] = { minpos[0], minpos[1], minpos[2] };

        for (const Vertex& vertex : vertices) {
            for (int axis = 0; axis < 3; ++axis) {
                minpos[axis] = std::min(minpos[axis], vertex.position[axis]);
                maxpos[axis] = std::max(maxpos[axis], vertex.position[axis]);
            }
        }

        const float center[3] = {
            (minpos[0] + maxpos[0]) * 0.5f,
            (minpos[1] + maxpos[1]) * 0.5f,
            (minpos[2] + maxpos[2]) * 0.5f
        };

        for (Vertex& vertex : vertices) {
            for (int axis = 0; axis < 3; ++axis) {
                vertex.position[axis] -= center[axis];
            }
        }
    }

    static void generatePlanarUVs(std::vector<Vertex>& vertices) {
        if (vertices.empty()) {
            return;
        }

        float minPos[3] = { vertices[0].position[0], vertices[0].position[1], vertices[0].position[2] };
        float maxPos[3] = { minPos[0], minPos[1], minPos[2] };

        for (const Vertex& vertex : vertices) {
            for (int axis = 0; axis < 3; ++axis) {
                minPos[axis] = std::min(minPos[axis], vertex.position[axis]);
                maxPos[axis] = std::max(maxPos[axis], vertex.position[axis]);
            }
        }

        const float range[3] = {
            maxPos[0] - minPos[0],
            maxPos[1] - minPos[1],
            maxPos[2] - minPos[2]
        };

        int axes[3] = {0, 1, 2};
        std::sort(axes, axes + 3, [&](int a, int b) { return range[a] > range[b]; });

        const int uAxis = axes[0];
        const int vAxis = axes[1];

        for (Vertex& vertex : vertices) {
            vertex.uv[0] = range[uAxis] > 0.0f ? (vertex.position[uAxis] - minPos[uAxis]) / range[uAxis] : 0.0f;
            vertex.uv[1] = range[vAxis] > 0.0f ? (vertex.position[vAxis] - minPos[vAxis]) / range[vAxis] : 0.0f;
        }
    }
};

#endif

