#include "bmp_image.hpp"
#include "gl_functions.hpp"
#include "gl_errors.hpp"

#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>

BmpImage::BmpImage()
    :   textureID_(0),
        image_{},
        fileHeader_{},
        infoHeader_{}
{}

BmpImage::~BmpImage() {
    destroy();
}

const unsigned int& BmpImage::getTextureId() const {
    return textureID_;
}

bool BmpImage::isImageDataEmpty() const {
    return image_.rgb.empty();
}

void BmpImage::destroy() {
    if (textureID_ != 0) {
        gl.DeleteTextures(1, &textureID_);
        textureID_ = 0;
    }
}

bool BmpImage::loadTexture(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file" << path << std::endl;
        return false;
    }

    if (!readU16(file, fileHeader_.signature) ||
        !readU32(file, fileHeader_.fileSize)  ||
        !readU16(file, fileHeader_.reserved1) ||
        !readU16(file, fileHeader_.reserved2) ||
        !readU32(file, fileHeader_.pixelOffset))
    {
        std::cerr << "File is too short for BMP header" << std::endl;
        return false;
    }

    if (fileHeader_.signature != 0x4D42) {
        std::cerr << "not a BMP file" << std::endl;
        return false;
    }

    if (!readU32(file, infoHeader_.dibsize)         ||
        !readI32(file, infoHeader_.width)           ||
        !readI32(file, infoHeader_.height)          ||
        !readU16(file, infoHeader_.colourPlanes)    ||
        !readU16(file, infoHeader_.bitsPerPixel)    ||
        !readU32(file, infoHeader_.compression)     ||
        !readU32(file, infoHeader_.imageSize)       ||
        !readI32(file, infoHeader_.xPixelsPerMeter) ||
        !readI32(file, infoHeader_.yPixelsPerMeter) ||
        !readU32(file, infoHeader_.colourTable)     ||
        !readU32(file, infoHeader_.importantColours))
    {
        std::cerr << "file is too short for a BMP info header" << std::endl;
        return false;
    }

    if (infoHeader_.dibsize < 40) {
        std::cerr << "unsuported BMP header" << std::endl;
        return false;
    }

    if (infoHeader_.width <= 0 ||
        infoHeader_.height == 0 ||
        infoHeader_.height == std::numeric_limits<int32_t>::min())
    {
        std::cerr << "invalid BMP dimensions" << std::endl;
        return false;
    }

    if (infoHeader_.colourPlanes != 1 || infoHeader_.bitsPerPixel != 24 || infoHeader_.compression != 0) {
        std::cerr << "only uncompressed 24 bit BMP files are supported" << std::endl;
        return false;
    }

    /* negative height means rows are stored top to bottom */
    int absoluteHeight;
    if (infoHeader_.height < 0) {
        absoluteHeight = -infoHeader_.height;
    }
    else {
        absoluteHeight = infoHeader_.height;
    }

    /* row = number of pixels times bytes per pixel */
    const uint64_t rowBytes = uint64_t(infoHeader_.width) * (infoHeader_.bitsPerPixel / 8);

    /* BMP rows are padded so their length is divisible by 4, this rounds up rowBytes to the next multiple of 4 */
    /* the second modulo handles the case where rowBytes is already divisible by 4 */
    const uint64_t padding = (4 - (rowBytes % 4)) % 4;
    const uint64_t paddedRowBytes = rowBytes + padding;

    /* total size of image without padding */
    const uint64_t pixelBytes = rowBytes * static_cast<uint64_t>(absoluteHeight);

    if (pixelBytes > image_.rgb.max_size() ||
        paddedRowBytes > std::numeric_limits<size_t>::max() ||
        paddedRowBytes > static_cast<uint64_t>(std::numeric_limits<std::streamsize>::max())) {
        std::cerr << "BMP is too large" << std::endl;
        return false;
    }

    image_.width = infoHeader_.width;
    image_.height = absoluteHeight;
    image_.rgb.resize(static_cast<size_t>(pixelBytes));

    file.seekg(fileHeader_.pixelOffset, std::ios::beg);
    if (!file) {
        image_ = {};
        std::cerr << "invalid BMP pixel data offset" << std::endl;
        return false;
    }

    std::vector<unsigned char> row(static_cast<size_t>(paddedRowBytes));

    for (int fileRow = 0; fileRow < absoluteHeight; ++fileRow) {
        if (!file.read(reinterpret_cast<char*>(row.data()), static_cast<std::streamsize>(row.size()))) {
            image_ = {};
            std::cerr << "unexpected end of BMP pixel data" << std::endl;
            return false;
        }

        /* if height is negative reverse row order, tow row first going down */
        int outputRow;
        if (infoHeader_.height > 0) {
            outputRow = fileRow;
        }
        else {
            outputRow = absoluteHeight - 1 - fileRow;
        }

        /* pixels are stored in BGR order, reorder them to RGB */
        for (int x = 0; x < infoHeader_.width; ++x) {
            /* read pixels in reverse order otherwise the image will be flipped */
            const size_t src = static_cast<size_t>(x) * (infoHeader_.bitsPerPixel / 8);
            const size_t dst =
                (static_cast<size_t>(outputRow) * infoHeader_.width + x) * (infoHeader_.bitsPerPixel / 8);

            image_.rgb[dst + 0] = row[src + 2];
            image_.rgb[dst + 1] = row[src + 1];
            image_.rgb[dst + 2] = row[src + 0];
        }
    }

    std::cout << "loaded texture from: " << path << std::endl;

    return uploadImageToGPU();
}

bool BmpImage::loadDefaultTexture() {
    std::cout << "No texture provided, using default pink texture." << std::endl;

    image_.width  = 1;
    image_.height = 1;
    image_.rgb    = {255, 0, 255};

    return uploadImageToGPU();
}

bool BmpImage::uploadImageToGPU() {
    GLuint newTexture = 0;
    /* creating texture object */
    gl.GenTextures(1, &newTexture);
    if (newTexture == 0) {
        std::cerr << "Failed to create OpenGL texture" << std::endl;
        return false;
    }

    /* binding the texture object */
    gl.BindTexture(GL_TEXTURE_2D, newTexture);

    /* set texture wrapping parameters */
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* set texture filtering parameters */
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* set pixel rows to 1 byte al so tightly packed rgb data is read correctly */
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* uploading the texture data to the GPU */
    gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_.width, image_.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image_.rgb.data());

    /* generating mipmap for the texture */
    gl.GenerateMipmap(GL_TEXTURE_2D);

    /* resetting unpack alignment to default */
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, 4);

    /* unbinding the texture object */
    gl.BindTexture(GL_TEXTURE_2D, 0);

    if (reportGlErrors("texture upload")) {
        gl.DeleteTextures(1, &newTexture);
        return false;
    }
    if (textureID_ != 0) {
        gl.DeleteTextures(1, &textureID_);
    }
    textureID_ = newTexture;
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
