#include "Arduino.h"

//  Eszett
const uint8_t Eszett[8] = {
    B01100,
    B10010,
    B10010,
    B10110,
    B10001,
    B10001,
    B10110,
    B10000,
};

// Grad
const uint8_t Degree[8] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000,
};

// Copy
const uint8_t Copy[8] = {
    B01110,
    B10001,
    B10111,
    B11001,
    B11001,
    B10111,
    B10001,
    B01110,
};

// Heart1
const uint8_t Heart1[8] = {
    B00000,
    B01010,
    B10101,
    B10001,
    B01010,
    B00100,
    B00000,
    B00000,
};

// Heart2
const uint8_t Heart2[8] = {
    B00000,
    B01010,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
    B00000,
};
