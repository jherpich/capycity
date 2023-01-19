/**
 * ConsoleIO Hilfsklasse, nicht in Aufgabenstellung enthalten
 * @author Julius Herpich
 */
#include <windows.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#define ESC "\x1b"
#define CSI "\x1b["
#define OSC "\x1b]"

using namespace std;

struct SGR_char
{ // char using virtual terminal formating
  char c;
  char charset;
  char* format;

  bool operator==(SGR_char const & rhs) const {
      return ((this->c == rhs.c) && (this->charset == rhs.charset) && (this->format == rhs.format));
    }
  
  bool operator!=(SGR_char const & rhs) const {
      return ((this->c != rhs.c) || (this->charset != rhs.charset) || (this->format != rhs.format));
    }
};

typedef vector<SGR_char> SGRRow;
typedef vector<SGRRow> SGRMatrix;

class ConsoleIO
{
private:
  HANDLE hOut;
  DWORD dwMode;
  CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
  CONSOLE_FONT_INFOEX consoleFontInfoEx;

  SMALL_RECT windowSize;

  bool currentBufferState;

public:
  ConsoleIO(short width, short height)
  {
    hOut = 0;
    dwMode = 0;
    screenBufferInfo = {0};
    consoleFontInfoEx = {0};
    windowSize = {0, 0, (short)(width), (short)(height)};
    currentBufferState = false;
  }

  ~ConsoleIO() = default;

  void Init()
  {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
      throw runtime_error("Couldn't get console handle");

    if (!GetConsoleMode(hOut, &dwMode))
      throw runtime_error("Couldn't get console mode");

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
      throw runtime_error("Couldn't enable virtual terminal processing");

    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
    SetConsoleScreenBufferSize(hOut, {(short)(windowSize.Right + 1), (short)(windowSize.Bottom + 1)});

    GetCurrentConsoleFontEx(hOut, FALSE, &consoleFontInfoEx);
    consoleFontInfoEx.cbSize = sizeof(consoleFontInfoEx);
    consoleFontInfoEx.dwFontSize.X = 15;
    consoleFontInfoEx.dwFontSize.Y = 15;
    wcscpy_s(consoleFontInfoEx.FaceName, L"Terminal");
    SetCurrentConsoleFontEx(hOut, FALSE, &consoleFontInfoEx);

    currentBufferState = false;
  }

  bool toggleAlternateBuffer()
  {
    if (!currentBufferState)
    {
      printf(CSI "?1049h"); // enter Buffer
      printf(CSI "2J");     // clear screen
    }
    else
    {
      printf(CSI "?1049l"); // exit Buffer
    }
    currentBufferState = !currentBufferState;
    return currentBufferState;
  }

  void clearWindow()
  {
    printf(CSI "0m");                       // clear leftover formatting
    printf(CSI "2J");                       // Clear screen
    printf(CSI "%d;1H", windowSize.Bottom); // go to last row
    printf(ESC "(B");                       // set charset to ASCII 7-bit
  }

  void clearFormating()
  {
    printf(CSI "0m");                       // clear leftover formatting
    printf(CSI "%d;1H", windowSize.Bottom); // go to last row
    printf(ESC "(B");                       // set charset to ASCII 7-bit
  }

  void print(const SGRMatrix &matrix)
  {
    for (short row = 0; row < matrix.size(); ++row)
    { // Iterate Rows
      for (short col = 0; col < matrix.at(row).size(); ++col)
      { // Iterate Columns In Row
        printf(CSI "%i;%iH", row + 1, col + 1);
        printf(ESC "(%c", matrix.at(row).at(col).charset);
        printf(CSI "%sm", matrix.at(row).at(col).format);
        printf("%c", matrix.at(row).at(col).c);
      }
    }
    clearFormating();
  }

  void printAt(const SGRMatrix &matrix, const COORD &printOrigin)
  {
    for (short row = 0; row < matrix.size(); ++row)
    { // Iterate Rows
      for (short col = 0; col < matrix.at(row).size(); ++col)
      { // Iterate Columns In Row
        printf(CSI "%i;%iH", (row + printOrigin.Y) + 1, (col + printOrigin.X) + 1);
        printf(ESC "(%c", matrix.at(row).at(col).charset);
        printf(CSI "%sm", matrix.at(row).at(col).format);
        printf("%c", matrix.at(row).at(col).c);
      }
    }
    clearFormating();
  }

  void printAtFormatString(const char *string, const COORD &printOrigin, const char *format = "")
  {
    printf(CSI "%d;%dH", printOrigin.Y + 1, printOrigin.X + 1);
    printf(CSI "%sm", format);
    printf("%s", string);
    clearFormating();
  }

  void printInfoLine(const char *const message)
  {
    printf(CSI "34m");                            // blue
    printf(CSI "%d;1H", (windowSize.Bottom) - 1); // move cursor to row windowSize.Bottom, pos 1 (= start of second to last row)
    printf(CSI "K");                              // clear the line
    printf("%s", message);
    clearFormating();
  }

  int getInt(const char *const message, int &inputInt, int min = INT_MIN, int max = INT_MAX, bool hint = false)
  {
    printf(CSI "4m"); // underline
    if (hint)
    {
      printf(CSI "%d;1H", (windowSize.Bottom) - 1);
      printf(CSI "K");
      printf("(Min: %i | Max: %i)", min, max);
    }
    do
    {

      printf(CSI "%d;1H", windowSize.Bottom); // move cursor to row windowSize.Bottom, pos 1 (= start of second to last row)
      printf(CSI "K");                              // clear the line
      printf("%s ", message);
      scanf("%i", &inputInt);
    } while (inputInt < min || inputInt > max);
    if (hint)
    {
      printf(CSI "%d;1H", (windowSize.Bottom) - 1); // clear hints
      printf(CSI "0m");                             // underline
      printf(CSI "K");
    }
    clearFormating();
    return inputInt;
  }

  char getChar(const char *const message, char &inputChar, const char *allowed)
  {
    printf(CSI "4m"); // underline
    do
    {
      printf(CSI "%d;1H", windowSize.Bottom); // move cursor to row windowSize.Bottom, pos 1 (= start of second to last row)
      printf(CSI "K");                              // clear the line
      printf("%s ", message);
      scanf("%s", &inputChar);
    } while (string(allowed).find(inputChar) == string::npos);
    clearFormating();
    return inputChar;
  }

  char *getString(const char *const message, char *inputString, const string &allowed)
  {
    printf(CSI "4m"); // underline
    do
    {
      printf(CSI "%d;1H", windowSize.Bottom); // move cursor to row windowSize.Bottom, pos 1 (= start of second to last row)
      printf(CSI "K");                              // clear the line
      printf("%s ", message);
      scanf("%s", inputString);
    } while (allowed.find(inputString) == string::npos);
    clearFormating();
    return inputString;
  }
};
