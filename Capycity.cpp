#include "includes.h"
#include "ConsoleIO.h" //helper class for Windows virtual console
#include "CapycitySim.h"

#define MAX_Y_INDEX 24 // max blueprint height
#define MAX_X_INDEX 24 // max blueprint height
#define BLUEPRINT_X_ORIGIN 1
#define BLUEPRINT_Y_ORIGIN 1

int height, width, inputInt = 0;
char inputChar = '\0';
std::string inputStr = "";

COORD consoleSize = {MAX_X_INDEX + 25, MAX_Y_INDEX + 5};

void refresh(ConsoleIO &cio, CapycitySim &ccs)
{
    cio.clearWindow();
    cio.printAtFormat<const char *const>("Blueprint:", {0, 0}, "1");
    cio.printAt(ccs.blueprint, {BLUEPRINT_Y_ORIGIN, BLUEPRINT_X_ORIGIN});

    cio.printAtFormat<const char *const>("Controls:", {MAX_X_INDEX + 5, 0}, "1;4");
}

void setBuilding(ConsoleIO &cio, CapycitySim &ccs)
{
    int startX, endX, startY, endY = 0;
    bool placeable;

    cio.printAtFormat<const char *const>("> Set:", {MAX_X_INDEX + 5, 1}, "4");
    cio.printAtFormat<const char *const>("1: Hydro Station", {MAX_X_INDEX + 5, 2}, "34");
    cio.printAtFormat<const char *const>("2: Solar Panel", {MAX_X_INDEX + 5, 3}, "33");
    cio.printAtFormat<const char *const>("3: Wind Turbine", {MAX_X_INDEX + 5, 4}, "");
    cio.printAtFormat<const char *const>("0: Cancel", {MAX_X_INDEX + 5, 5}, "91");

    do
    {
        placeable = true;
        cio.getInt("Building:", inputInt, 0, 3);
        cio.printAt(ccs.blueprint, {BLUEPRINT_Y_ORIGIN, BLUEPRINT_X_ORIGIN}); // refresh blueprint in case collision was shown
        cio.printInfoLine<const char *const>("");

        if (inputInt == 0)
            return;

        cio.getInt("Start X:", startX, 0, width);
        cio.getInt("Start Y:", startY, 0, height);
        cio.getInt("End X:", endX, startX, width);
        cio.getInt("End Y:", endY, startY, height);
        cio.getStr("Label:", inputStr);

        if (!ccs.setBuilding({(short)startX, (short)startY, (short)endX, (short)endY}, inputInt, inputStr))
        {
            cio.printInfoLine<const char *const>("Building collided!");
            cio.printRectFormat<char>('X', {(short)(startX + BLUEPRINT_X_ORIGIN), (short)(startY + BLUEPRINT_Y_ORIGIN), (short)(endX + BLUEPRINT_X_ORIGIN), (short)(endY + BLUEPRINT_Y_ORIGIN)},
                                      "101");
            placeable = false;
        }

    } while (!placeable);
}

void clearArea(ConsoleIO &cio, CapycitySim &ccs)
{
    int startX, endX, startY, endY = 0;

    cio.printAtFormat<const char *const>("> Clear:", {MAX_X_INDEX + 5, 1}, "4");

    cio.getInt("Start X:", startX, 0, width);
    cio.getInt("Start Y:", startY, 0, height);
    cio.getInt("End X:", endX, startX, width);
    cio.getInt("End Y:", endY, startY, height);

    ccs.clearArea({(short)startX, (short)startY, (short)endX, (short)endY});
}

void printBuildingInfo(ConsoleIO &cio, CapycitySim &ccs)
{
    cio.clearWindow();
    cio.printAtFormat<const char *const>("Building Info:", {0, 0}, "1;4");
    cio.printAtFormat<const char *const>("Type:", {0, 1}, "4");
    cio.printAtFormat<const char *const>("Label:", {6, 1}, "4");
    cio.printAtFormat<const char *const>("Materials:", {13, 1}, "4");
    cio.printAtFormat<const char *const>("Price:", {MAX_X_INDEX + 15, 1}, "4");

    int total = 0;

    for (int i = 0; i < ccs.buildings.size(); i++)
    {   
        std::cout << CSI << i+3 << ";" << 3 << "H";
        cio.printSGR(ccs.buildings.at(i).displayChar);
        cio.clearFormatting();
        cio.printAtFormat<std::string>(ccs.buildings.at(i).label, {6, (short)(i+2)}, "");
        cio.printAtFormat<std::string>(ccs.buildings.at(i).matsString(), {12, (short)(i+2)}, "");
        cio.printAtFormat<int>(ccs.buildings.at(i).totalCost(), {MAX_X_INDEX + 15, (short)(i+2)}, "");
        total += ccs.buildings.at(i).totalCost();
    }
    cio.printAtFormat<int>(total, {MAX_X_INDEX + 15, (short)(ccs.buildings.size()+2)}, "1;4");
    
    cio.getStr("Input any:", inputStr);
    return;
}

int main(void)
{
    ConsoleIO cio(consoleSize.X, consoleSize.Y); // CIO has to be constructed first so the Init can throw errors
    try
    {
        cio.Init();
    }
    catch (std::runtime_error &e)
    {
        printf("%s", e.what());
        exit(1);
    }

    cio.toggleAlternateBuffer();
    cio.printAtFormat<const char *const>("Capycity", {(short)(consoleSize.X / 2 - 4), 2}, "1;4;91;107");

    cio.getInt("Max Index X:", width, 0, MAX_X_INDEX, true);
    cio.getInt("Max Index Y:", height, 0, MAX_Y_INDEX, true);

    CapycitySim ccs(width, height);

    while (true)
    {
        refresh(cio, ccs);
        cio.printAtFormat<const char *const>("1: Set building", {MAX_X_INDEX + 5, 1}, "92");
        cio.printAtFormat<const char *const>("2: Clear area", {MAX_X_INDEX + 5, 2}, "91");
        cio.printAtFormat<const char *const>("3: Print Building Info", {MAX_X_INDEX + 5, 3}, "94");
        cio.printAtFormat<const char *const>("0: Exit", {MAX_X_INDEX + 5, 4}, "");

        cio.getInt("Operation:", inputInt, 0, 3);
        refresh(cio, ccs);

        switch (inputInt)
        {
        case 1:
            setBuilding(cio, ccs);
            break;
        case 2:
            clearArea(cio, ccs);
            break;
        case 3:
            printBuildingInfo(cio, ccs);
            break;
        case 0:
            exit(0);
        default:
            exit(1);
        }
    }

    return 0;
}
