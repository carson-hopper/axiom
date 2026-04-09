#pragma once

#include <cmath>
#include <cstdint>

namespace Axiom {

	struct Vector2 {
		float x = 0.0f;
		float y = 0.0f;

		Vector2() = default;
		Vector2(const float x, const float y) : x(x), y(y) {}
		Vector2(const int32_t x, const int32_t y) : x(x), y(y) {}
	};

	struct Vector3 {
		double x = 0.0;
		double y = 0.0;
		double z = 0.0;

		Vector3() = default;
		Vector3(const double x, const double y, const double z) : x(x), y(y), z(z) {}

		Vector3 operator+(const Vector3& other) const { return {x + other.x, y + other.y, z + other.z}; }
		Vector3 operator-(const Vector3& other) const { return {x - other.x, y - other.y, z - other.z}; }
		Vector3 operator*(const double scalar) const { return {x * scalar, y * scalar, z * scalar}; }

		double Length() const { return std::sqrt(x * x + y * y + z * z); }
		double LengthSquared() const { return x * x + y * y + z * z; }

		double DistanceTo(const Vector3& other) const { return (*this - other).Length(); }
		double DistanceSquaredTo(const Vector3& other) const { return (*this - other).LengthSquared(); }

		int32_t ChunkX() const { return static_cast<int32_t>(std::floor(x)) >> 4; }
		int32_t ChunkZ() const { return static_cast<int32_t>(std::floor(z)) >> 4; }
	};

}
