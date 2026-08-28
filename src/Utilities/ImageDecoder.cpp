#include "ImageDecoder.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_GIF
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#include "stb_image.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace Utilities
{
    namespace
    {
        constexpr float kDefaultFrameDelaySeconds = 0.05f;
        constexpr int kChannels = 4;

        bool HasGifSignature(const uint8_t* bytes, size_t size)
        {
            return size >= 6 && std::memcmp(bytes, "GIF8", 4) == 0;
        }

        DecodedImage DecodeAnimatedGif(const uint8_t* bytes, size_t size)
        {
            DecodedImage image;

            int* delays = nullptr;
            int width = 0;
            int height = 0;
            int frameCount = 0;
            int sourceChannels = 0;

            stbi_uc* data = stbi_load_gif_from_memory(
                bytes, static_cast<int>(size),
                &delays, &width, &height, &frameCount, &sourceChannels, kChannels);

            if (!data)
                return image;

            image.Width = width;
            image.Height = height;
            image.Frames.reserve(static_cast<size_t>(frameCount));

            const size_t frameBytes = static_cast<size_t>(width) * height * kChannels;
            for (int index = 0; index < frameCount; ++index)
            {
                DecodedFrame frame;
                frame.Pixels.assign(data + frameBytes * index, data + frameBytes * (index + 1));
                const int milliseconds = delays ? delays[index] : 0;
                frame.DelaySeconds = milliseconds > 0 ? milliseconds / 1000.0f : kDefaultFrameDelaySeconds;
                image.Frames.push_back(std::move(frame));
            }

            stbi_image_free(data);
            STBI_FREE(delays);

            return image;
        }

        DecodedImage DecodeStillImage(const uint8_t* bytes, size_t size)
        {
            DecodedImage image;

            int width = 0;
            int height = 0;
            int sourceChannels = 0;

            stbi_uc* data = stbi_load_from_memory(
                bytes, static_cast<int>(size),
                &width, &height, &sourceChannels, kChannels);

            if (!data)
                return image;

            image.Width = width;
            image.Height = height;

            DecodedFrame frame;
            frame.Pixels.assign(data, data + static_cast<size_t>(width) * height * kChannels);
            frame.DelaySeconds = kDefaultFrameDelaySeconds;
            image.Frames.push_back(std::move(frame));

            stbi_image_free(data);
            return image;
        }
    }

    DecodedImage DecodeImage(const uint8_t* bytes, size_t size)
    {
        if (!bytes || size == 0)
            return {};

        return HasGifSignature(bytes, size)
            ? DecodeAnimatedGif(bytes, size)
            : DecodeStillImage(bytes, size);
    }

    namespace
    {
        std::vector<uint8_t> ResampleFrame(
            const std::vector<uint8_t>& source, int sourceWidth, int sourceHeight,
            int targetWidth, int targetHeight)
        {
            std::vector<uint8_t> target(static_cast<size_t>(targetWidth) * targetHeight * kChannels);

            const float scaleX = static_cast<float>(sourceWidth) / targetWidth;
            const float scaleY = static_cast<float>(sourceHeight) / targetHeight;

            for (int y = 0; y < targetHeight; ++y)
            {
                const float sourceTop = y * scaleY;
                const float sourceBottom = (y + 1) * scaleY;
                const int firstRow = static_cast<int>(sourceTop);
                const int lastRow = std::min(static_cast<int>(std::ceil(sourceBottom)), sourceHeight);

                for (int x = 0; x < targetWidth; ++x)
                {
                    const float sourceLeft = x * scaleX;
                    const float sourceRight = (x + 1) * scaleX;
                    const int firstColumn = static_cast<int>(sourceLeft);
                    const int lastColumn = std::min(static_cast<int>(std::ceil(sourceRight)), sourceWidth);

                    float accumulated[kChannels] = { 0.0f, 0.0f, 0.0f, 0.0f };
                    float totalWeight = 0.0f;

                    for (int row = firstRow; row < lastRow; ++row)
                    {
                        const float rowWeight =
                            std::min(sourceBottom, static_cast<float>(row + 1)) -
                            std::max(sourceTop, static_cast<float>(row));

                        for (int column = firstColumn; column < lastColumn; ++column)
                        {
                            const float columnWeight =
                                std::min(sourceRight, static_cast<float>(column + 1)) -
                                std::max(sourceLeft, static_cast<float>(column));

                            const float weight = rowWeight * columnWeight;
                            if (weight <= 0.0f)
                                continue;

                            const size_t offset = (static_cast<size_t>(row) * sourceWidth + column) * kChannels;
                            const float alpha = source[offset + 3] / 255.0f;

                            accumulated[0] += source[offset + 0] * alpha * weight;
                            accumulated[1] += source[offset + 1] * alpha * weight;
                            accumulated[2] += source[offset + 2] * alpha * weight;
                            accumulated[3] += source[offset + 3] * weight;
                            totalWeight += weight;
                        }
                    }

                    const size_t targetOffset = (static_cast<size_t>(y) * targetWidth + x) * kChannels;
                    if (totalWeight <= 0.0f)
                        continue;

                    const float averageAlpha = accumulated[3] / totalWeight;
                    const float normalizedAlpha = averageAlpha / 255.0f;

                    for (int channel = 0; channel < 3; ++channel)
                    {
                        const float value = normalizedAlpha > 0.0f
                            ? (accumulated[channel] / totalWeight) / normalizedAlpha
                            : 0.0f;
                        target[targetOffset + channel] = static_cast<uint8_t>(std::clamp(value, 0.0f, 255.0f));
                    }
                    target[targetOffset + 3] = static_cast<uint8_t>(std::clamp(averageAlpha, 0.0f, 255.0f));
                }
            }

            return target;
        }
    }

    void ResampleImage(DecodedImage& image, int targetWidth, int targetHeight)
    {
        if (!image.IsValid() || targetWidth <= 0 || targetHeight <= 0)
            return;

        if (image.Width == targetWidth && image.Height == targetHeight)
            return;

        for (DecodedFrame& frame : image.Frames)
            frame.Pixels = ResampleFrame(frame.Pixels, image.Width, image.Height, targetWidth, targetHeight);

        image.Width = targetWidth;
        image.Height = targetHeight;
    }

    void ConvertToWhiteSilhouette(DecodedImage& image)
    {
        for (DecodedFrame& frame : image.Frames)
        {
            for (size_t offset = 0; offset + 3 < frame.Pixels.size(); offset += 4)
            {
                frame.Pixels[offset + 0] = 255;
                frame.Pixels[offset + 1] = 255;
                frame.Pixels[offset + 2] = 255;
            }
        }
    }
}
