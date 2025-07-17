#ifndef UTILITY_H
#define UTILITY_H
#include "SFML/Graphics/Color.hpp"


class Utility {
public:
    static int random_int(int min, int max);
    static float random_float(float min, float max);
    static bool random_bool();
    static sf::Color get_RGB(float hue, float sat, float val);
};



#endif //UTILITY_H
