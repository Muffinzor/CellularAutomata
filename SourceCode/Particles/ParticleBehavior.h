#ifndef PARTICLEBEHAVIOR_H
#define PARTICLEBEHAVIOR_H

#include "../Grid/CellularMatrix.h"

class ParticleBehavior {
public:
    static void solid_behavior(CellularMatrix&, int x, int y);
    static void liquid_behavior(CellularMatrix&, int x, int y);
};



#endif //PARTICLEBEHAVIOR_H
