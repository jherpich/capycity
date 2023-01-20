#ifndef CAPYCITY_SIM_H
#define CAPYCITY_SIM_H

#include "includes.h"
#include "Building.h"

enum MapTile
{
    EMPTY,
    HYDRO,
    SOLAR,
    WIND
};

class CapycitySim
{
protected:
    int width, height;
    Empty empty;

public:
    std::vector<Building> buildings;
    Blueprint blueprint;

    CapycitySim(int w, int h) : width(w), height(h)
    {
        for (size_t row = 0; row <= height; row++)
        {
            BlueprintRow bRow(width + 1);
            for (size_t col = 0; col <= width; col++)
                bRow[col] = &empty;
            blueprint.push_back(bRow); // push each row after filling it
        }
    };

    ~CapycitySim() = default;

    bool setBuilding(const SMALL_RECT &area, int &type, std::string &label)
    {
        for (size_t row = area.Top; row <= area.Bottom; row++)
            for (size_t col = area.Left; col <= area.Right; col++)
                if (!(blueprint.at(row).at(col)->canOverwrite))
                    return false;

        int size = (area.Right - area.Left + 1) * (area.Bottom - area.Top + 1);

        switch (type)
        {
        case MapTile::HYDRO:
        {
            Hydro h(size, label);
            buildings.push_back(h);
            break;
        }
        case MapTile::SOLAR:
        {
            Solar s(size, label);
            buildings.push_back(s);
            break;
        }
        case MapTile::WIND:
        {
            Wind w(size, label);
            buildings.push_back(w);
            break;
        }
        default:
            return false;
        }

        for (size_t row = area.Top; row <= area.Bottom; row++)
            for (size_t col = area.Left; col <= area.Right; col++)
                blueprint.at(row).at(col) = &(buildings.back());

        return true;
    }

    void clearArea(const SMALL_RECT &area)
    {
        for (size_t row = area.Top; row <= area.Bottom; row++)
            for (size_t col = area.Left; col <= area.Right; col++)
            {
                blueprint.at(row).at(col)->clear();
                blueprint.at(row).at(col) = &empty;
                
            }
    }
};

#endif // CAPYCITY_SIM_H