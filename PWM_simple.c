///* if define TIMER0_FSYS_DIV12, timer = (0x1FFF-0x1000)*12/24MHz = 4.08ms */

///* if define TIMER0_FSYS, timer = (0x1FFF-0x0010)/24MHz = 340us */

//#define TH0_INIT        0x00 
//#define TL0_INIT        0x10

/* if define TIMER0_FSYS_DIV12, timer = (256-56)*12/246MHz = 100us */
/* if define TIMER0_FSYS, timer = (256-56)/24MHz = 8.33us */



#include "MS51_16K.H"
#include<stdlib.h>

/************************************************************************************************************
*    Main function 
************************************************************************************************************/

#define TH0_INIT        56 
#define TL0_INIT        56


int clearBit(int n, int k);
int setBit(int n, int k);

void MSDelay(unsigned int value); 
void Delay(unsigned int value);

struct
{
    unsigned int a;
		unsigned int b;
	
		unsigned int d;
//		unsigned int e;
	
	//	unsigned long x;
	
    unsigned char c;

} StructData;


unsigned long xdata ADCdata;
unsigned long xdata ADC_stable,ADC_stable_prev,ADC_stable_curent,dif_value;

unsigned long xdata count_gain;

unsigned int counter=0;


unsigned char xdata ADCdataAIN5H, ADCdataAIN5L;

float xdata final_amp;
float xdata f_disp_amp;
float xdata k_watt;



unsigned char i;
unsigned int counter_ms=0;
unsigned int counter_sec=0;
unsigned int counter_min;

unsigned int key=0;

unsigned long watt,temp_watt;
unsigned int unit;
bit unit_update=0;

unsigned int Final_current;

/************************************************************************************************************
*    TIMER 0 interrupt subroutine
************************************************************************************************************/

bit counter_min_update;
	
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
				
					counter_sec++;
					if(counter_sec > 60) //if(counter_sec > 1800) // MIN 30 60*30
					 {
							counter_sec=0;
							counter_min_update=1;
					 }
					
//						watt += 230 * f_disp_amp;
//							if(watt>60000){unit++; watt=0;}
				
							watt += (230 * f_disp_amp);
							
							temp_watt = watt;
					
							k_watt = (230 * f_disp_amp)/1000;  //k_watt = (float)temp_watt/1000;
					
					
						//	 printf("\t\t\n unit=%ld ,watt=%ld",unit,watt); 
						
							// unit watts second
					
					   
					   if(watt>3600000)
								{
									unit++; watt=watt-3600000; unit_update=1;
								}
						 		
							//	printf("\t\t counter_ms=%d ,counter_sec=%d",counter_ms,counter_sec); 
							/******************* calcultion code *********************/
				}		
			
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



void delay1(unsigned int i)
{
 	 unsigned char j;
	 while(i > 0)
	 {
	    for(j=0;j<5;j++);
		i--;
	 }
}


unsigned int a,b;
unsigned long c;

