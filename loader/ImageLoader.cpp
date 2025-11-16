#include<fstream>
#include<iostream>
#include<cstring>

#include "ImageLoader.h"

namespace loader
{
    misc::Texture ImageLoader::Load(const std::string& path_) {
        std::string ext = path_.substr(path_.find_last_of('.') + 1);

        for (auto& c :ext) c = tolower(c);

        if (ext == "bmp") return LoadBMP(path_);
        if (ext == "tga") return LoadTGA(path_);
        if (ext == "dds") return LoadBMP(path_);
        
        std::cerr << "Unsupported texture format: " << ext << "\n";
        return {};
    }

    misc::Texture ImageLoader::LoadBMP(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        misc::Texture tex;

        if (!file.is_open()) {
            std::cerr << "Failed to open BMP file: " << path << "\n";
            return tex;
        }

        uint16_t bfType;
        file.read((char*)&bfType, 2);

        if (bfType != 0x4D42) { // 'BM'
            std::cerr << "Not a BMP file!\n";
            return tex;
        }

        file.ignore(8); // skip file size + reserved
        uint32_t dataOffset;
        file.read((char*)&dataOffset, 4);

        uint32_t headerSize;
        file.read((char*)&headerSize, 4);

        int width;
        int height;

        file.read((char*)&width, 4);
        file.read((char*)&height, 4);

        tex.width = width;
        tex.height = height;
        tex.channels = 4; // output RGBA

        file.ignore(2); // planes
        uint16_t bpp;
        file.read((char*)&bpp, 2);

        if (bpp != 24 && bpp != 32) {
            std::cerr << "Unsupported BMP bit depth: " << bpp << "\n";
            return tex;
        }

        // jump to pixel array
        file.seekg(dataOffset, std::ios::beg);

        int rowSize = ((bpp * width + 31) / 32) * 4;
        int dataSize = rowSize * height;

        std::vector<uint8_t> raw(dataSize);
        file.read((char*)raw.data(), raw.size());

        tex.data.resize(width * height * 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int bmpIndex = y * rowSize + x * (bpp / 8);
                int texIndex = ((height - 1 - y) * width + x) * 4;

                uint8_t B = raw[bmpIndex + 0];
                uint8_t G = raw[bmpIndex + 1];
                uint8_t R = raw[bmpIndex + 2];
                uint8_t A = (bpp == 32) ? raw[bmpIndex + 3] : 255;

                tex.data[texIndex + 0] = R;
                tex.data[texIndex + 1] = G;
                tex.data[texIndex + 2] = B;
                tex.data[texIndex + 3] = A;
            }
        }

