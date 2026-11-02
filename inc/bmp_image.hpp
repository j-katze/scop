#ifndef BMP_IMAGE_HPP
#define BMP_IMAGE_HPP

#include <cstdint>
#include <string>
#include <vector>

class BmpImage {
public:
    struct Image {
        int width;
        int height;
        std::vector<std::uint8_t> rgb;
    };

    struct FileHeader {
        uint16_t signature;

        uint32_t fileSize;

        uint16_t reserved1;
        uint16_t reserved2;

        uint32_t pixelOffset;
    };

    struct InfoHeader {
        uint32_t dibsize;

        int32_t  width;
        int32_t  height;

        uint16_t colourPlanes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;

        int32_t  xPixelsPerMeter;
        int32_t  yPixelsPerMeter;

        uint32_t colourTable;
        uint32_t importantColours;
    };

    BmpImage();
    ~BmpImage();
    BmpImage(const BmpImage&) = delete;
    BmpImage& operator=(const BmpImage&) = delete;

    const unsigned int& getTextureId() const;
    bool isImageDataEmpty() const;

    void destroy();

    bool loadTexture(const std::string& path);

    void loadDefaultTexture();

private:
    unsigned int textureID_;
    Image        image_;

    FileHeader   fileHeader_;
    InfoHeader   infoHeader_;

    void uploadImageToGPU();

    bool readU16(std::ifstream& file, uint16_t& value);
    bool readU32(std::ifstream& file, uint32_t& value);
    bool readI32(std::ifstream& file, int32_t& value);
};

#endif
