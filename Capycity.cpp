#include "ConsoleIO.h" //helper class for Windows virtual console

#define MAX_Y_INDEX 24 //max blueprint height
#define MAX_X_INDEX 24 //max blueprint height
#define MATRIX_X_ORIGIN 1
#define MATRIX_Y_ORIGIN 1

SGR_char const buildingLookup[] = {
    {'a', '0', (char *)"30;42"} // green DEC char Ox61
    ,
    {'H', 'B', (char *)"37;44"} // blue ASCII H
    ,
    {'S', 'B', (char *)"30;43"} // yellow ASCII S
    ,
    {'W', 'B', (char *)"30;47"} // white ASCII W
};

enum MapTile
{
    EMPTY,
    HYDRO,
    SOLAR,
    WIND
};

int height, width, inputInt = 0;
char inputChar = '\0';
char inputStr[20] = "";

SGRMatrix matrix;
COORD consoleSize = {MAX_X_INDEX + 25, MAX_Y_INDEX + 5};

ConsoleIO cio(consoleSize.X, consoleSize.Y);

void refresh()
{
    cio.clearWindow();
    cio.printAtFormat<char>("Blueprint:", {0, 0}, "1");
    cio.printAt(matrix, {MATRIX_Y_ORIGIN, MATRIX_X_ORIGIN});

    cio.printAtFormat<char>("Controls:", {MAX_X_INDEX + 5, 0}, "1;4");
}

void setBuilding()
{
    int start_x, end_x, start_y, end_y = 0;
    bool placeable;

    SGR_char c;

    cio.printAtFormat<char>("> Set:", {MAX_X_INDEX + 5, 1}, "4");
    cio.printAtFormat<char>("1: Hydro Station", {MAX_X_INDEX + 5, 2}, "34");
    cio.printAtFormat<char>("2: Solar Panel", {MAX_X_INDEX + 5, 3}, "33");
    cio.printAtFormat<char>("3: Wind Turbine", {MAX_X_INDEX + 5, 4}, "");
    cio.printAtFormat<char>("0: Cancel", {MAX_X_INDEX + 5, 5}, "91");

    do
    {
        placeable = true;
        cio.getInt("Building:", inputInt, 0, 3);
        cio.printAt(matrix, {MATRIX_Y_ORIGIN, MATRIX_X_ORIGIN}); // refresh matrix in case collision was shown

        switch (inputInt)
        {
        case MapTile::HYDRO:
            c = buildingLookup[1];
            break;
        case MapTile::SOLAR:
            c = buildingLookup[2];
            break;
        case MapTile::WIND:
            c = buildingLookup[3];
            break;
        default:
            return;
        }

        cio.getInt("Start X:", start_x, 0, width);
        cio.getInt("Start Y:", start_y, 0, height);
        cio.getInt("End X:", end_x, start_x, width);
        cio.getInt("End Y:", end_y, start_y, height);

        for (size_t row = start_y; row <= end_y; row++)
            for (size_t col = start_x; col <= end_x; col++)
                if (matrix[row][col] != buildingLookup[MapTile::EMPTY])
                {
                    cio.printInfoLine("Building collided!");
                    cio.printAtFormat<char>("X", {(short)(row + MATRIX_Y_ORIGIN), (short)(col + MATRIX_X_ORIGIN)}, "101");
                    placeable = false;
                }
    } while (!placeable);

    for (size_t row = start_y; row <= end_y; row++)
        for (size_t col = start_x; col <= end_x; col++)
            matrix[row][col] = c;
}

void clearBuilding()
{
    int start_x, end_x, start_y, end_y = 0;

    cio.printAtFormat<char>("> Clear:", {MAX_X_INDEX + 5, 1}, "4");

    cio.getInt("Start X:", start_x, 0, width);
    cio.getInt("Start Y:", start_y, 0, height);
    cio.getInt("End X:", end_x, start_x, width);
    cio.getInt("End Y:", end_y, start_y, height);

    for (size_t row = start_y; row <= end_y; row++)
        for (size_t col = start_x; col <= end_x; col++)
            matrix[row][col] = buildingLookup[MapTile::EMPTY];
}

int main(void)
{

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
    cio.printAtFormat<char>("Capycity", {(short)(consoleSize.X / 2 - 4), 2}, "1;4;91;107");

    cio.getInt("Max Index X:", width, 0, MAX_X_INDEX, true);
    cio.getInt("Max Index Y:", height, 0, MAX_Y_INDEX, true);

    for (size_t row = 0; row <= height; row++)
    {
        SGRRow sgRow(width + 1);

        for (size_t col = 0; col <= width; col++)
        {
            SGR_char c = buildingLookup[MapTile::EMPTY];
            sgRow[col] = c;
        }

        matrix.push_back(sgRow); // push each row after filling it
    }

    while (true)
    {
        refresh();
        cio.printAtFormat<char>("1: Set building", {MAX_X_INDEX + 5, 1}, "92");
        cio.printAtFormat<char>("2: Clear area", {MAX_X_INDEX + 5, 2}, "91");
        cio.printAtFormat<char>("0: Exit", {MAX_X_INDEX + 5, 3}, "");

        cio.getInt("Operation:", inputInt, 0, 2);
        refresh();

        switch (inputInt)
        {
        case 1:
            setBuilding();
            break;
        case 2:
            clearBuilding();
            break;
        case 0:
            exit(0);
        default:
            exit(1);
        }
    }

    return 0;
}
