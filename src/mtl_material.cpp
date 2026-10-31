#include "mtl_material.hpp"

#include <fstream>
#include <sstream>

MtlMaterial::MtlMaterial()
    :   ambientColour_{0.0f, 0.0f, 0.0f},
        diffuseColour_{0.8f, 0.8f, 0.8f},
        specularColour_{0.0f, 0.0f, 0.0f},
        emissiveColour_{0.0f, 0.0f, 0.0f},
        transmissionFilter_{1.0f, 1.0f, 1.0f},
        specularExponent_(0.0f),
        refractionIndex_(1.0f),
        dissolve_(1.0f),
        illuminationModel_(0) {}

const Vec3& MtlMaterial::getAmbientColour() const {
    return ambientColour_;
}

const Vec3& MtlMaterial::getDiffuseColour() const {
    return diffuseColour_;
}

const Vec3& MtlMaterial::getSpecularColour() const {
    return specularColour_;
}

const Vec3& MtlMaterial::getEmissiveColour() const {
    return emissiveColour_;
}

const Vec3& MtlMaterial::getTransmissionFilter() const {
    return transmissionFilter_;
}

const float& MtlMaterial::getSpecularExponent() const {
    return specularExponent_;
}

const float& MtlMaterial::getRefractionIndex() const {
    return refractionIndex_;
}

const float& MtlMaterial::getDissolve() const {
    return dissolve_;
}

const int& MtlMaterial::getilluminationModel() const {
    return illuminationModel_;
}

bool MtlMaterial::load(const std::string &path, std::map<std::string, MtlMaterial>& materials) {
    std::ifstream file(path);
    if (!file) {
        return false;
    }

    std::map<std::string, MtlMaterial> parsedMaterials;
    MtlMaterial* current = nullptr;
    std::string currentName;
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream input(line);
        std::string keyword;

        if (!(input >> keyword) || keyword[0] == '#') {
            continue;
        }

        if (keyword == "newmtl") {
            std::string name;
            if (!(input >> name)) {
                return false;
            }

            if (current != nullptr) {
                parsedMaterials[currentName] = *current;
            }

            currentName = name;
            MtlMaterial material;

            /* if material with same name already existed in the map replace that material with newly loaded material */
            std::pair<std::map<std::string, MtlMaterial>::iterator, bool> result =
                parsedMaterials.emplace(currentName, material);
            if (!result.second) {
                result.first->second = material;
            }
            current = &result.first->second;
            continue;
        }

        if (keyword == "Ka") {
            float r, g, b;
            if (!(input >> r >> g >> b)) {
                return false;
            }
            const Vec3 value {r, g, b};
            current->ambientColour_ = value;
        }
        else if (keyword == "Kd") {
            float r, g, b;
            if (!(input >> r >> g >> b)) {
                return false;
            }
            const Vec3 value {r, g, b};
            current->diffuseColour_ = value;
        }
        else if (keyword == "Ks") {
            float r, g, b;
            if (!(input >> r >> g >> b)) {
                return false;
            }
            const Vec3 value {r, g, b};
            current->specularColour_ = value;
        }
        else if (keyword == "Ke") {
            float r, g, b;
            if (!(input >> r >> g >> b)) {
                return false;
            }
            const Vec3 value {r, g, b};
            current->emissiveColour_ = value;
        }
        else if (keyword == "Tf") {
            float r, g, b;
            if (!(input >> r >> g >> b)) {
                return false;
            }
            const Vec3 value {r, g, b};
            current->transmissionFilter_ = value;
        }

        else if (keyword == "Ns") {
            float value;
            if (!(input >> value)) {
                return false;
            }
            current->specularExponent_ = value;
        }
        else if (keyword == "Ni") {
            float value;
            if (!(input >> value)) {
                return false;
            }
            current->refractionIndex_ = value;
        }
        else if (keyword == "d") {
            float value;
            if (!(input >> value)) {
                return false;
            }
            current->dissolve_ = value;
        }
        else if (keyword == "Tr") {
            float value;
            if (!(input >> value)) {
                return false;
            }
            current->dissolve_ = 1.0f - value;
        }

        else if (keyword == "illum") {
            if (!(input >> current->illuminationModel_)) {
                return false;
            }
        }
    }

    if (file.bad()) {
        return false;
    }

    if (current != nullptr) {
        parsedMaterials[currentName] = *current;
    }

    for (const auto& entry : parsedMaterials) {
        materials[entry.first] = entry.second;
    }

    return true;
}
