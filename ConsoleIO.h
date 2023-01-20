/**
 * ConsoleIO Hilfsklasse, nicht in Aufgabenstellung enthalten
 * @author Julius Herpich
 */

#ifndef CONSOLEIO_H
#define CONSOLEIO_H

#include <windows.h>

// #include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#define ESC "\x1b"
#define CSI "\x1b["
#define OSC "\x1b]"

struct SGR_char
{ // char using virtual terminal formating
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

typedef std::vector<SGR_char> SGRRow;
typedef std::vector<SGRRow> SGRMatrix;

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
      throw std::runtime_error("Couldn't get console handle");

    if (!GetConsoleMode(hOut, &dwMode))
      throw std::runtime_error("Couldn't get console mode");

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
      throw std::runtime_error("Couldn't enable virtual terminal processing");

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
      std::cout << CSI "?1049h"; // enter Buffer
      std::cout << CSI "2J";     // clear screen
    }
    else
    {
      std::cout << CSI "?1049l"; // exit Buffer
    }
    std::cout.flush();
    currentBufferState = !currentBufferState;
    return currentBufferState;
  }

  void clearFormating()
  {
    std::cout << CSI "0m";                          // clear leftover formatting
    std::cout << CSI << windowSize.Bottom << ";1H"; // go to last row
    std::cout << ESC "(B";                          // set charset to ASCII 7-bit
    std::cout.flush();
  }

  void clearWindow()
  {
    std::cout << CSI "2J"; // Clear screen
    clearFormating();
  }

  void printAt(const SGRMatrix &matrix, const COORD &printOrigin)
  {
    for (short row = 0; row < matrix.size(); ++row)
    { // Iterate Rows
      for (short col = 0; col < matrix.at(row).size(); ++col)
      { // Iterate Columns In Row
        std::cout << CSI << (row + printOrigin.Y + 1) << ";" << (col + printOrigin.X + 1) << "H";
        std::cout << ESC "(" << matrix.at(row).at(col).charset;
        std::cout << CSI << matrix.at(row).at(col).format << "m";
        std::cout << matrix.at(row).at(col).c;
      }
    }
    std::cout.flush();
    clearFormating();
  }

  template <typename T>
  void printAtFormat(const T *const print, const COORD &printOrigin, const char *format = "")
  {
    std::cout << CSI << (printOrigin.Y + 1) << ";" << (printOrigin.X + 1) << "H";
    std::cout << CSI << format << "m";
    std::cout << print;
    std::cout.flush();
    clearFormating();
  }

  template <typename T>
  void printInfoLine(const T *const message)
  {
    std::cout << CSI "34m";                               // blue
    std::cout << CSI << (windowSize.Bottom - 1) << ";1H"; // move cursor to row windowSize.Bottom -1, pos 1 (= start of second to last row)
    std::cout << CSI "K";                                 // clear the line
    std::cout << message;
    std::cout.flush();
    clearFormating();
  }

  int getInt(const char *const message, int &inputInt, int min = INT_MIN, int max = INT_MAX, bool hint = false)
  {
    std::cout << CSI "4m"; // underline
    if (hint)
    {
      std::cout << CSI << (windowSize.Bottom - 1) << ";1H"; // move cursor
      std::cout << CSI "K";
      std::cout << "(Min: " << min << " | Max: " << max << ")";
      std::cout.flush();
    }
    do
    {
      std::cout << CSI << (windowSize.Bottom) << ";1H"; // move cursor
      std::cout << CSI "K";                             // clear the line
      std::cout << message;
      std::cout.flush();
      std::cin >> inputInt;
    } while (inputInt < min || inputInt > max);
    if (hint)
    {
      std::cout << CSI << (windowSize.Bottom - 1) << ";1H"; // move cursor
      std::cout << CSI "0m";                                // underline
      std::cout << CSI "K";
      std::cout.flush();
    }
    clearFormating();
    return inputInt;
  }

  char getChar(const char *const message, char &inputChar, const char *allowed)
  {
    std::cout << CSI "4m"; // underline
    do
    {
      std::cout << CSI << (windowSize.Bottom) << ";1H"; // move cursor
      std::cout << CSI "K";                             // clear the line
      std::cout << message;
      std::cout.flush();
      std::cin >> inputChar;
    } while (std::string(allowed).find(inputChar) == std::string::npos);
    clearFormating();
    return inputChar;
  }

};

#endif // CONSOLEIO_H