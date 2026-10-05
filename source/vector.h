#pragma once
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <sstream>

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

    bool operator==(const Vector& other) const {
        return x == other.x && y == other.y && z == other.z;
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
        result.x = sinYaw * cosPitch;
        result.z = -cosYaw * cosPitch;
        result.y = sinPitch;
        return result.normalized();
    }

    Vector angles() const {
        Vector dir = normalized();
        Vector angles;
    
        angles.x = glm::degrees(asin(dir.y));
        angles.y = glm::degrees(atan2(-dir.z, dir.x));
        angles.z = 0.0f; 

        return angles;
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

    static Vector fromOrigin(std::string value) {
        std::istringstream stream(value);
        float x, y, z;
        stream >> x >> y >> z;
        return Vector(x, z, -y) / 32.0f;
    }

    static Vector fromAngles(std::string value) {
        std::istringstream stream(value);
        float x, y, z;
        stream >> x >> y >> z;
        return Vector(x, y, z);
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

    static Quaternion Slerp(float a, Quaternion quat1, Quaternion quat2) {
        quat1 = glm::normalize(quat1.gl());
        quat2 = glm::normalize(quat2.gl());
        float dot = glm::dot(quat1.gl(), quat2.gl());

        if (dot < 0.0f) {
            quat2.w = -quat2.w;
            quat2.x = -quat2.x;
            quat2.y = -quat2.y;
            quat2.z = -quat2.z;
            dot = -dot;
        }

        const float EPSILON = 0.0001f;
        if (dot > 1.0f - EPSILON) {
            return glm::normalize(quat1.gl() + (quat2.gl() - quat1.gl()) * a);
        }

        float angle = acosf(dot);
        float sinAngle = sinf(angle);
        float invSinAngle = 1.0f / sinAngle;
        float c1 = sinf((1.0f - a) * angle) * invSinAngle;
        float c2 = sinf(a * angle) * invSinAngle;

        return quat1.gl() * c1 + quat2.gl() * c2;
    }

    static Quaternion fromEulerAngles(Vector angles) {
        return glm::quat(glm::radians(angles.gl() * glm::vec3(1, -1, 1)));
    }

    const glm::quat gl() const {return glm::quat(w, x, y, z);}
    const btQuaternion bt() const {return btQuaternion(x, y, z, w);}
};