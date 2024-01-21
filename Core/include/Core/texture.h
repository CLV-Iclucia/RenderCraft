#ifndef RENDERCRAFT_TEXTURE_H
#define RENDERCRAFT_TEXTURE_H
#include <Core/core.h>
#include <Core/debug.h>
#include <Core/spectrums.h>
#include <Core/utils.h>
#include <Spatify/grids.h>
#include <fstream>
#include <memory>

namespace rdcraft {
using spatify::Grid;
class TextureMapping {
  public:
    virtual Vec2 map(Real x, Real y) const = 0;
    virtual ~TextureMapping() = default;
};

/**
 * @tparam T the type of the texture value.
 */
template <typename T, int Dim>
class Texture {
  public:
    virtual T eval(const Vector<Real, Dim>&) const = 0;
    virtual ~Texture() = default;
};

template <typename T, int Dim>
class ConstantTexture final : public Texture<T, Dim> {
  public:
    explicit ConstantTexture(const T& _tex)
      : tex(_tex) {
    }
    explicit ConstantTexture(T&& _tex)
      : tex(std::move(_tex)) {
    }
    T eval(const Vec2&) const override { return tex; }

  private:
    T tex;
};

template <typename T, int Dim>
class PerlinNoise final : public Texture<T, Dim>, NonCopyable {
  public:
    PerlinNoise(const Vector<T, Dim>& resolution,
                const Vector<T, Dim>& spacing) {
      grid = std::make_unique<spatify::Grid>(resolution, spacing);
    }
    T eval(const Vector<T, Dim>& tex_coord) const override {
    }

  private:
    std::unique_ptr<Grid<Real, Real, Dim>> grid = nullptr;
    std::unique_ptr<Texture<T, Dim>> texA{}, texB{};
};

template <typename T, int Dim>
class CheckerBoard : public Texture<T, Dim> {
  private:
    Real grid_w = 0.0, grid_h = 0.0;
    Texture<T, Dim>* texA, texB;

  public:
    CheckerBoard(const Texture<T, Dim>& A_, const Texture<T, Dim>& B_)
      : texA(A_), texB(B_) {
    }
    T eval(const Vec2& uv) const override {
      int _u = floor(uv[0] / grid_w);
      int _v = floor(uv[1] / grid_h);
      Real u = uv[0] - _u * grid_w;
      Real v = uv[1] - _v * grid_h;
      return ((_u ^ _v) & 1) ? texA->eval({u, v}) : texB->eval({u, v});
    }
};

template <typename T>
using SurfaceTexture = Texture<T, 2>;
template <typename T>
using VolumeTexture = Texture<T, 3>;

template <typename T>
class GridVolumeTexture final : public VolumeTexture<T> {
  public:
    explicit GridVolumeTexture(const char* path) {
      std::ifstream fin(path);
      if (!fin.is_open())
        ERROR("Cannot open file");
      Vec3i resolution{};
      fin >> resolution.x >> resolution.y >> resolution.z;
      grid = spatify::Array3D<T>(resolution);
      if constexpr (std::is_same_v<T, Spectrum>) {
        Spectrum maxv(-infinity<Real>());
        for (int i = 0; i < grid.width(); i++)
          for (int j = 0; j < grid.height(); j++)
            for (int k = 0; k < grid.depth(); k++) {
              Spectrum v;
              fin >> v.r >> v.g >> v.b;
              majorant.r = std::max(majorant.r, v.r);
              majorant.g = std::max(majorant.g, v.g);
              majorant.b = std::max(majorant.b, v.b);
              grid(i, j, k) = v;
            }
      }
      fin.close();
    }
    GridVolumeTexture(const Vec3i& resolution, const T& constant)
      : grid(resolution), majorant(constant) {
      grid.fill(constant);
    }
    T eval(const Vector<Real, 3>& pos) const override {
      assert(pos.x >= 0 && pos.x <= 1 && pos.y >= 0 && pos.y <= 1 &&
          pos.z >= 0 && pos.z <= 1);
      int resx = grid.width(), resy = grid.height(), resz = grid.depth();
      auto idx = Vec3i(pos.x * resx, pos.y * resy, pos.z * resz);
      idx.x = clamp(idx.x, 0, resx - 1);
      idx.y = clamp(idx.y, 0, resy - 1);
      idx.z = clamp(idx.z, 0, resz - 1);
      int nx = clamp(idx.x + 1, 0, resx - 1);
      int ny = clamp(idx.y + 1, 0, resy - 1);
      int nz = clamp(idx.z + 1, 0, resz - 1);
      T v000 = grid(idx.x, idx.y, idx.z);
      T v001 = grid(idx.x, idx.y, nz);
      T v010 = grid(idx.x, ny, idx.z);
      T v011 = grid(idx.x, ny, nz);
      T v100 = grid(nx, idx.y, idx.z);
      T v101 = grid(nx, idx.y, nz);
      T v110 = grid(nx, ny, idx.z);
      T v111 = grid(nx, ny, nz);
      Real tx = pos.x * resx - idx.x;
      Real ty = pos.y * resy - idx.y;
      Real tz = pos.z * resz - idx.z;
      assert(
          tx >= 0.0 && tx <= 1.0 && ty >= 0.0 && ty <= 1.0 && tz >= 0.0 && tz <=
          1.0);
      return tx * ty * tz * v111 + tx * ty * (1 - tz) * v110 +
             tx * (1 - ty) * tz * v101 + tx * (1 - ty) * (1 - tz) * v100 +
             (1 - tx) * ty * tz * v011 + (1 - tx) * ty * (1 - tz) * v010 +
             (1 - tx) * (1 - ty) * tz * v001 + (1 - tx) * (1 - ty) * (1 - tz) *
             v000;
    }
    T getMajorant() const {
      return majorant;
    }

  private:
    spatify::Array3D<T> grid;
    T majorant;
};
} // namespace rdcraft
#endif