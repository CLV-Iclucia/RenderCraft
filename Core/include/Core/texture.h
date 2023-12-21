#ifndef RENDERCRAFT_TEXTURE_H
#define RENDERCRAFT_TEXTURE_H
#include <Core/core.h>
#include <Spatify/grids.h>
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
class ConstantTexture : public Texture<T, Dim> {
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
class PerlinNoise : public Texture<T, Dim>, NonCopyable {
  private:
    std::unique_ptr<Grid<Real, Real, Dim>> grid = nullptr;
    std::unique_ptr<Texture<T, Dim>> texA{}, texB{};

  public:
    PerlinNoise(const Vector<T, Dim>& resolution,
                const Vector<T, Dim>& spacing) {
      grid = std::make_unique<spatify::Grid>(resolution, spacing);

    }
    T eval(const Vector<T, Dim>& tex_coord) const override {
    }
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
class SampledVolumeTexture : VolumeTexture<T> {
};
} // namespace rdcraft
#endif