/*
 Copyright (C) 2026 MaxED -- Heretic II .m32 texture loader.

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mdl/LoadM32Texture.h"

#include "fs/Reader.h"
#include "fs/ReaderException.h"
#include "gl/Texture.h"

namespace tb::mdl
{
namespace M32Layout
{
constexpr int Version = 4;
constexpr size_t TextureNameLength = 128;
constexpr size_t TextureAltNameLength = 128;
constexpr size_t AnimNameLength = 128;
constexpr size_t DamageNameLength = 128;
constexpr size_t MipLevels = 16;
} // namespace M32Layout

Result<gl::Texture> loadM32Texture(fs::Reader& reader)
{
  try
  {
    const auto version = reader.readInt<int32_t>();
    if (version != M32Layout::Version)
    {
      return Error{"Unknown M32 texture version: " + std::to_string(version)};
    }

    reader.seekForward(
      M32Layout::TextureNameLength + M32Layout::TextureAltNameLength
      + M32Layout::AnimNameLength + M32Layout::DamageNameLength);

    auto widths = std::vector<size_t>{};
    auto heights = std::vector<size_t>{};
    auto offsets = std::vector<size_t>{}; // Offsets from the beginning of the file.

    widths.reserve(M32Layout::MipLevels);
    heights.reserve(M32Layout::MipLevels);
    offsets.reserve(M32Layout::MipLevels);

    for (size_t i = 0; i < M32Layout::MipLevels; i++)
    {
      widths.push_back(reader.readSize<uint32_t>());
    }
    for (size_t i = 0; i < M32Layout::MipLevels; i++)
    {
      heights.push_back(reader.readSize<uint32_t>());
    }
    for (size_t i = 0; i < M32Layout::MipLevels; i++)
    {
      offsets.push_back(reader.readSize<uint32_t>());
    }

    auto mip0AverageColor = Color{RgbaF{}};
    auto buffers = gl::TextureBufferList{};

    for (size_t mipLevel = 0; mipLevel < M32Layout::MipLevels; mipLevel++)
    {
      const auto w = widths[mipLevel];
      const auto h = heights[mipLevel];

      if (w == 0 || h == 0)
      {
        break;
      }

      reader.seekFromBegin(offsets[mipLevel]);

      const size_t pixelCount = w * h;
      const size_t dataSize = pixelCount * 4;

      auto rgbaImage = gl::TextureBuffer{dataSize};

      // Copy RGBA pixels.
      auto* const rgbaData = rgbaImage.data();
      reader.read(rgbaData, dataSize);

      buffers.emplace_back(std::move(rgbaImage));

      // Check average color.
      if (mipLevel == 0)
      {
        uint32_t colorSum[3] = {0, 0, 0};

        for (size_t i = 0; i < pixelCount; i++)
        {
          colorSum[0] += static_cast<uint32_t>(rgbaData[(i * 4) + 0]);
          colorSum[1] += static_cast<uint32_t>(rgbaData[(i * 4) + 1]);
          colorSum[2] += static_cast<uint32_t>(rgbaData[(i * 4) + 2]);
        }

        mip0AverageColor = RgbaF{
          static_cast<float>(colorSum[0]) / (255.0f * static_cast<float>(pixelCount)),
          static_cast<float>(colorSum[1]) / (255.0f * static_cast<float>(pixelCount)),
          static_cast<float>(colorSum[2]) / (255.0f * static_cast<float>(pixelCount)),
          1.0f};
      }
    }

    return gl::Texture{
      widths[0],
      heights[0],
      mip0AverageColor,
      GL_RGBA,
      gl::TextureMask::Off,
      gl::NoEmbeddedDefaults{},
      std::move(buffers)};
  }
  catch (const fs::ReaderException& e)
  {
    return Error{e.what()};
  }
}

} // namespace tb::mdl
