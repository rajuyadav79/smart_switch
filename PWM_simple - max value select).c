/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2020 nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************/
/* Website: http://www.nuvoton.com                                                                         */
/*  E-Mail : MicroC-8bit@nuvoton.com                                                                       */
/*  Date   : Jan/21/2020                                                                                   */
/***********************************************************************************************************/

/************************************************************************************************************/
/*  File Function: MS51 PWM simple output demo                                                              */
/************************************************************************************************************/
#include "MS51_16K.H"
#include<stdlib.h>
/************************************************************************************************************
*    Main function 
************************************************************************************************************/


struct
{
    unsigned int a;
		unsigned int z;
	
    unsigned long b;
    unsigned char  c;

} StructData;


///* if define TIMER0_FSYS_DIV12, timer = (0x1FFF-0x1000)*12/24MHz = 4.08ms */
///* if define TIMER0_FSYS, timer = (0x1FFF-0x0010)/24MHz = 340us */
//#define TH0_INIT        0x00 
//#define TL0_INIT        0x10


/* if define TIMER0_FSYS_DIV12, timer = (256-56)*12/246MHz = 100us */
/* if define TIMER0_FSYS, timer = (256-56)/24MHz = 8.33us */

#define TH0_INIT        56 
#define TL0_INIT        56

unsigned char xdata ADCdataAIN5H, ADCdataAIN5L;
unsigned char xdata ADCdataVBGH, ADCdataVBGL;
unsigned int xdata ADCdata,ADCdata1;
unsigned int xdata ADCcount;

unsigned long xdata ADC_stable,ADC_stable_prev,ADC_stable_curent,dif_value,Temp_ADC_stable;
unsigned long xdata ADC_Store;

unsigned int count_gain;
unsigned int gain_value;


void MSDelay(unsigned int value); 

void Delay(unsigned int value);

unsigned int counter=0;
unsigned int temp_counter=0;



unsigned char buff[6];


unsigned char xdata ADCdataAIN5H, ADCdataAIN5L;
unsigned char xdata ADCdataVBGH, ADCdataVBGL;

//unsigned int xdata ADCdata;

unsigned char delay_time=400;
 float xdata final_amp;
 float xdata f_disp_amp;
 float xdata k_watt;


unsigned long i;
unsigned int counter_ms=0;
unsigned int counter_sec=0;
unsigned int counter_min;


//unsigned long accurate_adc_count,adc_count;
/************************************************************************************************************
*    TIMER 0 interrupt subroutine
************************************************************************************************************/
unsigned long watt,temp_watt;

unsigned int unit;
bit unit_update=0;
void Timer0_ISR (void) interrupt 1           /*interrupt address is 0x000B */
{
    _push_(SFRS);
//    TH0 = TH0_INIT;
//    TL0 = TL0_INIT;
//    TF0 = 0 ;
			i++;
			if(i>120)
			{
				i=0;
				counter_ms++;
				if(counter_ms>1000)
				{
						counter_ms=0; //P12 = ~P12;    // GPIO1 toggle when interrup
//						watt += 230 * f_disp_amp;
//							if(watt>60000){unit++; watt=0;}
				
							watt += (230 * f_disp_amp);
							temp_watt = watt;
					
								k_watt = (230 * (float)f_disp_amp)/1000;  //k_watt = (float)temp_watt/1000;
					
						//	watt_1=watt;
							// printf("\t\t\n unit=%ld ,watt=%ld",unit,watt); 
							if(watt>3600000)
								{
									unit++; watt=watt-3600000; unit_update=1;
								}
						 
							//	printf("\t\t counter_ms=%d ,counter_sec=%d",counter_ms,counter_sec); 
							/******************* calcultion code *********************/
				}		
			}
    _pop_(SFRS);
}


void Delay(unsigned int value)
	{
		unsigned int x;
		for(x=0; x<value; x++);
	}

	
// Function to set the kth bit of n
int setBit(int n, int k)
{
    return (n | (1 << (k - 1)));
}

