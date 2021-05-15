#include "Screens.h"
#include <HardwareSerial.h>
#include <Arduino.h>

Screens::Screens(int numofscreens, ...)
{
    _count = 0;
    _currentScreen = 0;
    _numofscreens = numofscreens;
    _lastRefresh = millis();

    va_list valist;
    va_start(valist, numofscreens); //initialize valist for num number of arguments
    for (int i = 0; i < numofscreens; i++)
    {
        if (i < MAX_SCREENS)
        {
            _showtimeperscreen[i] = (int)va_arg(valist, int);
        }
    }
    va_end(valist); //clean memory reserved for valist
}

uint8_t Screens::currentScreen()
{
    return _currentScreen;
}

int Screens::count()
{
    return _numofscreens;
}

uint8_t Screens::currentTick()
{
    return _currentTick;
}

void Screens::debug(HardwareSerial &serial)
{
    serial.print(F("currentScreen: "));
    serial.print(_currentScreen);
    serial.println();
    serial.print(F("showtime: "));
    serial.print(_showtimeperscreen[_currentScreen]);
    serial.println();
    serial.print(F("count: "));
    serial.print(_count);
    serial.println();
    serial.print(F("currentTick: "));
    serial.print(_currentTick);
    serial.println(F("\n------------------"));
}

void Screens::loop()
{
    if ((millis() - _lastRefresh) > 1000)
    {
        _currentTick = !_currentTick;

        _needRefresh = true;
        _lastRefresh = millis();
        _count++;

        if (_count == _showtimeperscreen[_currentScreen])
        {
            _currentScreen++;
            _needFullRefresh = true;
            _count = 0;
        }
        if (_currentScreen >= _numofscreens)
        {
            _currentScreen = 0;
        }
    }
}

void Screens::nextScreen()
{
    _currentScreen++;
    if (_currentScreen >= _numofscreens)
    {
        _currentScreen = 0;
    }
    _needRefresh = true;
    _needFullRefresh = true;
    _count = 0;
}

bool Screens::needRefresh()
{
    if (_needRefresh)
    {
        _needRefresh = false;
        return true;
    }
    else
    {
        return false;
    }
}
bool Screens::needFullRefresh()
{
    if (_needFullRefresh)
    {
        _needFullRefresh = false;
        return true;
    }
    else
    {
        return false;
    }
}
void Screens::reset()
{
    _currentScreen = 0;
    _count = 0;
    _needRefresh = true;
    _needFullRefresh = true;
}