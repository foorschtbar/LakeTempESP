#ifndef screens_h
#define screens_h

#include <HardwareSerial.h>

#define MAX_SCREENS 5

class Screens
{
public:
    Screens(int numofscreens, ...);
    uint8_t currentTick();
    uint8_t currentScreen();
    bool needRefresh();
    bool needFullRefresh();
    void reset();
    void debug(HardwareSerial &serial);
    int count();
    void loop();
    void nextScreen();

private:
    uint8_t _numofscreens;
    uint8_t _showtimeperscreen[MAX_SCREENS];
    uint8_t _currentScreen;
    uint8_t _currentTick;
    int _count;
    bool _needRefresh;
    bool _needFullRefresh;
    unsigned long _lastRefresh;
};

#endif