        std::cout << "Loaded BMP: " << width << "x" << height << "\n";
        return tex;
    }

    misc::Texture ImageLoader::LoadTGA(const std::string& path_)
    {
        misc::Texture tex;
        std::ifstream file(path_, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "[TGA] Failed to open: " << path_ << "\n";
            return tex; // empty
        }

        uint8_t header[18];
        file.read(reinterpret_cast<char*>(header), 18);
        if (file.gcount() != 18) {
            std::cerr << "[TGA] Header read failed: " << path_ << "\n";
            return tex;
        }

        uint8_t idLength     = header[0];
        uint8_t colorMapType = header[1];
        uint8_t imageType    = header[2]; // 2 = uncompressed RGB, 10 = RLE RGB
        uint16_t width       = header[12] | (header[13] << 8);
        uint16_t height      = header[14] | (header[15] << 8);
        uint8_t bpp          = header[16];
        uint8_t descriptor   = header[17];

        if (colorMapType != 0) {
            std::cerr << "[TGA] Color-mapped TGA not supported.\n";
            return tex;
        }

        if (imageType != 2 && imageType != 10) {
            std::cerr << "[TGA] Unsupported TGA image type: " << int(imageType) << "\n";
            return tex;
        }

        if (bpp != 24 && bpp != 32) {
            std::cerr << "[TGA] Unsupported BMP bit depth: " << int(bpp) << "\n";
            return tex;
        }

        // skip ID field if present
        if (idLength > 0) file.seekg(idLength, std::ios::cur);

        tex.width = width;
        tex.height = height;
        tex.channels = (bpp == 24) ? 3 : 4;
        tex.data.resize(size_t(width) * size_t(height) * tex.channels);

        const size_t pixelCount = size_t(width) * size_t(height);
        const size_t pixelSize = tex.channels;

        // Temporary buffer where file-ordered pixels will be decoded (BGR(A))
        std::vector<uint8_t> fileBuffer;
        fileBuffer.resize(pixelCount * pixelSize);

        if (imageType == 2) {
            // Uncompressed: read raw pixel data
            file.read(reinterpret_cast<char*>(fileBuffer.data()), fileBuffer.size());
            if ((size_t)file.gcount() != fileBuffer.size()) {
                // gcount might be less for some streams; do a robust check:
                if (file.fail()) {
                    std::cerr << "[TGA] Failed to read image data (uncompressed).\n";
                    return misc::Texture();
                }
            }
        } else {
            // RLE compressed (imageType == 10)
            size_t outIndex = 0;
            while (outIndex < fileBuffer.size() && !file.eof()) {
                uint8_t packetHeader = 0;
                file.read(reinterpret_cast<char*>(&packetHeader), 1);
                if (file.eof()) break;

                if ((packetHeader & 0x80) == 0) {
                    // raw packet: (packetHeader + 1) pixels follow
                    int count = (packetHeader & 0x7F) + 1;
                    for (int i = 0; i < count; ++i) {
                        if (outIndex + pixelSize > fileBuffer.size()) break;
                        file.read(reinterpret_cast<char*>(&fileBuffer[outIndex]), pixelSize);
                        outIndex += pixelSize;
                    }
                } else {
                    // RLE packet: read one pixel and repeat (packetHeader - 127) times
                    int count = (packetHeader & 0x7F) + 1;
                    std::vector<char> pixel(pixelSize);
                    file.read(pixel.data(), pixelSize);
                    for (int i = 0; i < count; ++i) {
                        if (outIndex + pixelSize > fileBuffer.size()) break;
                        std::memcpy(&fileBuffer[outIndex], pixel.data(), pixelSize);
                        outIndex += pixelSize;
                    }
                }
            }
            // if RLE decoder produced less than expected, that's an error but we try to continue
            if (outIndex < fileBuffer.size()) {
                std::cerr << "[TGA] Warning: RLE decoded fewer pixels than expected.\n";
            }
        }

        file.close();

        // TGA stores pixels as B,G,R,(A). Convert to R,G,B,(A) and handle vertical origin.
        bool originTop = (descriptor & 0x20) != 0; // bit 5 = top-left when set
        for (uint32_t y = 0; y < height; ++y) {
            uint32_t srcY = originTop ? y : (height - 1 - y); // if origin bottom-left -> flip
            const uint8_t* srcRow = &fileBuffer[size_t(srcY) * width * pixelSize];
            uint8_t* dstRow = &tex.data[size_t(y) * width * pixelSize];

            for (uint32_t x = 0; x < width; ++x) {
                const uint8_t* srcPx = &srcRow[size_t(x) * pixelSize];
                uint8_t* dstPx = &dstRow[size_t(x) * pixelSize];

                // BGR(A) -> RGB(A)
                dstPx[0] = srcPx[2];
                dstPx[1] = srcPx[1];
                dstPx[2] = srcPx[0];
                if (pixelSize == 4) dstPx[3] = srcPx[3];
            }
        }

        std::cout << "[TGA] Loaded: " << tex.width << "x" << tex.height
                << " channels=" << int(tex.channels)
                << " originTop=" << (originTop ? "yes" : "no") << "\n";

        return tex;
    }

}