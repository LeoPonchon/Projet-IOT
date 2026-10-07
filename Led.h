#pragma once

void ledInitialiser();
void ledAllumer();
void ledEteindre();
void ledBasculer();
void ledModeManuel();
void ledActiverAutomatique();
void ledActualiser(int valeurLumiere);

bool ledEstAllumee();
bool ledEstAutomatique();

int ledSeuilAllumage();
int ledSeuilExtinction();
bool ledDefinirSeuils(int allumage, int extinction);
