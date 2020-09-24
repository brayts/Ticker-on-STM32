//#include "stm32f10x_gpio.h"
#include "GPIO_STM32F10x.h"

//---Переопределяем порты для подключения дисплея, для удобства---//
#define     LCM_OUT               GPIOB->ODR
#define     LCM_PIN_RS            GPIO_Pin_0          // PB0
#define     LCM_PIN_EN            GPIO_Pin_1          // PB1
#define     LCM_PIN_D7            GPIO_Pin_7          // PB7
#define     LCM_PIN_D6            GPIO_Pin_6          // PB6
#define     LCM_PIN_D5            GPIO_Pin_5          // PB5
#define     LCM_PIN_D4            GPIO_Pin_4          // PB4
#define     LCM_PIN_MASK  ((LCM_PIN_RS | LCM_PIN_EN | LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5 | LCM_PIN_D4))
GPIO_InitTypeDef  GPIO_InitStructure;
 
//---Функция задержки в микросекундах---//
void DelayUs(int a)
{
    //int i = 0;
    int f = 0;
		a*=4;
    while(f < a)
    {
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			f++;
    }
}
 
//---Нужная функция для работы с дисплеем, по сути "дергаем ножкой" EN---//
/*void PulseLCD()
{
    LCM_OUT |= LCM_PIN_EN;
    DelayUs(50);
    LCM_OUT &= ~LCM_PIN_EN;
    DelayUs(50);
}*/
 
//---Отсылка байта в дисплей---//
//IsData = 0 - отправка команды
//IsData = 1 - отправка данных
void SendByte(char ByteToSend, int IsData)
{
		DelayUs(50);//задержка на выполнение предыущей команды
		//передаем старшую тетраду
	  LCM_OUT &= (~(LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5 | LCM_PIN_D4));
    if (IsData == 1)
        LCM_OUT |= LCM_PIN_RS;//RS=1
    else
        LCM_OUT &= ~LCM_PIN_RS;//RS=0
		LCM_OUT |= LCM_PIN_EN;//EN=1
    LCM_OUT |= (ByteToSend & 0xF0);
    DelayUs(1);
		LCM_OUT &= ~LCM_PIN_EN;//EN=0
		//передаем младшую тетраду
    LCM_OUT &= (~(LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5 | LCM_PIN_D4));
		LCM_OUT |= LCM_PIN_EN;//EN=1
    LCM_OUT |= ((ByteToSend & 0x0F) << 4);
    DelayUs(1);
		LCM_OUT &= ~LCM_PIN_EN;//EN=0
}
 
//---Установка позиции курсора---//
void Cursor(char Row, char Col)
{
   char address;
   if (Row == 0)
   address = 0;
   else
   address = 0x40;
   address |= Col;
   SendByte(0x80 | address, 0);
}
 
//---Очистка дисплея---//
void ClearLCDScreen()
{
    SendByte(0x01, 0);
		DelayUs(2000);
}
 
//---Инициализация дисплея---//
void InitializeLCD(void)
{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//включаем тактирование GPIOB
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;// Режим "Выход" Push-Pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);          // Применяем настройки на порт B

    LCM_OUT &= ~(LCM_PIN_MASK);
		//передаем инициализирующую последовательность
		DelayUs(15000);//15мс
		LCM_OUT |= LCM_PIN_EN;//EN=1
		LCM_OUT=0x30;
	  DelayUs(1);
    LCM_OUT &= ~LCM_PIN_EN;//EN=0
	
		DelayUs(4100);//4,1мс
		LCM_OUT |= LCM_PIN_EN;//EN=1
		LCM_OUT=0x30;
	  DelayUs(1);
		LCM_OUT &= ~LCM_PIN_EN;//EN=0
	
		DelayUs(100);//100мкс
		LCM_OUT |= LCM_PIN_EN;//EN=1
		LCM_OUT=0x30;
		DelayUs(1);
		LCM_OUT &= ~LCM_PIN_EN;//EN=0
	
    SendByte(0x2, 0);//4х битный режим
    SendByte(0x28, 0);
		SendByte(0x0C, 0);//выключим курсор
    SendByte(0x06, 0);
}
 
//---Печать строки---//
void PrintStr(char *Text)
{
    char *c;
    c = Text;
    while ((c != 0) && (*c != 0))
    {
        SendByte(*c, 1);
        c++;
    }
}