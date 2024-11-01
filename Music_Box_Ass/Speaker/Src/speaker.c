/*
 * speaker.c
 *
 *  Created on: Oct 22, 2024
 *      Author: newton
 */

#include "speaker.h"

void StopNote()
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
}

uint8_t getNoteIndex(char note)
{
	switch(note)
	{
	case 'C': return 0;
	case 'c': return 1;
	case 'D': return 2;
	case 'd': return 3;
	case 'E': return 4;
	case 'F': return 5;
	case 'f': return 6;
	case 'G': return 7;
	case 'g': return 8;
	case 'A': return 9;
	case 'a': return 10;
	case 'B': return 11;
	default: return -1;
	}
}

void playNoteFrequency(uint32_t frequency)
{
    if ((frequency > 0) && (frequency <= 10000))  // Only handle frequencies in the buzzer's range
    {
        /* Calculate the timer period for the given frequency */
        uint32_t timerPeriod = (SystemCoreClock / (frequency * 2)) - 1;  // Timer clock / (2 * frequency)

        /* Update the period dynamically */
        __HAL_TIM_SET_AUTORELOAD(&htim2, timerPeriod);

        /* Set a 50% duty cycle for the note (adjustable) */
        uint32_t pulseValue = timerPeriod / 2;
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulseValue);

        /* Start the PWM signal */
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    }
    else
    {
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);  // Stop the signal if the frequency is out of range
    }
}

uint32_t getNoteFrequency(char note, int octave)
{
	uint32_t frequencies[12][5] = {
			{32, 65, 130, 261, 523},  // C
			{34, 69, 138, 277, 554},  // C#
			{36, 73, 146, 293, 587},  // D
			{38, 77, 155, 311, 622},  // D#
			{41, 82, 164, 329, 659},  // E
			{43, 87, 174, 349, 698},  // F
			{46, 92, 185, 369, 739},  // F#
			{49, 98, 196, 392, 784},  // G
			{52, 104, 208, 415, 830}, // G#
			{55, 110, 220, 440, 880}, // A
			{58, 116, 233, 466, 932}, // A#
			{61, 123, 246, 493, 987}  // B
	};

	int noteIndex = getNoteIndex(note);

	if(noteIndex == -1)
	{
		return 0;
	}

	return frequencies[noteIndex][octave];
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	char receivedData[2];
	HAL_UART_Receive(&huart2,(uint8_t*)receivedData, 2, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, 2, HAL_MAX_DELAY);

	char note = receivedData[0];
	int octave = receivedData[1]-'1';

	uint32_t frequency = getNoteFrequency(note,octave);

	playNoteFrequency(frequency);

	HAL_Delay(500);
}
