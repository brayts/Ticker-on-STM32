
#include<stm32f10x.h>
#include "lcd.h"

unsigned char *string;//указатель на текст бегущей строки
unsigned char pos;//выводимая позиция строки

volatile unsigned long count=0;

//обработчик прерывания SusTick таймера
void SysTick_Handler(void)
{
  count++;
}

//задержка, мс
void DelayMs(unsigned long c)
{
	count=0;
	while(count<c);
	return;
}

//установим текст бегущей строки
void SetString(unsigned char * str)
{
	string=str;
	pos=0;
	return;
}

//вывод очередного кадра бегущей строки
void UpdateScreen()
{
	unsigned char n;
	if(string[pos]==0)//если дошли до конца строки
		pos=0;
	for(n=0;n<16;n++)
	{
		if(string[pos+n]!=0)//если не дошли до конца строки
			SendByte(string[pos+n], 1);
		else
		{
			for(;n<16;n++)//добиваем пробелами до 16 символов
				SendByte(' ', 1);
			break;
		}
	}
	pos++;//переходим к следующей позиции строки
	return;
}
	
int main(void)
{
// Настраиваем тактирование. Через функцию из CMSIS. Все настройки прописаны в файле system_stm32f10x.c
// У меня там задана частота 72Мгц с тактированием от кварца на 8МUц
	SystemInit();	
	SysTick_Config(72000);//запустим таймер с периодом 1мс
	RCC -> APB2ENR |= RCC_APB2ENR_AFIOEN;//вкл тактирование AFIO
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;//отключаем JTAG для освобождения PB4
	InitializeLCD(); //Инициализация дисплея
	ClearLCDScreen(); //Очистка дисплея от мусора
	SetString("                STM32F103C8T6 SCROLLING TEXT!!!");//установим текст бегущей строки
	while(10)
	{
		Cursor(0,0);//установим курсор на заданную позицию
		UpdateScreen();//выведем очередной кадр бегущей строки
		DelayMs(10);//задержка в миллисекундах, влияет на скорость бегущей строки
	}
}
 
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %drn", file, line) */
 
  /* Infinite loop */
  while (1)
  {
  }
}
#endif
