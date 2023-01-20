#ifndef MATERIAL_H
#define MATERIAL_H

#include "includes.h"

class Material
{
protected:

public:
    int cost;
    std::string name;

    Material(int p, std::string n) : cost(p), name(n){};
    ~Material() = default;
};

class Wood : public Material
{
public:
    Wood() : Material(50, "\"Wood\""){};
    ~Wood() = default;
};

class Plastic : public Material
{
public:
    Plastic() : Material(100, "\"Plastic\""){};
    ~Plastic() = default;
};

class Metal : public Material
{
public:
    Metal() : Material(150, "\"Metal\""){};
    ~Metal() = default;
};

#endif // MATERIAL_H