#ifndef AI_CHAT_H
#define AI_CHAT_H

#include <Arduino.h>

extern String lastAIAnswer;
extern String lastAIQuestion;

String askAI(String question);
void handleSerialAI();

#endif