void main(void)
{
	 unsigned char t,j;
	 unsigned long s1,r_phase_c_max_sample,r_phase_c_min_sample;
	
	unsigned long r_phase_current_highest_sample,r_phase_current_lowest_sample;
  
	 unsigned char max_counter=50;
//	 unsigned long temp_disp;
	
 //   ALL_GPIO_QUASI_MODE;
	
		P10_QUASI_MODE;
		P11_QUASI_MODE;
		P12_QUASI_MODE;
		P13_QUASI_MODE;
		P14_QUASI_MODE;
		P15_QUASI_MODE;
		P16_QUASI_MODE;
//		P17_QUASI_MODE;

		P17_INPUT_MODE;

	
	  P06_PUSHPULL_MODE;
	  P07_INPUT_MODE;
	
    P05_PUSHPULL_MODE;
 
	
	
/********************************* end for display *****************************/
	
	  /* UART0 settting for printf function */
    MODIFY_HIRC(HIRC_24);
    UART_Open(24000000,UART0_Timer3,9600);
    ENABLE_UART0_PRINTF;

    P03_QUASI_MODE;
		P01_QUASI_MODE;
		
/******************************** timer 0 *************************/
  
    
    TH0 = TH0_INIT;            //initial counter values 
    TL0 = TL0_INIT;    
   
		TIMER0_FSYS;
    ENABLE_TIMER0_MODE2;
		
		
    ENABLE_TIMER0_INTERRUPT;                       //enable Timer0 interrupt
    ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts
  
    set_TCON_TR0;                                    //Timer0 run
		counter=0;
		P07 = 1;



//								StructData.a=0;
//								Write_DATAFLASH_ARRAY(0x38FE,(unsigned char *)&StructData,sizeof(StructData));//write structure
  
	/*call read byte */
    count_gain = ((read_APROM_BYTE(0x38FE)<<8)+read_APROM_BYTE(0x38FF));
		
		unit = ((read_APROM_BYTE(0x39FE)<<8)+read_APROM_BYTE(0x39FF));

//		watt = ((read_APROM_BYTE(0x42FD)<<16)+(read_APROM_BYTE(0x42FE)<<8)+read_APROM_BYTE(0x42FF));

		//if(watt>3600000) watt = 0;
		
//		printf ("\n system16highsite = 0x%X", system16highsite);

//		printf ("\n count_gain = %ld", count_gain);
//		printf ("\n unit = %ld", unit);

if(unit<0)unit=0;


 key=0;
 ENABLE_ADC_AIN5;
 k_watt=0;
 
 
			a = ((read_APROM_BYTE(0x3800)<<8)+read_APROM_BYTE(0x3801));
			b = ((read_APROM_BYTE(0x3802)<<8)+read_APROM_BYTE(0x3803));

			//printf("\t\t\n write2 watt=%ld ",ADC_stable); 
			//printf("\t\t\n write2 watt=%ld ",count_gain); 
	
			c = b*100000 + a;
			watt = c;
			printf("\t\t\n write3 watt=%ld ",c); 
			
									
									
									
//									a= ADC_stable%10000;
//									b= ADC_stable/10000;
//									
//									printf("\t\t\n write1 watt=%ld ",ADC_stable); 
//									
//									
//									
//									StructData.a=a;
//									Write_DATAFLASH_ARRAY(0x3800,(unsigned char *)&StructData,sizeof(StructData));//write structure
//								
//									StructData.a=b;
//									
//									Write_DATAFLASH_ARRAY(0x3802,(unsigned char *)&StructData,sizeof(StructData));//write structure

//									delay1(100);
//									
//									ADC_stable = ((read_APROM_BYTE(0x3800)<<8)+read_APROM_BYTE(0x3801));

//									printf("\t\t\n write2 watt=%ld ",ADC_stable); 
//									
//									count_gain = ((read_APROM_BYTE(0x3802)<<8)+read_APROM_BYTE(0x3803));
//									
//									printf("\t\t\n write2 watt=%ld ",count_gain); 
//							
//							
//									count_gain = count_gain*10000 + ADC_stable;
//									printf("\t\t\n write3 watt=%ld ",count_gain); 

if(unit==-1)unit=0;
while(1)
		{
	
				r_phase_current_highest_sample = 0;
        r_phase_current_lowest_sample = 0;
			
				 for(t=0;t<max_counter;t++)
					{  
						r_phase_c_max_sample = 100;
						r_phase_c_min_sample = 100;

						for(j=0;j<250;j++)
						 {
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

									s1 = ADCdataAIN5H * 256 + ADCdataAIN5L;
			 
							if(s1>r_phase_c_max_sample)r_phase_c_max_sample = s1;
							
							if(s1<r_phase_c_min_sample)r_phase_c_min_sample = s1;
						
								Delay(5);
						 }
							 Delay(1);
							
							r_phase_current_highest_sample = r_phase_current_highest_sample + r_phase_c_max_sample;
							r_phase_current_lowest_sample  = r_phase_current_lowest_sample  + r_phase_c_min_sample;
					
					}			 
		
		
					r_phase_current_highest_sample = r_phase_current_highest_sample / max_counter;
					r_phase_current_lowest_sample = r_phase_current_lowest_sample / max_counter;
				
					Final_current = (r_phase_current_highest_sample -  r_phase_current_lowest_sample);
					
			
//		temp_disp = r_phase_current_highest_sample;
//				printf ("\n\nr_phase_current_highest_sample = %ld", temp_disp);	
//		temp_disp = r_phase_current_lowest_sample;
//			
//		printf ("\nr_phase_current_lowest_sample = %ld", temp_disp);				 

		
			//printf("\n\r\t ADCdata=%u", ADCdata);	
	
				ADC_stable = Final_current;
		
				//ADC_stable_prev,dif_value
				//ADC_stable_curent
		
			
		
				
				ADC_stable_curent = ADC_stable; 
				if(ADC_stable_curent > ADC_stable_prev)													 
				{
						dif_value = ADC_stable_curent - ADC_stable_prev;
						if(dif_value < 100)
						ADC_stable = ADC_stable_prev;
				}
				else if(ADC_stable_curent < ADC_stable_prev)
				{
					dif_value =  ADC_stable_prev - ADC_stable_curent;
					if(dif_value < 100)
					ADC_stable = ADC_stable_prev;
				} 
				ADC_stable_prev=ADC_stable;
		
	
	
//				temp_disp = ADC_stable;
//				printf ("\n temp_disp = %ld", temp_disp);	
			
				
					
			

//				printf("\n\r\t ADC_stable=%u", ADC_stable); 
//				final_amp = (ADC_stable*0.0108);
//				printf("\n\r\t ADC_cureent_Amp= %0.2f", final_amp); show_data_time_counter=0;
				

					
			
					final_amp = ((float)ADC_stable*(10.00))/count_gain;
			
//				printf("\n\r\t final_amp= %0.2f", final_amp); 

					
		//		printf("\n Actual Amp %0.1f", final_amp); 

					

				//if(final_amp == 0.0 ) f_disp_amp = 0;

				 if(final_amp >= 0.1 && final_amp  <=  0.5) f_disp_amp = (final_amp* (1.0000));
				else if(final_amp >= 0.6 && final_amp  <=  1.0) f_disp_amp = (final_amp* (1.0309));

					
				else if(final_amp >= 1.1 && final_amp  <=  1.5) f_disp_amp = (final_amp* (1.2097));
				else if(final_amp >= 1.6 && final_amp  <=  2.0) f_disp_amp = (final_amp* (1.1765));

				else if(final_amp >= 2.1 && final_amp  <=  2.5) f_disp_amp = (final_amp* (1.2136));
				else if(final_amp >= 2.6 && final_amp  <=  3.0) f_disp_amp = (final_amp* (1.1905));
				
				else if(final_amp >= 3.1 && final_amp  <=  3.5) f_disp_amp = (final_amp* (1.2069));
				else if(final_amp >= 3.6 && final_amp  <=  4.0) f_disp_amp = (final_amp* (1.2195));
				
				else if(final_amp >= 4.1 && final_amp  <=  4.5) f_disp_amp = (final_amp* (1.1688));
				else if(final_amp >= 4.6 && final_amp  <=  5.0) f_disp_amp = (final_amp* (1.1468));
				
				
				else if(final_amp >= 5.1 && final_amp  <=  5.5) f_disp_amp = (final_amp* (1.1270));
				else if(final_amp >= 5.6 && final_amp  <=  6.0) f_disp_amp = (final_amp* (1.1257));
				
				
				else if(final_amp >= 6.1 && final_amp  <=  6.5) f_disp_amp = (final_amp* (1.1149));
				else if(final_amp >= 6.6 && final_amp  <=  7.0) f_disp_amp = (final_amp* (1.0819));
				
				
				else if(final_amp >= 7.1 && final_amp  <=  7.5) f_disp_amp = (final_amp* (1.0838));
				else if(final_amp >= 7.6 && final_amp  <=  8.0) f_disp_amp = (final_amp* (1.0652));
				
				
				else if(final_amp >= 8.1 && final_amp  <=  8.5) f_disp_amp = (final_amp* (1.0612));
				else if(final_amp >= 8.6 && final_amp  <= 9.0) f_disp_amp = (final_amp* (1.0274));
				
				else if(final_amp >= 9.1 && final_amp  <= 9.5) f_disp_amp = (final_amp* (1.0248));
				else if(final_amp >= 9.6 && final_amp  <= 10.0) f_disp_amp = (final_amp* (1.0000));
				
				else if(final_amp >= 10.1 && final_amp <= 10.5) f_disp_amp = (final_amp* (1.0077));
				else if(final_amp >= 10.6 && final_amp <= 11.0) f_disp_amp = (final_amp* (1.0036));
				
				else if(final_amp >= 11.1 && final_amp <= 11.5) f_disp_amp = (final_amp* (0.9991));
				else if(final_amp >= 11.6 && final_amp <= 12.0) f_disp_amp = (final_amp* (0.9961));
				
				else if(final_amp >= 12.1 && final_amp <= 12.5) f_disp_amp = (final_amp* (0.9850));
				else if(final_amp >= 12.6 && final_amp <= 13.0) f_disp_amp = (final_amp* (0.9893));
				
				else if(final_amp >= 13.1 && final_amp <= 13.5) f_disp_amp = (final_amp* (0.9783));
				else if(final_amp >= 13.6 && final_amp <= 14.0) f_disp_amp = (final_amp* (0.9777));
				
				else if(final_amp >= 14.1 && final_amp <= 14.5) f_disp_amp = (final_amp* (0.9693));
				else if(final_amp >= 14.6 && final_amp <= 15.0) f_disp_amp = (final_amp* (0.9721));
				
				
				else if(final_amp >= 15.1 && final_amp <= 15.5) f_disp_amp = (final_amp* (0.9663));
				else if(final_amp >= 15.6 && final_amp <= 16.0) f_disp_amp = (final_amp* (0.9633));


				else if(final_amp >= 16.1 && final_amp <= 16.5) f_disp_amp = (final_amp* (0.9655));
				else if(final_amp >= 16.6 && final_amp <= 17.0) f_disp_amp = (final_amp* (0.9610));
				
				else if(final_amp >= 17.1 && final_amp <= 17.5) f_disp_amp = (final_amp* (0.9631));
				else if(final_amp >= 17.6 && final_amp <= 18.0) f_disp_amp = (final_amp* (0.9585));

				else if(final_amp >= 18.1 && final_amp <= 18.5) f_disp_amp = (final_amp* (0.9590));
				else if(final_amp >= 18.6 && final_amp <= 19.0) f_disp_amp = (final_amp* (0.9601));

				else if(final_amp >= 19.1 && final_amp <= 19.5) f_disp_amp = (final_amp* (0.9452));
				else if(final_amp >= 19.6 && final_amp <= 20.0) f_disp_amp = (final_amp* (0.9421));

				else if(final_amp >= 20.1 && final_amp <= 20.5) f_disp_amp = (final_amp* (0.9352));
				else if(final_amp >= 20.6 && final_amp <= 21.0) f_disp_amp = (final_amp* (0.8750));

				else if(final_amp >= 21.1 && final_amp <= 21.5) f_disp_amp = (final_amp* (0.8661));
				else if(final_amp >= 21.6 && final_amp <= 22.0) f_disp_amp = (final_amp* (0.8661));

				else f_disp_amp = (final_amp* (1));
	
			//  printf("\t\t%005.2f", f_disp_amp);  			

			//	printf("\nDisplay Amp=%0.1f switch=%d,Unit = %d,watt = %ld,k_watt = %f",f_disp_amp,key,unit,watt,k_watt); 
				
			//	printf("\n%d,%d,%ld",key,unit,k_watt); // this command final for kunjan bhai
				
				
				
				//printf("%02d%04d%04d\n",key,unit,k_watt);
										
				printf("\n%ld",temp_watt);
	
			c = temp_watt;	
									b= c%10000;
									a= c/10000;
						
				printf("\n%d%d",a,b); delay1(10);
				
//				printf("\n\r%02d%04d%006.3f",key,unit,k_watt);	//printf("%005.2f",f_disp_amp); 
//				printf("%005.2f",f_disp_amp); 
				
				
									
				
			//	
				
					if(P05==0)
						{	
								  clr_TCON_TR0;                       //Stop Timer0
									count_gain = ADC_stable;
									StructData.a=ADC_stable;
									Write_DATAFLASH_ARRAY(0x38FE,(unsigned char *)&StructData,sizeof(StructData));//write structure
									/***************data write in iap ******************************/				
									//printf("\n\r\t count_gain1= %ld", count_gain); 
									watt=0;
									printf("\n DONE ");	delay1(1000);
 								set_TCON_TR0;                       //start Timer0
							
									while(P05==0);
							
						}	

			
	
				if(unit_update==1)
					{	unit_update=0;
						//  clr_TCON_TR0;                       //Stop Timer0
						StructData.b = unit;
						Write_DATAFLASH_ARRAY(0x39FE,(unsigned char *)&StructData,sizeof(StructData));//write structure
					//   set_TCON_TR0;                       //start Timer0
					}
	
					
					
					if(counter_min_update==1)
					{
						counter_min_update=0;
						//printf("\n\r watt=%ld",temp_watt); 
						//  clr_TCON_TR0;                       //Stop Timer0
									a= temp_watt%100000;
									b= temp_watt/100000;
									
									StructData.a=a;
									Write_DATAFLASH_ARRAY(0x3800,(unsigned char *)&StructData,sizeof(StructData));//write structure
								
									StructData.a=b;
									
									Write_DATAFLASH_ARRAY(0x3802,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(100);
									
//									a = ((read_APROM_BYTE(0x3800)<<8)+read_APROM_BYTE(0x3801));
//									b = ((read_APROM_BYTE(0x3802)<<8)+read_APROM_BYTE(0x3803));

//									//printf("\t\t\n write2 watt=%ld ",ADC_stable); 
//									//printf("\t\t\n write2 watt=%ld ",count_gain); 
//							
//									c = b*100000 + a;
//									printf("\t\t\n write3 watt=%ld ",c); 
					//					set_TCON_TR0;                       //start Timer0
						
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



/* 
// old data
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