/* Most parts of this class were originally developed by Lander Griffith (https://github.com/landr0id/).
   Much of his code is used throughout this class or very slightly modified */

#ifndef FATXHELPERS_H
#define FATXHELPERS_H

#define DownToNearestSector(x) (0xFFFFFFFFFFFFFE00 & x)
#define UpToNearestSector(x) ((x + 0x1FF) & 0x1FF)

#include "../winnames.h"

class XBOXINTERNALSSHARED_EXPORT FATXHelpers
{
};

#endif // FATXHELPERS_H
