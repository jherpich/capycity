#ifndef BUILDING_H
#define BUILDING_H

#include "includes.h"
#include "Material.h"

struct SGR_char
{ // char for display virtual terminal formating
    char c;
    char charset;
    char *format;

    bool operator==(SGR_char const &rhs) const
    {
        return ((this->c == rhs.c) && (this->charset == rhs.charset) && (this->format == rhs.format));
    }

    bool operator!=(SGR_char const &rhs) const
    {
        return ((this->c != rhs.c) || (this->charset != rhs.charset) || (this->format != rhs.format));
    }
};

Wood wood;
Metal metal;
Plastic plastic;

class Building
{
protected:
    std::vector<Material *> materials;

public:
    int size;
    int baseCost;
    std::string label;
    SGR_char displayChar;
    bool canOverwrite;

    Building() = default;
    Building(int s, int bC, std::string l, const SGR_char &dC, bool b) : size(s), baseCost(bC), label(l), displayChar(dC), canOverwrite(b){};
    Building(const Building &) = default;
    ~Building() = default;

    void clear()
    {
        if (--size == 0)
            delete this;
    }

    std::string matsString()
    {
        std::string mats = "";
        for (short i = 0; i < materials.size(); i++)
        {
            mats.append(" ");
            mats.append(materials.at(i)->name);
        }
        return mats;
    }

    int totalCost()
    {
        int cost = 0;
        for (short i = 0; i < materials.size(); i++)
        {
            cost += (materials.at(i)->cost);
        }
        return cost;
    }
};

class Empty : public Building
{
public:
    Empty() : Building(0, 0, "\"Empty\"", {'a', '0', (char *)"30;42"}, true) // green DEC char Ox61
    {
        materials = {};
    };
    ~Empty() = default;

    void clear(){};
};

class Hydro : public Building
{
public:
    Hydro(int size, std::string label) : Building(size, 100, label, {'H', 'B', (char *)"37;44"}, false) // blue ASCII H
    {
        materials = {&wood, &metal};
    };
    ~Hydro() = default;
};

class Solar : public Building
{
public:
    Solar(int size, std::string label) : Building(size, 250, label, {'S', 'B', (char *)"30;43"}, false) // yellow ASCII S
    {
        materials = {&metal, &metal, &plastic};
    };
    ~Solar() = default;
};

class Wind : public Building
{
public:
    Wind(int size, std::string label) : Building(size, 500, label, {'W', 'B', (char *)"30;47"}, false) // white ASCII W
    {
        materials = {&wood, &metal, &plastic};
    };
    ~Wind() = default;
};

typedef std::vector<Building *> BlueprintRow;
typedef std::vector<BlueprintRow> Blueprint;

#endif // BUILDING_H