//// Function to return kth bit on n
//int getBit(int n, int k)
//{
//    return ((n >> k) & 1);
//}

// Function to clear the kth bit of n
int clearBit(int n, int k)
{
    return (n & (~(1 << (k - 1))));
}


	
char r;
unsigned int key=0;
bit complete;
	
int show_data_time_counter=0;
unsigned int c;

bit K;
idata unsigned long delay_counter;

bit High_value_get;
void main(void)
{
		//unsigned int tempw = 15;
	
 //   ALL_GPIO_QUASI_MODE;
//unsigned char q;
		P10_QUASI_MODE;
		P11_QUASI_MODE;
		P12_QUASI_MODE;
		
		P13_QUASI_MODE;
		P14_QUASI_MODE;
		P15_QUASI_MODE;
		P16_QUASI_MODE;
		P17_QUASI_MODE;

	
	
   P06_PUSHPULL_MODE;
   P07_INPUT_MODE;
	
/********************************* end for display *****************************/
	
	  /* UART0 settting for printf function */
    MODIFY_HIRC(HIRC_24);
    UART_Open(24000000,UART0_Timer3,9600);
    ENABLE_UART0_PRINTF;

	
    P03_QUASI_MODE;
		P01_QUASI_MODE;

	
		PWM4_P01_OUTPUT_ENABLE;

		PWM5_P03_OUTPUT_ENABLE;

  
    PWM_IMDEPENDENT_MODE;
    PWM_CLOCK_DIV_8;
    
		PWMPH = 0x30;
    PWMPL = 0x0f;
		
		/******************************** timer 0 *************************/
		// P12_PUSHPULL_MODE;
    ENABLE_TIMER0_MODE2;
    TIMER0_FSYS;
  
    TH0 = TH0_INIT;            //initial counter values 
    TL0 = TL0_INIT;    
   
    ENABLE_TIMER0_INTERRUPT;                       //enable Timer0 interrupt
    ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts
  
    set_TCON_TR0;                                    //Timer0 run
		
		
//		ENABLE_TIMER1_MODE0;                           /* Timer 0 mode configuration */
//    TIMER0_FSYS_DIV12;

//    TH0 = TH0_INIT;
//    TL0 = TL0_INIT;
      
//    ENABLE_TIMER0_INTERRUPT;                       /* enable Timer0 interrupt  */ 
//    ENABLE_GLOBAL_INTERRUPT;                       /* enable interrupts */

//    set_TCON_TR0;                                  /* Timer0 run */
		
		/********************************End  timer 0 *************************/
counter=0;
K=0;


//								StructData.a=0;
//								Write_DATAFLASH_ARRAY(0x38FE,(unsigned char *)&StructData,sizeof(StructData));//write structure
  
	/*call read byte */
    count_gain = ((read_APROM_BYTE(0x38FE)<<8)+read_APROM_BYTE(0x38FF));
		
		unit = ((read_APROM_BYTE(0x39FE)<<8)+read_APROM_BYTE(0x39FF));

//		printf ("\n system16highsite = 0x%X", system16highsite);
		printf ("\n system16highsite decimal = %d", count_gain);

if(unit<0)unit=0;


key=0; delay_counter=1;
   ENABLE_ADC_AIN5;
while(1)
		{
			if(P11==1)
			{
				key = setBit(key, 1);
			//	Timer1_Delay(24000000,800,500);	q=0;
			}
			else if(P11==0)
			{
				key = clearBit(key, 1);
				//	Timer1_Delay(24000000,800,500);	q=0;
			}
		
			if(P12==1)
			{
				key = setBit(key, 2);
				//	Timer1_Delay(24000000,800,500);	q=0;
			}
			else if(P12==0)
			{
				key = clearBit(key, 2);
				//	Timer1_Delay(24000000,800,500);	q=0;
			}

			if(P13==1)
			{
				key = setBit(key, 3);
				//	Timer1_Delay(24000000,800,500);	q=0;
			}
			else if(P13==0)
			{
				key = clearBit(key, 3);
				//	Timer1_Delay(24000000,800,500);	q=0;
			}
		 if(P14==1)
			{
				  key = setBit(key, 4);
					//Timer1_Delay(24000000,800,500);	q=0;
			}
			else if(P14==0)
			{
				key = clearBit(key, 4);
				//Timer1_Delay(24000000,800,500);	q=0;
			}
			
			
			/*Enable channel 5 */
			ADCRH=0;
			ADCRL=0;	
   
      ADCCON1|=0X30;            /* clock divider */
      ADCCON2|=0X0E;            /* AQT time */
      AUXR1|=SET_BIT4;          /* ADC clock low speed */
      clr_ADCCON0_ADCF;
      set_ADCCON0_ADCS;                                
      while(ADCF == 0);
      ADCdataAIN5H = ADCRH;
      ADCdataAIN5L = ADCRL;
    //  DISABLE_ADC;
    //  ADCdata=0;

			//ADCdata=ADCdataAIN5H + ADCdataAIN5L ;
			ADCdata = ADCdataAIN5H;
			ADCdata = ADCdata<<4;
			ADCdata = ADCdata + ADCdataAIN5L ;
		
			//printf("\n\r\t ADCdata=%u", ADCdata);	
			if(ADCdata > ADCdata1) {ADCdata1 = ADCdata;  } //complete=1;
			
			if(ADCdata == 0){ 
				
				if((ADCdata <= ADCdata1)) 
					{
						//printf("\t\t ADCdata1=%u", ADCdata1);
						
					 ADC_Store+=ADCdata1;	ADCdata1=0; counter++; 	//printf("\t\t ADC_Store=%u", ADC_Store);
					} 
			
			}
		
			
			
			
			
	//		printf("\t\t ADCcount=%u", ADCcount);
		
	if(counter > 100)
		{
			
					Temp_ADC_stable = ADC_Store/counter; //ADC_stable=ADC_Store/counter;
					
					ADC_stable=Temp_ADC_stable;
					Temp_ADC_stable=0;
					counter =0; ADC_Store=0;  
			
			//		printf("\n\t\t ADC_stable=%u", ADC_stable);
				
				//ADC_stable_prev,dif_value
				//ADC_stable_curent
				
				
			ADC_stable_curent = ADC_stable; 
			if(ADC_stable_curent > ADC_stable_prev)													 
     	{
      		dif_value = ADC_stable_curent - ADC_stable_prev;
      		if(dif_value < 15)
      		ADC_stable = ADC_stable_prev;
			}
  		else if(ADC_stable_curent < ADC_stable_prev)
   		{
    		dif_value =  ADC_stable_prev - ADC_stable_curent;
    		if(dif_value < 15)
    		ADC_stable = ADC_stable_prev;
   		} 
			ADC_stable_prev=ADC_stable;
				
		}
			
			show_data_time_counter++;
			if(show_data_time_counter>50)
			{	
				show_data_time_counter=0;
				
				if(P05==0 && K==0)
					{	
								c++; if(c>10) K=1;
								count_gain = ADC_stable;
								StructData.a=ADC_stable;
								Write_DATAFLASH_ARRAY(0x38FE,(unsigned char *)&StructData,sizeof(StructData));//write structure
								/***************data write in iap ******************************/				
				  }	

//				printf("\n\r\t ADC_stable=%u", ADC_stable); 
//				final_amp = (ADC_stable*0.0108);
//				printf("\n\r\t ADC_cureent_Amp= %0.2f", final_amp); show_data_time_counter=0;
//				

				final_amp = ((float)ADC_stable * (20.00)) / count_gain;
			
					
			//	printf("\n Actual Amp %0.1f", Temp_final_amp); 


				if(final_amp == 0.0 ) f_disp_amp = 0;

				else if(final_amp >= 0.1 && final_amp  <=  0.5) f_disp_amp = (final_amp* (1.0520));
				else if(final_amp >= 0.6 && final_amp  <=  1.0) f_disp_amp = (final_amp* (1.0520));

					
				else if(final_amp >= 1.1 && final_amp  <=  1.5) f_disp_amp = (final_amp* (1.0526));
				else if(final_amp >= 1.6 && final_amp  <=  2.0) f_disp_amp = (final_amp* (1.0526));

				else if(final_amp >= 2.1 && final_amp  <=  2.5) f_disp_amp = (final_amp* (1.1111));
				else if(final_amp >= 2.6 && final_amp  <=  3.0) f_disp_amp = (final_amp* (1.1111));
				
				else if(final_amp >= 3.1 && final_amp  <=  3.5) f_disp_amp = (final_amp* (1.0811));
				else if(final_amp >= 3.6 && final_amp  <=  4.0) f_disp_amp = (final_amp* (1.0811));
				
				else if(final_amp >= 4.1 && final_amp  <=  4.5) f_disp_amp = (final_amp* (1.1111));
				else if(final_amp >= 4.6 && final_amp  <=  5.0) f_disp_amp = (final_amp* (1.1111));
				
				
				else if(final_amp >= 5.1 && final_amp  <=  5.5) f_disp_amp = (final_amp* (1.0714));
				else if(final_amp >= 5.6 && final_amp  <=  6.0) f_disp_amp = (final_amp* (1.0714));
				
				
				else if(final_amp >= 6.1 && final_amp  <=  6.5) f_disp_amp = (final_amp* (1.0294));
				else if(final_amp >= 6.6 && final_amp  <=  7.0) f_disp_amp = (final_amp* (1.0294));
				
				
				else if(final_amp >= 7.1 && final_amp  <=  7.5) f_disp_amp = (final_amp* (1.0127));
				else if(final_amp >= 7.6 && final_amp  <=  8.0) f_disp_amp = (final_amp* (1.0127));
				
				
				else if(final_amp >= 8.1 && final_amp  <=  8.5) f_disp_amp = (final_amp* (1.0112));
				else if(final_amp >= 8.6 && final_amp  <= 9.0) f_disp_amp = (final_amp* (1.0112));
				
				else if(final_amp >= 9.1 && final_amp  <= 9.5) f_disp_amp = (final_amp* (1.0000));
				else if(final_amp >= 9.6 && final_amp  <= 10.0) f_disp_amp = (final_amp* (1.0000));
				
				else if(final_amp >= 10.1 && final_amp <= 10.5) f_disp_amp = (final_amp* (1.0185));
				else if(final_amp >= 10.6 && final_amp <= 11.0) f_disp_amp = (final_amp* (1.0185));
				
				else if(final_amp >= 11.1 && final_amp <= 11.5) f_disp_amp = (final_amp* (1.0084));
				else if(final_amp >= 11.6 && final_amp <= 12.0) f_disp_amp = (final_amp* (1.0084));
				
				else if(final_amp >= 12.1 && final_amp <= 12.5) f_disp_amp = (final_amp* (0.9924));
				else if(final_amp >= 12.6 && final_amp <= 13.0) f_disp_amp = (final_amp* (0.9924));
				
				else if(final_amp >= 13.1 && final_amp <= 13.5) f_disp_amp = (final_amp* (0.9722));
				else if(final_amp >= 13.6 && final_amp <= 14.0) f_disp_amp = (final_amp* (0.9722));
				
				else if(final_amp >= 14.1 && final_amp <= 14.5) f_disp_amp = (final_amp* (0.9554));
				else if(final_amp >= 14.6 && final_amp <= 15.0) f_disp_amp = (final_amp* (0.9554));
				
				
				else if(final_amp >= 15.1 && final_amp <= 15.5) f_disp_amp = (final_amp* (0.8989));
				else if(final_amp >= 15.6 && final_amp <= 16.0) f_disp_amp = (final_amp* (0.8989));


				else if(final_amp >= 16.1 && final_amp <= 16.5) f_disp_amp = (final_amp* (0.9043));
				else if(final_amp >= 16.6 && final_amp <= 17.0) f_disp_amp = (final_amp* (0.9043));
				
				else if(final_amp >= 17.1 && final_amp <= 17.5) f_disp_amp = (final_amp* (0.8955));
				else if(final_amp >= 17.6 && final_amp <= 18.0) f_disp_amp = (final_amp* (0.8955));

				else if(final_amp >= 18.1 && final_amp <= 18.5) f_disp_amp = (final_amp* (0.8716));
				else if(final_amp >= 18.6 && final_amp <= 19.0) f_disp_amp = (final_amp* (0.8716));

				else if(final_amp >= 19.1 && final_amp <= 19.5) f_disp_amp = (final_amp* (0.8658));
				else if(final_amp >= 19.6 && final_amp <= 20.0) f_disp_amp = (final_amp* (0.8658));

				else if(final_amp >= 20.1 && final_amp <= 20.5) f_disp_amp = (final_amp* (0.8750));
				else if(final_amp >= 20.6 && final_amp <= 21.0) f_disp_amp = (final_amp* (0.8750));

				else if(final_amp >= 21.1 && final_amp <= 21.5) f_disp_amp = (final_amp* (0.8661));
				else if(final_amp >= 21.6 && final_amp <= 22.0) f_disp_amp = (final_amp* (0.8661));


				else if(final_amp >= 22.1 && final_amp <= 22.5) f_disp_amp = (final_amp* (0.8779));
				else if(final_amp >= 22.6 && final_amp <= 23.0) f_disp_amp = (final_amp* (0.8779));


				else if(final_amp >= 23.1 && final_amp <= 23.5) f_disp_amp = (final_amp* (0.8779));
				else if(final_amp >= 23.6 && final_amp <= 24.0) f_disp_amp = (final_amp* (0.8779));

				else if(final_amp >= 24.1 && final_amp <= 24.5) f_disp_amp = (final_amp* (0.8889));
				else if(final_amp >= 24.6 && final_amp <= 25.0) f_disp_amp = (final_amp* (0.8889));
				
				else if(final_amp >= 25.1 && final_amp <= 25.5) f_disp_amp = (final_amp* (0.8889));
				else if(final_amp >= 25.6 && final_amp <= 26.0) f_disp_amp = (final_amp* (0.8889));
				
				else if(final_amp >= 26.1 && final_amp <= 26.5) f_disp_amp = (final_amp* (0.8889));
				else if(final_amp >= 26.6 && final_amp <= 27.0) f_disp_amp = (final_amp* (0.8889));
				
				else if(final_amp >= 27.1 && final_amp <= 27.5) f_disp_amp = (final_amp* (0.8889));
				else if(final_amp >= 27.6 && final_amp <= 28.0) f_disp_amp = (final_amp* (0.8889));
				
				else if(final_amp >= 28.1 && final_amp <= 28.5) f_disp_amp = (final_amp* (0.8889));
				else if(final_amp >= 28.6 && final_amp <= 29.0) f_disp_amp = (final_amp* (0.8889));
				
				
				else if(final_amp >= 29.1 && final_amp <= 29.5) f_disp_amp = (final_amp* (0.8889));
				else if(final_amp >= 29.6 && final_amp <= 30.0) f_disp_amp = (final_amp* (0.8889));
				
				else if(final_amp >= 30.1) f_disp_amp = 30.0;
				
				
 //				 f_disp_amp = final_amp; // temp testing
				
				
				//printf("\nDisplay Amp=%0.1f switch=%d,Unit = %d,watt = %ld,k_watt = %ld",f_disp_amp,key,unit,watt,k_watt); 
				//printf("\n%d,%d,%ld",key,unit,k_watt); 
				
			
			
				//printf("%02d%04d%04d\n",key,unit,k_watt); 
				
			
					
			delay_counter++;
			if(delay_counter>5)
			{
				printf("\n%02d%04d%008.3f",key,unit,k_watt);
				printf("%005.2f", f_disp_amp);  
			  printf("\t\t%005.2f", final_amp);  
				delay_counter=1;
			}
			

				
				//Timer1_Delay(24000000,5000,500);
					
					if(unit_update==1)
					{	unit_update=0;
						StructData.z=unit;
						Write_DATAFLASH_ARRAY(0x39FE,(unsigned char *)&StructData,sizeof(StructData));//write structure
					}
		}	
	
			
			
			
//			printf("\n\r\t ADc_count=%u", ADCcount);
//			unsigned long accurate_adc_count,adc_count;
//			printf("\r\t ADc_data =%u", ADCdata);
//			ADCdata = ADCdata/5;
//			
//			printf("\n\n\t\t ADc_data =%u", ADCdata);
//			printf ("ADCH 0x%bx%bx\n",ADCdataAIN5H,ADCdataAIN5L);
//			
//			printf("\n ADC channel 5 =0x%bx", ADCdataAIN5H);
//			printf("\n ADC channel 5 =0x%bx", ADCdataAIN5L);
//			
//			printf("\n ADC channel H 5 =%u ", ADCdataAIN5H);
//			printf("\n ADC channel L 5 =%u ", ADCdataAIN5L);
//	Timer1_Delay(24000000,5,5);
		}	
		

}



