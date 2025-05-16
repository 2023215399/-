#include "ADC_single_ch_continuous.h"
#include "stm32f10x.h"
#include "Delay.h"
#include "usart.h"
#include "led.h"
#include "pwm.h"
#include "key.h"
#include "stdio.h"

volatile float threshold = 2.7f; 
volatile float voltage;
volatile int  a=1;
void USART1_Init(void);
int fputc(int ch, FILE *f);

 
int main(void)
{
   uint8_t i;	
	 
	
	 USART1_Init();  
   led_init();
   PWM_Init();
	 exti_init();
   adc_init();
  
	 ADC_SoftwareStartConvCmd(ADC1,ENABLE);
  
   for(int i=0; i<2; i++)
	 {
	    GPIO_ResetBits(GPIOB,GPIO_Pin_3);
      Delay_ms(1000);
      GPIO_SetBits(GPIOB,GPIO_Pin_3);
      Delay_ms(1000);
	 }
	
	while(1)
  { 
	
 	    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	    voltage = ADC_GetConversionValue(ADC1)*3.3/4096;
		
  	  if(voltage > threshold) 
      {
              
           for (i = 0; i <= 50; i++) 
           {
               PWM_SetCompare1(i * 2);      
               Delay_ms(5);                 
           }
           for (i = 0; i <= 50; i++) 
           {
               PWM_SetCompare1(100 - i*2);  
               Delay_ms(5);                 
           }
      }
      else 
     {
           GPIO_ResetBits(GPIOB,GPIO_Pin_3);

     }
		       printf("Volt: %.2fv\r\n",voltage);
           printf("threshold: %.2fv\r\n",threshold);
	    	   Delay_ms(1000);  
    }
}


void adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_InitStructure;

    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  

    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  

    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    
    TIM_InitStructure.TIM_Period = 200 - 1;          // 
    TIM_InitStructure.TIM_Prescaler = 7200 - 1;      //  (72MHz/7200=10kHz)
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_InitStructure);
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); 
    TIM_Cmd(TIM3, ENABLE);  

    
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);  

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);  

    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

void EXTI0_IRQHandler(void)
{
     
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
             if(threshold == 2.2f) 
	{	
	  threshold = 2.7f;
	 a=1;
	printf("the threshold is 2.7v\n");
	}
            else if(threshold == 2.7f) 
	{
	 threshold = 3.2f;
	a=2;
	printf("the threshold is 3.2v\n");
	}	   
            else 
{
  threshold = 2.2f;
	a=3;
	printf("the threshold is 2.2v\n" );
	}
         
        EXTI_ClearITPendingBit(EXTI_Line0);  
   }
}


for (i = 0; i <= 50; i++) 
           {
               PWM_SetCompare1(i * 2);      
               Delay_ms(5);                 
           }
           for (i = 0; i <= 50; i++) 
           {
               PWM_SetCompare1(100 - i*2);  
               Delay_ms(5);                 
           }
      }

void USART1_IRQHandler(void)
{
	           

    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        
        if(data == 0x21) 
        {
   switch(a) {
    case 1:
        printf("the threshold is 2.7v\n");
        break;
    case 2:
        printf("the threshold is 3.2v\n");
        break;
    case 3:
        printf("the threshold is 2.2v\n");
        break;
    default:
        printf("Invalid threshold level!\n");  
        break;
}
        }
 else 
  {
            printf("invalid instruction.\r\n");
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

 
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}
