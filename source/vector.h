#pragma once
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <LinearMath/btVector3.h>
#define _USE_MATH_DEFINES
#include <cmath>

class Vector {
public:
    float x, y, z;

    Vector() : x(0), y(0), z(0) {}
    Vector(float a) : x(a), y(a), z(a) {}
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector(glm::vec3 vec) : Vector(vec.x, vec.y, vec.z) {}

    float& operator[](int index) {
        return (&x)[index];
    }

    const Vector operator-() const {
        return Vector(-x, -y, -z);
    }

    const Vector operator+(const Vector& other) const {
        return Vector(x + other.x, y + other.y, z + other.z);
    }

    const Vector operator-(const Vector& other) const {
        return Vector(x - other.x, y - other.y, z - other.z);
    }

    const Vector operator*(const Vector& other) const {
        return Vector(x * other.x, y * other.y, z * other.z);
    }

    const Vector operator*(float scalar) const {
        return Vector(x * scalar, y * scalar, z * scalar);
    }

    const Vector operator/(const Vector& other) const {
        return Vector(x / other.x, y / other.y, z / other.z);
    }

    const Vector operator/(float scalar) const {
        return Vector(x / scalar, y / scalar, z / scalar);
    }

    const Vector& operator+=(const Vector& other) {
        *this = *this + other;
        return *this;
    }

    const Vector& operator-=(const Vector& other) {
        *this = *this - other;
        return *this;
    }

    const Vector& operator*=(const Vector& other) {
        *this = *this * other;
        return *this;
    }

    const Vector& operator*=(float scalar) {
        *this = *this * scalar;
        return *this;
    }

    const Vector& operator/=(const Vector& other) {
        *this = *this / other;
        return *this;
    }

    const Vector& operator/=(float scalar) {
        *this = *this / scalar;
        return *this;
    }

    float length2() const {
        return x * x + y * y + z * z;
    }

    float length() const {
        return sqrt(length2());
    }

    float dot(const Vector& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector cross(const Vector& other) const {
        return Vector(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    Vector normalized() const {
        float len = length();
        if (len == 0) return *this;
        return *this / len;
    }

    // Convert the vector representing Euler angles (degrees) to a unit vector representing a direction
    Vector direction() const {
        float sinYaw = sin(glm::radians(y)), cosYaw = cos(glm::radians(y));
        float sinPitch = sin(glm::radians(x)), cosPitch = cos(glm::radians(x));
        Vector result;
        result.x = cosYaw * cosPitch;
        result.z = -sinYaw * cosPitch;
        result.y = sinPitch;
        return result.normalized();
    }

    // Convert the vector to an OpenGL/glm vector
    const glm::vec3 gl() const {return glm::vec3(x, y, z);}
    // Convert the vector to a Bullet vector
    const btVector3 bt() const {return btVector3(x, y, z);}
};