/*  old data
//				if(final_amp >= 0.0 && final_amp  <=  1.0) f_disp_amp = (final_amp* (1.0));
//				else if(final_amp >= 1.1 && final_amp  <=  2.0) f_disp_amp = (final_amp* (1.54));
//				else if(final_amp >= 2.1 && final_amp  <=  3.0) f_disp_amp = (final_amp* (1.50));
//				else if(final_amp >= 3.1 && final_amp  <=  4.0) f_disp_amp = (final_amp* (1.43));
//				else if(final_amp >= 4.1 && final_amp  <=  5.0) f_disp_amp = (final_amp* (1.28));
//				else if(final_amp >= 5.1 && final_amp  <=  6.0) f_disp_amp = (final_amp* (1.20));
//				else if(final_amp >= 6.1 && final_amp  <=  7.0) f_disp_amp = (final_amp* (1.13));
//				
//				else if(final_amp >= 7.1 && final_amp  <=  7.5) f_disp_amp = (final_amp* (1.094));
//				else if(final_amp >= 7.6 && final_amp  <=  8.0) f_disp_amp = (final_amp* (1.09));
//				
//				
//				else if(final_amp >= 8.1 && final_amp  <=  9.5) f_disp_amp = (final_amp* (1.03));
//				
//				else if(final_amp >= 9.5 && final_amp  <= 9.9) f_disp_amp = (final_amp* (1.02));
//				
//				else if(final_amp >= 9.9 && final_amp  <= 10.0) f_disp_amp = (final_amp* (1.00));
//				
//				else if(final_amp >= 10.1 && final_amp <= 10.5) f_disp_amp = (final_amp* (0.99));
//				else if(final_amp >= 10.5 && final_amp <= 11.0) f_disp_amp = (final_amp* (0.97));
//				else if(final_amp >= 11.1 && final_amp <= 11.5) f_disp_amp = (final_amp* (0.96));
//				else if(final_amp >= 11.6 && final_amp <= 12.0) f_disp_amp = (final_amp* (0.96));
//				else if(final_amp >= 12.1 && final_amp <= 12.5) f_disp_amp = (final_amp* (0.96));

//				else if(final_amp >= 12.6 && final_amp <= 13.0) f_disp_amp = (final_amp* (0.95));
//				else if(final_amp >= 13.1 && final_amp <= 13.5) f_disp_amp = (final_amp* (0.94));
//				else if(final_amp >= 13.6 && final_amp <= 14.0) f_disp_amp = (final_amp* (0.94));
//				else if(final_amp >= 14.1 && final_amp <= 14.5) f_disp_amp = (final_amp* (0.94));
//				else if(final_amp >= 14.6 && final_amp <= 15.0) f_disp_amp = (final_amp* (0.93));
//				else if(final_amp >= 15.1 && final_amp <= 15.5) f_disp_amp = (final_amp* (0.93));

*/