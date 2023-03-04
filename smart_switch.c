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


struct
{
    unsigned int 		a;
    unsigned long int	b;
    unsigned char  	c;

} StructData;



#define TH0_INIT        0x1A 
#define TL0_INIT        0x37

void long_2_bcd(long data1,unsigned char *destination);
void TX_message(unsigned char tx_dp,unsigned char count_digit,bit Enter);

int clearBit(int n, int k);
int setBit(int n, int k);

void MSDelay(unsigned int value); 
void Delay(unsigned int value);




idata  unsigned char count_digit;
unsigned char data_buffer[10];
unsigned char rx_len,rx_length;

extern unsigned char tx_buffer[12],tx_len,tx_length;

unsigned char dp;
unsigned long xdata ADCdata;
unsigned long xdata ADC_stable,ADC_stable_prev,ADC_stable_curent,dif_value;

unsigned long xdata count_gain;
unsigned int xdata unit;
unsigned int counter=0;


unsigned char xdata ADCdataAIN5H, ADCdataAIN5L;

float xdata final_amp;
float xdata f_disp_amp;
int xdata k_watt;


unsigned int key=0;

unsigned long watt,temp_watt;

bit unit_update=0;

unsigned int Final_current;

/************************************************************************************************************
*    TIMER 0 interrupt subroutine
************************************************************************************************************/

bit counter_min_update;
unsigned char value;	
unsigned char sec,min;

