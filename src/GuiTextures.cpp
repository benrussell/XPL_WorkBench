//
// Created by Ben on 10/8/2024.
//

#include "GuiTextures.h"


bool GuiTextures::m_bDraw=true;

int GuiTextures::m_maxTexId=100;

std::function<void(int)> GuiTextures::openImageInspector;