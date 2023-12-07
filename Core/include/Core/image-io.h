//
// Created by creeper on 12/7/23.
//

#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include <tinyexr.h>
#include <Core/spectrums.h>
#include <cstring>
#include <array>
#include <vector>
#include <iostream>

namespace rdcraft {
class ImageIO {
  public:
    ImageIO(int w, int h)
      : width(w), height(h) {
      pixels[0].resize(width * height, 0.0);
      pixels[1].resize(width * height, 0.0);
      pixels[2].resize(width * height, 0.0);
    }
    void write(int x, int y, const Spectrum& rgb) {
      if (x >= 0 && x < width && y >= 0 && y < height) {
        int index = y * width + x;
        pixels[0][index] = rgb.r;
        pixels[1][index] = rgb.g;
        pixels[2][index] = rgb.b;
      } else {
        std::cerr << "Pixel coordinates out of bounds" << std::endl;
      }
    }

    bool exportEXR(const char* filename) {
      EXRHeader header;
      InitEXRHeader(&header);

      EXRImage image;
      InitEXRImage(&image);

      image.num_channels = 3;

      float* image_ptr[3];
      image_ptr[0] = &(pixels[2].at(0)); // B
      image_ptr[1] = &(pixels[1].at(0)); // G
      image_ptr[2] = &(pixels[0].at(0)); // R

      image.images = reinterpret_cast<unsigned char**>(image_ptr);
      image.width = width;
      image.height = height;

      header.num_channels = 3;
      std::array<EXRChannelInfo, 3> channelInfos;
      header.channels = &(channelInfos.at(0));
      // Must be BGR(A) order, since most of EXR viewers expect this channel order.
      strncpy(header.channels[0].name, "B", 1);
      header.channels[0].name[strlen("B")] = '\0';
      strncpy(header.channels[1].name, "G", 1);
      header.channels[1].name[strlen("G")] = '\0';
      strncpy(header.channels[2].name, "R", 1);
      header.channels[2].name[strlen("R")] = '\0';
      std::array<int, 3> pixel_types;
      std::array<int, 3> requested_pixel_types;
      header.pixel_types = &(pixel_types.at(0));
      header.requested_pixel_types = &(requested_pixel_types.at(0));
      for (int i = 0; i < header.num_channels; i++) {
        header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF;
      }
      const char* err;
      int ret = SaveEXRImageToFile(&image, &header, filename, &err);
      if (ret != TINYEXR_SUCCESS) {
        std::cerr << "Save EXR err: " << err << std::endl;
        return false;
      }
      printf("Saved exr file. [ %s ] \n", filename);
      return true;
    }

  private:
    int width;
    int height;
    std::array<std::vector<float>, 3> pixels; // Storing RGB values as doubles (0.0 - 1.0)
};
}
#endif //IMAGE_IO_H