void Timer0_ISR (void) interrupt 1           /*interrupt address is 0x000B */
{
    _push_(SFRS);
			TH0 = TH0_INIT;
			TL0 = TL0_INIT;    
			TF0 = 0;

			value++;
			if(value>27)
			{
					value=0;
					sec++;
					if(sec > 59) //if(counter_sec > 1800) // MIN 30 60*30
					 {
							sec=0;
							min++;
							if(min > 15) //if(counter_sec > 1800) // MIN 30 60*30
							 {
								 min=0;
								 counter_min_update=1;
							 }	 
					 
						}
//						watt += 230 * f_disp_amp;
//							if(watt>60000){unit++; watt=0;}
				
							watt += (230 * f_disp_amp);
							
							temp_watt = watt;
					
					
						k_watt = (230 * f_disp_amp);  //k_watt = (230 * f_disp_amp)/1000;
					
					
						//	 printf("\t\t\n unit=%ld ,watt=%ld",unit,watt); 
						
							// unit watts second
					
					   
					   if(watt>3600000)
								{
									unit++; watt=watt-3600000; unit_update=1;
									temp_watt = watt;
								}
						 		
							//	printf("\t\t counter_ms=%d ,counter_sec=%d",counter_ms,counter_sec); 
							/******************* calcultion code *********************/
									
					 
			
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



unsigned int a1,b1;
unsigned long int c1;
unsigned char read_data[5];

void main(void)
{
				unsigned char t,j;
				unsigned long s1,r_phase_c_max_sample,r_phase_c_min_sample;

				unsigned long r_phase_current_highest_sample,r_phase_current_lowest_sample;

				unsigned char max_counter=50;

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

				TIMER0_FSYS_DIV12;
				ENABLE_TIMER0_MODE1;
				
				
				ENABLE_TIMER0_INTERRUPT;                       //enable Timer0 interrupt
				ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts

				set_TCON_TR0;                                    //Timer0 run
				counter=0;
				P07 = 1;





				 key=0;
				 ENABLE_ADC_AIN5;
				 k_watt=0;
				 watt = 0;


				count_gain = ((read_APROM_BYTE(0x3800)<<8)+read_APROM_BYTE(0x3801));
				c1 = count_gain;
				printf("\t\t\n count_gain =%ld ",count_gain); 

				unit = ((read_APROM_BYTE(0x3810)<<8)+read_APROM_BYTE(0x3811));
				
				printf("\t\t\n unit =%u ",unit); 

				if(unit<0)unit=0;
				
				Read_DATAFLASH_ARRAY(0x3900,(unsigned char *)&read_data,sizeof(StructData));
				temp_watt = ((read_data[0]*0x1000000)+(read_data[1]*0x10000) + (read_data[2]*0x100) + (read_data[3])) ;
				watt =temp_watt;
					
				ENABLE_UART0_PRINTF;
				printf("\n temp_watt = %lld", temp_watt); delay1(1000);
				DISABLE_UART0_PRINTF; 
		
while(1)
		{

				r_phase_current_highest_sample = 0;
        r_phase_current_lowest_sample = 0;
			
				 for(t=0;t<max_counter;t++)
					{  
						r_phase_c_max_sample = 50;
						r_phase_c_min_sample = 50;

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
						
					
						ADC_stable = Final_current;
						
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
				
	
	
					final_amp = ((float)ADC_stable*(10.00))/count_gain;
			
					//printf("\n\r\t final_amp= %0.2f", final_amp); 

				//if(final_amp == 0.0 ) f_disp_amp = 0;

				 if(final_amp >= 0.0 && final_amp  <=  0.5) f_disp_amp = (final_amp* (1.0000));
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
	
				 if(final_amp <= 0.2) f_disp_amp = 0;


									//  printf("\t\t%005.2f", f_disp_amp);  			
									//	printf("\nDisplay Amp=%0.1f switch=%d,Unit = %d,watt = %ld,k_watt = %f",f_disp_amp,key,unit,watt,k_watt); 
									//	printf("\n%d,%d,%ld",key,unit,k_watt); // this command final for kunjan bhai
										
									//printf("%02d%04d%04d\n",key,unit,k_watt);
									//printf("\n\r%02d%04d%006.3f",key,unit,k_watt);	printf("%005.2f",f_disp_amp); 
		
		
				
				
			//	
				
					if(P05==0)
						{	
								  clr_TCON_TR0;                       //Stop Timer0
								  count_gain = ADC_stable;
									StructData.a=count_gain;
									Write_DATAFLASH_ARRAY(0x3800,(unsigned char *)&StructData,sizeof(StructData));//write structure
							
									unit= 0;
									StructData.a = unit;
									Write_DATAFLASH_ARRAY(0x3810,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(1000);
							
									ENABLE_UART0_PRINTF;
									printf("\n DONE, Unit reset 0 ");	delay1(1000);
									DISABLE_UART0_PRINTF; 
									set_TCON_TR0;                       //start Timer0
							watt = 0;
									while(P05==0);
									goto SET_WATT_0;
								
						}	

			
	
				if(unit_update==1)
					{		unit_update=0;
						  clr_TCON_TR0;                       //Stop Timer0
							StructData.a = unit;
							Write_DATAFLASH_ARRAY(0x3810,(unsigned char *)&StructData,sizeof(StructData));//write structure
							delay1(1000);
							set_TCON_TR0;                       //start Timer0
					}
	
					
					
					if(counter_min_update==1)
					{
									SET_WATT_0:
									counter_min_update=0;

									clr_TCON_TR0;    //Stop Timer0
																	
									b1= temp_watt%0x10000;
									a1= temp_watt/0x10000;
								
									StructData.a=a1;
									Write_DATAFLASH_ARRAY(0x3900,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(1000);
									StructData.a=b1;
									Write_DATAFLASH_ARRAY(0x3902,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(1000);

						
//									Read_DATAFLASH_ARRAY(0x3800,(unsigned char *)&read_data,sizeof(StructData));
//									
//									c1 = ((read_data[0]*0x1000000)+(read_data[1]*0x10000) + (read_data[2]*0x100) + (read_data[3])) ;


//									ENABLE_UART0_PRINTF;
//									printf("\n c1 = %lld", c1); delay1(1000);
//									DISABLE_UART0_PRINTF; 
									set_TCON_TR0;                       //start Timer0
					}
					
					
					
					ENABLE_UART0_PRINTF;
					printf("%02d,%04u",key,unit);	
					printf(",%005.2f",f_disp_amp); 
					printf(",%07ld\n\r",temp_watt);				
					DISABLE_UART0_PRINTF;
											
					
					
					
		} // end while 	
		

} //end main










			
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


/************************************** data wirte tesing purpose ************************************/		
									//count_gain = ADC_stable;
									
	/*								
									
									StructData.a=874;
									Write_DATAFLASH_ARRAY(0x3700,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(1000);
									
									count_gain = ((read_APROM_BYTE(0x3700)<<8) + read_APROM_BYTE(0x3701));
									printf("\n count_gain = %ld", count_gain); delay1(1000);
									
									
									StructData.a=12698;
									Write_DATAFLASH_ARRAY(0x3702,(unsigned char *)&StructData,sizeof(StructData));//write structure
									
									
									unit = ((read_APROM_BYTE(0x3702)<<8) + read_APROM_BYTE(0x3703));
									printf("\n count_gain1 = %u", unit); delay1(1000);
										
									
									Read_DATAFLASH_ARRAY(0x3700,read_data,4);
									//c1 = ((read_data[0]<<32) + (read_APROM_BYTE(0x3701)<<16) + (read_APROM_BYTE(0x3702)<<8) + read_APROM_BYTE(0x3703));									
									
									//c1 = ((read_APROM_BYTE(0x3700)<<32) + (read_APROM_BYTE(0x3701)<<16) + (read_APROM_BYTE(0x3702)<<8) + read_APROM_BYTE(0x3703));									

										c1 = 0x12345678;
									b1= c1%0x10000;
									a1= c1/0x10000;
									
									StructData.a=a1;
									Write_DATAFLASH_ARRAY(0x3700,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(1000);
									StructData.a=b1;
									Write_DATAFLASH_ARRAY(0x3702,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(1000);
									
									Read_DATAFLASH_ARRAY(0x3700,(unsigned char *)&read_data,sizeof(StructData));
									
									printf("\n 0x3700 read_data0 = %X", read_data[0]); delay1(1000);
									printf("\n 0x3700 read_data1 = %X", read_data[1]); delay1(1000);
									printf("\n 0x3700 read_data2 = %X", read_data[2]); delay1(1000);
									printf("\n 0x3700 read_data3 = %X", read_data[3]); delay1(1000);
									
									
									
									a1 = ((read_APROM_BYTE(0x3700)<<8)+read_APROM_BYTE(0x3701));
									b1 = ((read_APROM_BYTE(0x3702)<<8)+read_APROM_BYTE(0x3703));
									
									
									printf("\n a1 = %X", a1); delay1(1000);//printf("\n a1 = %u", a1); delay1(1000);
									printf("\n b1 = %X", b1); delay1(1000);//printf("\n b1 = %u", b1); delay1(1000);
									
									
									c1 = a1*0x10000 + b1;
									printf("\n c1 = %X", c1); delay1(1000);
									
									
									
									
									
									// for long variable data store
									c1=123456789;
									
									b1= c1%0x10000;
									a1= c1/0x10000;
									printf("\n c1 = %X", c1); delay1(1000);
									StructData.a=a1;
									Write_DATAFLASH_ARRAY(0x3800,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(1000);
									StructData.a=b1;
									Write_DATAFLASH_ARRAY(0x3802,(unsigned char *)&StructData,sizeof(StructData));//write structure
									delay1(1000);
	
											
									Read_DATAFLASH_ARRAY(0x3800,(unsigned char *)&read_data,sizeof(StructData));
									
									printf("\n read_data = %X", read_data[0]); delay1(1000);
									printf("\n read_data = %X", read_data[1]); delay1(1000);
									printf("\n read_data = %X", read_data[2]); delay1(1000);
									printf("\n read_data = %X", read_data[3]); delay1(1000);

									c1 = ((read_data[0]*0x1000000)+(read_data[1]*0x10000) + (read_data[2]*0x100) + (read_data[3])) ;
									printf("\n c1 = %lld", c1); delay1(1000);
									
									// for long variable data store  ENd	

										
//									StructData.a = 1345;
//									Write_DATAFLASH_ARRAY(0x3780,(unsigned char *)&StructData,sizeof(StructData));//write structure
//									unit = ((read_APROM_BYTE(0x3780)<<8)+read_APROM_BYTE(0x3781));
//									c1=unit;
//									printf("\n unit = %ld ", c1); delay1(1000);
//						
//	
//									StructData.b=12345;
//									Write_DATAFLASH_ARRAY(0x3900,(unsigned char *)&StructData,sizeof(StructData));//write structure
//									delay1(100);
									
								//	c1 = ((read_APROM_BYTE(0x3700)<<32)+(read_APROM_BYTE(0x3701)<<16)+(read_APROM_BYTE(0x3702)<<8)+read_APROM_BYTE(0x3703));									

*/
/***************************************** data wirte tesing purpose  *********************************/
