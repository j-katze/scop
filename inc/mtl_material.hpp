#ifndef MTL_MATERIAL_HPP
#define MTL_MATERIAL_HPP

#include "vec3.hpp"

#include <string>
#include <map>

class MtlMaterial {
public:
    MtlMaterial();

    const Vec3& getAmbientColour() const;
    const Vec3& getDiffuseColour() const;
    const Vec3& getSpecularColour() const;
    const Vec3& getEmissiveColour() const;
    const Vec3& getTransmissionFilter() const;

    const float& getSpecularExponent() const;
    const float& getRefractionIndex() const;
    const float& getDissolve() const;

    const int& getilluminationModel() const;

    static bool load(const std::string& path, std::map<std::string, MtlMaterial>& materials);

private:
    Vec3  ambientColour_;      // Ka
    Vec3  diffuseColour_;      // Kd
    Vec3  specularColour_;     // Ks
    Vec3  emissiveColour_;     // Ke
    Vec3  transmissionFilter_; // Tf

    float specularExponent_;   // Ns
    float refractionIndex_;    // Ni
    float dissolve_;           // d; Tr, d = 1 - Tr

    int   illuminationModel_;  // illum
};

#endif
