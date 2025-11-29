#pragma once

#include "Pnt.h" // for CPnt

void UserAxisInit();
void UserAxisSetInfAng(double);
CPnt UserAxisSnapLn(const CPnt&, const CPnt&);
