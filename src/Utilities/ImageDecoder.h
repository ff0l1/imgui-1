#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Utilities
{
    struct DecodedFrame
    {
        std::vector<uint8_t> Pixels;
        float DelaySeconds = 0.0f;
    };

    struct DecodedImage
    {
        std::vector<DecodedFrame> Frames;
        int Width = 0;
        int Height = 0;

        bool IsValid() const { return Width > 0 && Height > 0 && !Frames.empty(); }
    };

    DecodedImage DecodeImage(const uint8_t* bytes, size_t size);
    void ConvertToWhiteSilhouette(DecodedImage& image);
    void ResampleImage(DecodedImage& image, int targetWidth, int targetHeight);
}
