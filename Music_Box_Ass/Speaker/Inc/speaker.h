/*
 * speaker.h
 *
 *  Created on: Oct 22, 2024
 *      Author: newton
 */

#ifndef INC_SPEAKER_H_
#define INC_SPEAKER_H_

#include "tim.h"
#include "usart.h"
#include "gpio.h"

uint32_t getNoteFrequency(char note, int octave);
uint8_t getNoteIndex(char note);
void playConstantBuzz(void);
void StopNote();
void playNoteFrequency(uint32_t frequency);

#endif /* INC_SPEAKER_H_ */
