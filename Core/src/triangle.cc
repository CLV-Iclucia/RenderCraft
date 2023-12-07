//
// Created by creeper on 22-11-20.
//
#include <Core/maths.h>
#include <Core/record.h>
#include <Core/mesh.h>

namespace rdcraft {
static bool insideTriangle(const Vec3& p, const Vec3& a, const Vec3& b,
                           const Vec3& c) {
  Vec3 ab = b - a;
  Vec3 bc = c - b;
  Vec3 ca = a - c;
  Vec3 ap = p - a;
  Vec3 bp = p - b;
  Vec3 cp = p - c;
  Vec3 n1 = cross(ab, ap);
  Vec3 n2 = cross(bc, bp);
  Vec3 n3 = cross(ca, cp);
  return dot(n1, n2) >= 0 && dot(n2, n3) >= 0 && dot(n3, n1) >= 0;
}

bool Triangle::intersect(const Ray& ray, SurfaceRecord* pRec) const {
  Vec3 ab = mesh->vertices[idx[1]] - mesh->vertices[idx[0]];
  Vec3 ac = mesh->vertices[idx[2]] - mesh->vertices[idx[0]];
  Vec3 p_vec = cross(ray.dir, ac);
  Real det = dot(ab, p_vec);
  if (det < epsilon<Real>()) return false;
  Real inv_det = 1.0 / det;
  Vec3 t_vec = ray.orig - mesh->vertices[idx[0]];
  Real u = dot(t_vec, p_vec) * inv_det;
  if (u < 0.0 || u > 1.0) return false;
  Vec3 q_vec = cross(t_vec, ab);
  Real v = dot(ray.dir, q_vec) * inv_det;
  pRec->pos = u * mesh->vertices[idx[0]] + v * mesh->vertices[idx[1]] + (
                1.0 - u - v) * mesh->vertices[idx[2]];
  pRec->normal = u * mesh->normals[idx[0]] + v * mesh->normals[idx[1]] + (
                   1.0 - u - v) * mesh->normals[idx[2]];
  pRec->uv = u * mesh->uvs[idx[0]] + v * mesh->uvs[idx[1]] + (1.0 - u - v) *
             mesh->uvs[idx[2]];
  return true;
}

bool Triangle::intersect(const Ray& ray) const {
  Vec3 ab = mesh->vertices[idx[1]] - mesh->vertices[idx[0]];
  Vec3 ac = mesh->vertices[idx[2]] - mesh->vertices[idx[0]];
  Vec3 p_vec = cross(ray.dir, ac);
  Real det = dot(ab, p_vec);
  if (det < epsilon<Real>()) return false;
  Real inv_det = 1.0 / det;
  Vec3 t_vec = ray.orig - mesh->vertices[idx[0]];
  Real u = dot(t_vec, p_vec) * inv_det;
  if (u < 0.0 || u > 1.0) return false;
  Vec3 q_vec = cross(t_vec, ab);
  Real v = dot(ray.dir, q_vec) * inv_det;
  if (v < 0.0 || u + v > 1.0) return false;
  return true;
}

AABB Triangle::getAABB() const {
  Vec3 lo = Vec3(
      std::min(std::min(mesh->vertices[idx[0]].x, mesh->vertices[idx[1]].x),
               mesh->vertices[idx[2]].x),
      std::min(std::min(mesh->vertices[idx[0]].y, mesh->vertices[idx[1]].y),
               mesh->vertices[idx[2]].y),
      std::min(std::min(mesh->vertices[idx[0]].z, mesh->vertices[idx[1]].z),
               mesh->vertices[idx[2]].z));
  Vec3 hi = Vec3(
      std::max(std::max(mesh->vertices[idx[0]].x, mesh->vertices[idx[1]].x),
               mesh->vertices[idx[2]].x),
      std::max(std::max(mesh->vertices[idx[0]].y, mesh->vertices[idx[1]].y),
               mesh->vertices[idx[2]].y),
      std::max(std::max(mesh->vertices[idx[0]].z, mesh->vertices[idx[1]].z),
               mesh->vertices[idx[2]].z));
  return AABB(lo, hi);
}

Real Triangle::pdfSample(const Vec3& p) const {
}
} // namespace rdcraft