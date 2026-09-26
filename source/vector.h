#pragma once
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <LinearMath/btQuaternion.h>
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
    Vector(btVector3 vec) : Vector(vec[0], vec[1], vec[2]) {}

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

    void normalize() {
        *this = this->normalized();
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

    static Vector min(const Vector& a, const Vector& b) {
        return Vector(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z)
        );
    }

    static Vector max(const Vector& a, const Vector& b) {
        return Vector(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z)
        );
    }
};

class Quaternion {
public:
    float x, y, z, w;

    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Quaternion(glm::quat quat) : x(quat.x), y(quat.y), z(quat.z), w(quat.w) {}
    Quaternion(btQuaternion quat) : x(quat[0]), y(quat[1]), z(quat[2]), w(quat[3]) {}

    Vector toEulerAngles() {
        return glm::degrees(glm::eulerAngles(gl()));
    }

    static Quaternion fromEulerAngles(Vector angles) {
        return glm::quat(glm::radians(angles.gl()));
    }

    const glm::quat gl() const {return glm::quat(w, x, y, z);}
    const btQuaternion bt() const {return btQuaternion(x, y, z, w);}
};