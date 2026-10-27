#include "../include/glad/glad.h"
#include "BmpImage.hpp"

#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>

bool BmpImage::loadTexture(const std::string &path) {
    // int width, height, nrChannels;

    if (!loadFile(path)) {
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    /* set texture wrapping parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* set texture filtering parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image.rgb.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    return 1;
}

bool BmpImage::loadFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file" << path << std::endl;
        return false;
    }

    if (!readU16(file, fileHeader.signature) ||
        !readU32(file, fileHeader.fileSize)  ||
        !readU16(file, fileHeader.reserved1) ||
        !readU16(file, fileHeader.reserved2) ||
        !readU32(file, fileHeader.pixelOffset)) {
        std::cerr << "File is too short for BMP header" << std::endl;
        return false;
    }

    if (fileHeader.signature != 0x4D42) {
        std::cerr << "not a BMP file" << std::endl;
        return false;
    }

    if (!readU32(file, infoHeader.dibsize)         ||
        !readI32(file, infoHeader.width)           ||
        !readI32(file, infoHeader.height)          ||
        !readU16(file, infoHeader.colourPlanes)   ||
        !readU16(file, infoHeader.bitsPerPixel)    ||
        !readU32(file, infoHeader.compression)     ||
        !readU32(file, infoHeader.imageSize)       ||
        !readI32(file, infoHeader.xPixelsPerMeter) ||
        !readI32(file, infoHeader.yPixelsPerMeter) ||
        !readU32(file, infoHeader.colourTable)    ||
        !readU32(file, infoHeader.importantColours)) {
        std::cerr << "file is too short for a BMP info header" << std::endl;
        return false;
    }

    if (infoHeader.dibsize < 40) {
        std::cerr << "unsuported BMP header" << std::endl;
        return false;
    }

    if (infoHeader.width <= 0 || infoHeader.height <= 0 || infoHeader.height == std::numeric_limits<int32_t>::min()) {
        std::cerr << "invalid BMP dimensions" << std::endl;
        return false;
    }

    if (infoHeader.colourPlanes != 1 || infoHeader.bitsPerPixel != 24 || infoHeader.compression != 0) {
        std::cerr << "only uncompressed 24 bit BMP files are supported" << std::endl;
        return false;
    }

    /* negative height means rows are stored top to bottom */
    int absoluteHeight;
    if (infoHeader.height < 0) {
        absoluteHeight = -infoHeader.height;
    }
    else {
        absoluteHeight = infoHeader.height;
    }

    /* row = number of pixels times bytes per pixel */
    const uint64_t rowBytes = uint64_t(infoHeader.width) * (infoHeader.bitsPerPixel / 8);

    /* BMP rows are padded so their length is divisible by 4, this rounds up rowBytes to the next multiple of 4 */
    /* the second modulo handles the case where rowBytes is already divisible by 4 */
    const uint64_t padding = (4 - (rowBytes % 4)) % 4;
    const uint64_t paddedRowBytes = rowBytes + padding;

    /* total size of image without padding */
    const uint64_t pixelBytes = rowBytes * uint64_t(infoHeader.height);

    if (pixelBytes > image.rgb.max_size() ||
        paddedRowBytes > std::numeric_limits<size_t>::max()) {
        std::cerr << "BMP is too large" << std::endl;
        return false;
    }

    image.width = infoHeader.width;
    image.height = absoluteHeight;
    image.rgb.resize(static_cast<size_t>(pixelBytes));

    file.seekg(fileHeader.pixelOffset, std::ios::beg);
    if (!file) {
        image = {};
        std::cerr << "invalid BMP pixel data offset" << std::endl;
        return false;
    }

    std::vector<unsigned char> row(static_cast<size_t>(paddedRowBytes));

    for (int fileRow = 0; fileRow < absoluteHeight; ++fileRow) {
        if (!file.read(reinterpret_cast<char*>(row.data()), static_cast<std::streamsize>(row.size()))) {
            image = {};
            std::cerr << "unexpected end of BMP pixel data" << std::endl;
            return false;
        }

        /* if height is negative reverse row order, tow row first going down */
        int outputRow;
        if (infoHeader.height > 0) {
            outputRow = fileRow;
        }
        else {
            outputRow = absoluteHeight - 1 - fileRow;
        }

        /* pixels are stored in BGR order, reorder them to RGB */
        for (int x = 0; x < infoHeader.width; ++x) {
            /* read pixels in reverse order otherwise the image will be flipped */
            const size_t src = static_cast<size_t>(infoHeader.width - 1 - x) * (infoHeader.bitsPerPixel / 8);
            const size_t dst = (static_cast<size_t>(outputRow) * infoHeader.width + x) * (infoHeader.bitsPerPixel / 8);

            image.rgb[dst + 0] = row[src + 2];
            image.rgb[dst + 1] = row[src + 1];
            image.rgb[dst + 2] = row[src + 0];
        }
    }

    return true;
}

/* read 2 bytes from file and store them in a uint16_t */
bool BmpImage::readU16(std::ifstream& file, uint16_t& value) {
    unsigned char buffer[2];

    if (!file.read(reinterpret_cast<char*>(buffer), 2)) {
        return false;
    }
    value = uint16_t(buffer[0]) | (uint16_t(buffer[1]) << 8);
    return true;
}

/* read 4 bytes from file and store them in a uint32_t */
bool BmpImage::readU32(std::ifstream &file, uint32_t &value) {
    unsigned char buffer[4];

    if (!file.read(reinterpret_cast<char*>(buffer), 4)) {
        return false;
    }
    value = uint32_t(buffer[0]) | (uint32_t(buffer[1]) << 8) |
            (uint32_t(buffer[2]) << 16) | (uint32_t(buffer[3]) << 24);
    return true;
}

/* read uint32_t from file and then cast it to int32_t */
bool BmpImage::readI32(std::ifstream &file, int32_t &value) {
    uint32_t raw;

    if (!readU32(file, raw)) {
        return false;
    }
    value = static_cast<int32_t>(raw);
    return true;
}
