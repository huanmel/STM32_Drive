/*
 * command_parser.c
 *
 *  Created on: 10 нояб. 2016 г.
 *      Author: meleshinis
 */

#include "motor_control.h"
#include "application_data.h"

uint8_t PrevCommand; //command to PWM at previous step.
uint8_t TRSHLD_min=(int)(10*255/100);

/*!
 * @brief Parse command from UART and make PWM duty through threshold of the counter
 * @param[in]  APPLICATION_DATA.external_command  global variable
 * @param[out] APPLICATION_DATA.counter_state.TRSHLD global variable
 */

void command_parser(void)
{
	int tempTRSHLD; // Temporal Variable for TRSHLD calculation
	switch(APPLICATION_DATA.external_command){
	case 'S':
		//	LLR_0014 Команда запуска генерации ШИМ Команда выдается в виде символа "S"
		//	LLR_0018 Старт устройства При старте устройства выставляется минимальная скважность
		// 	LLR_0002 Минимальная скважность ШИМ Должна быть обеспечена минимальная скважность в 10%
		// 	TODO: 10% of [0..255] = 25.6 Lets consider that 10% is 25 for simplicity or 26 for accuracy
		APPLICATION_DATA.counter_state.TRSHLD=TRSHLD_min;
		PrevCommand=10; // initialize previous command var
		break;
	case 'E':
		// LLR_0015 Команда остановки генерации ШИМ Команда выдается в виде символа "E"
		APPLICATION_DATA.counter_state.TRSHLD=0;
		PrevCommand=0;
		break;
	case 'F':
		//		LR_0012 Шаг изменения скважности ШИМ Шаг изменения скважности ШИМ должен быть 10%
		//		 LLR_0013 Команда на изменение скважности ШИМ Команда выдается в виде символа:
		//		"F" - увеличение скважности
		//		"R" - уменьшение скважности

		if (APPLICATION_DATA.counter_state.TRSHLD<255)
		{
			tempTRSHLD=(int)((PrevCommand+10)*255/100);

			if (tempTRSHLD>255)
			{
				tempTRSHLD=255;
				PrevCommand=100;
			}
			else
			{
				PrevCommand=PrevCommand+10;
				if (PrevCommand>100)
				{
					PrevCommand=100;
				}
			}

			APPLICATION_DATA.counter_state.TRSHLD=tempTRSHLD;
		}
		break;
	case 'R':
		//		"R" - уменьшение скважности
		if (APPLICATION_DATA.counter_state.TRSHLD>TRSHLD_min)
		{
			tempTRSHLD=(int)((PrevCommand-10)*255/100);

			if (tempTRSHLD<TRSHLD_min)
			{
				tempTRSHLD=TRSHLD_min;
				PrevCommand=10;
			}
			else
			{
				PrevCommand=PrevCommand-10;
				if (PrevCommand<10)
				{
					PrevCommand=10;
				}
			}

			APPLICATION_DATA.counter_state.TRSHLD=tempTRSHLD;
		}
		break;
	default:
		// nothing change;
		break;
	}
}



