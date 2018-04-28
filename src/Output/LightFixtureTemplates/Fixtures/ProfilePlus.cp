//
//  ProfilePlus.cpp
//  KimaticaOSCDMX
//
//  Created by Terry Clark on 09/06/2017.
//
//

#include "ProfilePlus.hpp"

ProfilePlus::ProfilePlus() {
}

ProfilePlus::~ProfilePlus(){
    
}

void ProfilePlus::update(){
    dmx->setLevel(address, color.x);
    dmx->setLevel(address + 1, color.y);
    dmx->setLevel(address + 2, color.z);
    dmx->setLevel(address + 3, uv);
    dmx->setLevel(address + 4, color.w);
}
