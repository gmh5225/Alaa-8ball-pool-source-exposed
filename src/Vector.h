#pragma once

#include "const.h"

#include <cfloat>
#include <Windows.h>

struct Vector2D
{
    inline Vector2D() { x = y = vecZero; }
    inline Vector2D(vec_t dstX, vec_t dstY) { x = dstX, y = dstY; }

    bool     isNull() const;
    vec_t    length() const;
    Vector2D toScreen() const;
    void     print(const char* prefix = "", const char* suffix = "\n") const;
    vec_t    getDistanceTo(const Vector2D& destination) const;

    inline void nullify() { x = y = vecZero; }

    inline vec_t& operator[](int i) 
    {
        return PVECTOR(this)[i];
    }

    inline vec_t& operator[](int i) const 
    { 
        return PVECTOR(this)[i];
    }

    inline Vector2D operator-(const Vector2D& other) 
    {
        return Vector2D(this->x - other.x, this->y - other.x); 
    }

    inline bool operator!=(const Vector2D& other) 
    { 
        return ((this->x != other.x) && (this->y != other.x));
    }

    inline void operator=(const Vector2D& other)
    {
        this->x = other.x;
        this->y = other.y;
    }

    vec_t x, y;
};

struct Vector3D
{
    inline Vector3D() { x = y = z = vecZero; }

    bool  isNull() const;
    vec_t length() const;
    void  print(const char* prefix = "", const char* suffix = "\n") const;

    inline void nullify() { x = y = z = vecZero; }

    inline vec_t& operator[](int i) { return PVECTOR(this)[i]; }
    inline vec_t& operator[](int i) const { return PVECTOR(this)[i]; }

    vec_t x, y, z;
};

struct Vector4D
{
    inline Vector4D() { x = y = z = w = vecZero; };
    inline Vector4D(vec_t srcX, vec_t srcY, vec_t srcZ, vec_t srcW) { x = srcX, y = srcY, z = srcZ, w = srcW; }

    vec_t x, y, z, w;
};