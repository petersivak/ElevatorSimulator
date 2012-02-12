#include "mymath.hpp"

#include <cstdlib>
#include <cmath>
#include <QTime>

int realRandom()
{
    return abs( rand() * QTime::currentTime().msec() );
}
