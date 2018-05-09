//
//  Wash.hpp
//  GoldsmithsDMX
//
//  Created by Terry Clark on 09/06/2017.
//
//

#pragma once

#include "Wash.hpp"

/* Washes
 MH2: Shutter 1,
 Dimmer 2,
 Pan 3, 4
 Tilt 5, 6
 Colour Wheel 8
 R,G,B,W =  9, 10, 11, 12
 */

class MH2 : public Wash {
protected:
    
public:
    MH2();
    ~MH2();
    
    void update();
    void on(float mouse);
    void fadeIn();
    void fadeOut();
    void pulseSlow(float speed);
};
