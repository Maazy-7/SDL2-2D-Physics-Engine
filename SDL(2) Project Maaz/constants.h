#pragma once
#include <string>

#define yellow 255,255,0
#define red 255,0,0
#define blue 0,0,255
#define green 0,255,0
#define black 0,0,0
#define white 255,255,255
#define HEIGHT 1440
#define WIDTH 2550
#define PI 3.141592
#define Log(x) std::cout << x << "\n"
#define Log2(x,y) std::cout << x << ',' << y << "\n"
#define coolRound(x,n) round(x*pow(10,n)) / pow(10,n)
#define INFINITY std::numeric_limits<float>::infinity()
#define PI_180 0.0174532925 // pi/180 constant
#define INVPIx180 57.2957795131 // 180/pi constant
#define VERY_SMALL_VALUE 0.0005