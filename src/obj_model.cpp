#include "obj_model.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>

ObjModel::~ObjModel() {
    destroy();
}

const unsigned int& ObjModel::getVAO() const {
    return VAO_;
}

const unsigned int& ObjModel::getVBO() const {
    return VBO_;
}

const GLsizei& ObjModel::getVertexCount() const {
    return vertexCount_;
}

void ObjModel::destroy() {
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

bool ObjModel::load(const std::string& path) {
    /* open the file for reading */
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open OBJ file: " << path << std::endl;
        return false;
    }

    /* destroy previous model when loading new one */
    destroy();
    materials_.clear();

    const MtlMaterial defaultMaterial;
    MtlMaterial currentMaterial = defaultMaterial;

    const size_t slash = path.find_last_of("/\\");
    const std::string objDirecory = (slash == std::string::npos) ? "" : path.substr(0, slash + 1);

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

        if (type.empty() || type[0] == '#' || type == "vp" || type == "l" || type == "s") {
            continue;
        }

        /* v = vertex */
        /* vertices have x, y, z, w values e.g.: v 0.123 0.234 0.345 1.0 */
        /* w is optional and controls the scale of the vertex, this program igonores it */
        if (type == "v") {
            float x, y, z;

            if (stream >> x >> y >> z) {
                positions.insert(positions.end(), {x, y, z});
            }
        }

        /* vt = vertex texture coordinate */
        /* tex coords consist of u, v, w values e.g: vt 0.500 1 [0] */
        /* w is for control points of rational trimming curves according to wikipedia  
         * i dont know enough about blender to understand what that means
         * or if the w value is supposed to be inside square brackets or not
         * but this program ignores it */
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
        /* faces are defined as references to previously read positions, tex coords, and normals */
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

                vertex.materialColour = currentMaterial.getDiffuseColour();

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
            /* this way of triangulating does not take into consideration concave polygons,
             * polygons whose edges cross and non planar polygons */
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

        else if (type == "mtllib") {
            std::string mtlFilename;

            while (stream >> mtlFilename) {
                const std::string mtlPath = objDirecory + mtlFilename;

                if (!MtlMaterial::load(mtlPath, materials_)) {
                    std::cerr << "Could not open MTL file: " << mtlPath << std::endl;
                }
            }

        }

        else if (type == "usemtl") {
            std::string materialName;

            if (stream >> materialName) {
                const std::map<std::string, MtlMaterial>::iterator it = materials_.find(materialName);

                if (it != materials_.end()) {
                    currentMaterial = it->second;
                }
                else {
                    std::cerr << "Material not found:" << materialName << "; using default material." << std::endl;
                    currentMaterial = defaultMaterial;
                }
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

    centerVector_ = centerObject(vertices);

    for (Vertex& vertex : vertices) {
        vertex.position += centerVector_;
    }

    vertexCount_ = static_cast<GLsizei>(vertices.size());

    /* creating vertex array and buffer */
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    /* binding the vao and vbo, making them active */
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    /* copying the vertices from the cpu to the gpu */
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(),
                 GL_STATIC_DRAW);

    /* configuring attribute location 0 as position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    /* configuring attribute location 1 for the normals */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    /* configuring attribute location 2 for texture coordinates */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(2);

    /* configuring attribute location 3 for vertex colours */
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, colour)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, materialColour)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
    return true;
}

void ObjModel::defaultCube() {
    destroy();
    std::cout << "No model provided, using default cube." << std::endl;
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     0.0f, 1.0f, 1.0f,

         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f, 
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);

    /* copy vertex array to buffer for opengl to use */
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* set the vertex attribute pointer */
    /* position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* texture coordinate */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /* colour */
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vertexCount_ = 36;
}

bool ObjModel::parseFaceVertex(const std::string& token, int& position, int& texcoord, int& normal) {
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

bool ObjModel::convertIndex(int index, std::size_t count, std:: size_t& result) {
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

bool ObjModel::storeVec3(const std::vector<float>& values, int index, Vec3& vertexComponent) {
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

bool ObjModel::storeVec2(const std::vector<float>& values, int index, Vec2& vertexComponent) {
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

void ObjModel::calculateNormal(const Vec3& a, const Vec3& b, const Vec3& c, Vec3& out) {
    /* building two edges of the triangle */
    const Vec3 ab = {b[0] - a[0], b[1] - a[1], b[2] - a[2]};
    const Vec3 ac = {c[0] - a[0], c[1] - a[1], c[2] - a[2]};

    out = cross(ab, ac);
    out = normalize(out);
}

void ObjModel::assignRandomTriangleColour(Vertex& a, Vertex& b, Vertex& c) {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<float> colourValue(0.2f, 0.7f);

    // const float colour[3] = {colourValue(rng), colourValue(rng), colourValue(rng)};

    const float grey = colourValue(rng);
    Vec3 colour{grey, grey, grey};

    a.colour = colour;
    b.colour = colour;
    c.colour = colour;
}

Vec3 ObjModel::centerObject(std::vector<Vertex>& vertices) {
    /* find smallest and largest vertex position */
    Vec3 minPos = { vertices[0].position[0], vertices[0].position[1], vertices[0].position[2] };
    Vec3 maxPos = minPos;

    for (const Vertex& vertex : vertices) {
        for (int axis = 0; axis < 3; ++axis) {
            minPos[axis] = std::min(minPos[axis], vertex.position[axis]);
            maxPos[axis] = std::max(maxPos[axis], vertex.position[axis]);
        }
    }

    return { -(minPos[0] + maxPos[0]) * 0.5f,
             -(minPos[1] + maxPos[1]) * 0.5f,
             -(minPos[2] + maxPos[2]) * 0.5f };
}

void ObjModel::generatePlanarUVs(std::vector<Vertex>& vertices) {
    if (vertices.empty()) {
        return;
    }

    Vec3 minPos = { vertices[0].position[0], vertices[0].position[1], vertices[0].position[2] };
    Vec3 maxPos = minPos;

    for (const Vertex& vertex : vertices) {
        for (int axis = 0; axis < 3; ++axis) {
            minPos[axis] = std::min(minPos[axis], vertex.position[axis]);
            maxPos[axis] = std::max(maxPos[axis], vertex.position[axis]);
        }
    }

    const Vec3 range = {
        maxPos[0] - minPos[0],
        maxPos[1] - minPos[1],
        maxPos[2] - minPos[2]
    };

    int uAxis; // = axes[0];
    int vAxis; // = axes[1];

    if (range.x >= range.y && range.x >= range.z) {
        uAxis = 0;
        if (range.y >= range.z) {
            vAxis = 1;
        }
        else {
            vAxis = 2;
        }
    }
    else if (range.y >= range.x && range.y >= range.z) {
        uAxis = 1;
        if (range.x >= range.z) {
            vAxis = 0;
        }
        else {
            vAxis = 2;
        }
    }
    else {
        uAxis = 2;
        if (range.x >= range.y) {
            vAxis = 0;
        }
        else {
            vAxis = 1;
        }
    }

    for (Vertex& vertex : vertices) {
        if (range[uAxis] > 0.0f) {
            vertex.uv[0] = (vertex.position[uAxis] - minPos[uAxis]) / range[uAxis];
        }
        else {
            vertex.uv[0] = 0.0f;
        }

        if (range[vAxis] > 0.0f) {
            vertex.uv[1] = (vertex.position[vAxis] - minPos[vAxis]) / range[vAxis];
        }
        else {
            vertex.uv[1] = 0.0f;
        }
    }
}

