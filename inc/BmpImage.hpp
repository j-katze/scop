#pragma once

#include <cstdint>
#include <string>
#include <vector>

class BmpImage {
public:
    unsigned int textureID;

    struct Image {
        int width = 0;
        int height = 0;
        std::vector<std::uint8_t> rgb;
    };

    Image image;

    bool loadTexture(const std::string& path);

private:
    struct FileHeader {
        uint16_t signature;

        uint32_t fileSize;

        uint16_t reserved1;
        uint16_t reserved2;

        uint32_t pixelOffset;
    };

    struct InfoHeader {
        uint32_t dibsize;

        int32_t width;
        int32_t height;

        uint16_t colourPlanes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;

        int32_t xPixelsPerMeter;
        int32_t yPixelsPerMeter;

        uint32_t colourTable;
        uint32_t importantColours;
    };

    FileHeader fileHeader;
    InfoHeader infoHeader;

    bool loadFile(const std::string& path);
    bool readU16(std::ifstream& file, uint16_t& value);
    bool readU32(std::ifstream& file, uint32_t& value);
    bool readI32(std::ifstream& file, int32_t& value);
};
