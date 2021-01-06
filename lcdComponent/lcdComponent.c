 /* Copyright (c) 2018 AKSHAY KULKARNI, Bangalore
  All rights reserved.
  Confidential and Proprietary - AKSHAY KULKARNI , Bangalore
  Authors : Akshay Kulkarni


	 =========================================================
	 ||						     	 ||
	 ||		 ___     ____	 _   _	            	 ||
	 ||		|   | 	| 	 |  /		     	 ||
	 ||		|   |   |	 | /    	         ||
	 ||		|___|   |___	 |/	  	     	 ||
	 ||		|   |       |	 |\      	     	 ||
	 ||		|   |       |	 | \		     	 ||
	 ||		|   | O	____| O  |  \		   	 ||
	 ||					 	         ||
  	 =========================================================
*/


#include "legato.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "ILI9341.h"
#include "f1.h"
//#include "logo.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>



#define BUFSIZE 1000



void push_color (int opt,uint16_t color,int left,int top,int right,int bottom);
void fill_color(uint16_t color);
void GLCD_DrawString(char *string, uint16_t x, uint16_t y, uint16_t fgColour, uint16_t bgColour);
void GLCD_DrawChar(unsigned char character, uint16_t x, uint16_t y, uint16_t fgColour, uint16_t bgColour);
unsigned char reverse(unsigned char v);
//int Getwords(const char *string,char buffer[][500]);
//static int out,top_out=0;
//===========================GLOBAL DECLARATION=================================

//char uart_dev1[]="/dev/smd10";
	int fd;
	int result=0;
	int ret_val;
	fd_set fdset;

//============================FILL_COLOR_SLOW_MOTION ==================================





	void fill_color(uint16_t color)
	{
	    ili_set_window(0,0,ILI9341_TFTWIDTH-1,ILI9341_TFTHEIGHT-1);
	int i,j;
		for (i= 0; i < ILI9341_TFTHEIGHT; i++)
	{
	for(j=0;j<ILI9341_TFTWIDTH;j++)

		{
		ili_send16bit(DATA,color);


		}
	}
	}
//=======================================================================================

	/*##################################### OPTIMIZED PUSH COLOR ##########################################*/

	void push_color (int opt,uint16_t color,int left,int top,int right,int bottom)
	{

		LE_INFO("ENTERING PUSH_COLOR FUNCTION\n ");
		uint16_t y =  ((color & 0xff) << 8) | ((color & 0xff00) >> 8); // FUNCTION TO SWAP BYTES;
		int i,limit;
	if(opt==1)
	{
		limit = ((right-left)*320);
	}
	else
	{

		limit = ((right-left+1)*320);
	}
		uint16_t array[limit];
		for (i=0;i<limit;i++)
		{
			array[i]=y;
		}
		ili_set_window(left,top,right,bottom); // SET THE AREA
		ili_send(DATA,(const unsigned char *)array,sizeof(array));
		LE_INFO("EXITING PUSH_COLOR FUNCTION\n ");
	}

	//==================================================================================================

	//################################################  CHAR REVERSE   #################################################

	unsigned char reverse(unsigned char v)
	{
		unsigned char r=v;
		int s=sizeof(unsigned char)*7;
		for(v>>=1;v;v>>=1)
		{
			r=r<<1;
			r|=v&1;
			s--;
		}
		return r<<s;

	}


	//######################################## DRAW CHAR FOR NORMAL ########################################

	void GLCD_DrawChar(unsigned char character, uint16_t x, uint16_t y, uint16_t fgColour, uint16_t bgColour) {
		uint8_t row=1,i=0;
		unsigned char ele[24];
		uint16_t fgColour1;
		uint16_t bgColour1;
		uint16_t array1[35000];
		int n=1,il=0,counter;
		//ili_set_window(0,0,x+5,0x7f);
		ili_set_window(x,y,x+15,y+11);
		fgColour1 =  ((fgColour & 0xff) << 8) | ((fgColour & 0xff00) >> 8);
		bgColour1 =  ((bgColour & 0xff) << 8) | ((bgColour & 0xff00) >> 8);
		//	ili_set_window(224,320-12,239,319);
		// Plot the font data
		for(i=0;i<24;i++)
		{
			ele[i]=reverse(font12x16[character][i]);
		}
			for(counter = 1; counter <= 24; counter++) {
				if(counter%2==1)
				{
					n=counter;


					for(row=0;row<8;row++)
					{
						if (ele[n] & (1<<row)){
							array1[il++]=fgColour1;//0X00F8;


						}
						else{
							array1[il++]=bgColour1; //0XFFFF;

						}
					}
				}
			else
				{
					n=counter-2;
					for(row=0;row<8;row++)
					{
						if (ele[(n)] & (1<<row)){
							array1[il++]=fgColour1;
							//ili_send(DATA,(const unsigned char *)array3,sizeof(array3));
						}
						else{
							array1[il++]=bgColour1;
							//ili_send(DATA,(const unsigned char *)array4,sizeof(array4));

						}
					}



				}

			}
		//}
	ili_send(DATA,(const unsigned char *)array1,384);
	}


	//######################################## DRAW STRING FOR NORMAL ########################################


	void GLCD_DrawString(char *string, uint16_t x, uint16_t y, uint16_t fgColour, uint16_t bgColour)
	{

		uint8_t origin = y;
		uint8_t characterNumber;
		for (characterNumber=0; characterNumber < strlen(string); characterNumber++)
		{

			// Check if x out of bounds and move to

			// the next line



			   if (y>=312 || string[characterNumber]=='\n')
			   {

				y = origin;

			   x -= 16;

			   }

	if(string[characterNumber]=='\r')
	{
				string[characterNumber]=' ';
				//characterNumber++;

	}



			// If y reaches bottom of the screen just exit

				if (x > 240) break;



			// Plot the current character

			GLCD_DrawChar((string[characterNumber]), x, y, fgColour, bgColour);

			y += 12;

		}


	}

	void start()
	{

		LE_INFO("ENTERING START FUNCTION \n");
	       data_command_select_fd = init_output_gpio (8); // Data_Command_select // IOT0_GPIO4
		   reset_fd = init_output_gpio (7); // reset OPTINAL CAN BE HANDLED IN THE HARDWARE SO MUTED {GNDED} // IOT0_GPIO3
		    LE_INFO("EXPORTED GPIO \n");
		    display_fd = init_spidev(1,0); // select the SPI
		    LE_INFO("INITIATING SPIDEV IN START FUNCTION \n");
		    int ind=0;

	//	ili_reset(); // NOT WORKING
		soft_reset();

		ili_init ();
		//printf("filling screen\n");
	//	fill_color(0XF800); //red
		LE_INFO("FILLING SCREEN\n");
		push_color(1,0xFFFF,0,0,240,320);//clear screen

	while (ind < 10 )

	{
	ind++;
		push_color(1,0xF800,0,0,240,320);//clear screen
		sleep(2);
		GLCD_DrawString("SYRATRON TECHNOLOGIES",160,40,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("PRESENTS",140,120,0xFFFF,0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("SIERRA WIRELESS",100,80, 0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("WP7608",80,120, 0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
	sleep (3);
		push_color(1,0xF800,80,40,200,300);//clear screen
		GLCD_DrawString("SIERRA WIRELESS",200,80,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("PRESENTS",180,120,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("LEGATO",140,130,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("An Open Source",100,80,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("Embedded Platform",80,60,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("Designed to simplify",60,45,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("IoT Application",40,80,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA
		GLCD_DrawString("Development",20,100,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // SIERRA

		sleep(3);
		push_color(1,0xFFFF,0,0,240,320);//clear screen
		GLCD_DrawString("STARTING COLOR TEST ON",140,40,0x0000, 0xFFFF); // CHAR->LEFT->TOP->FGND->BGND // COLOR TESTING
		GLCD_DrawString("ILI9341 Based TFT LCD",100,40,0x0000, 0xFFFF); // CHAR->LEFT->TOP->FGND->BGND // COLOR TESTING
		sleep(3);
		push_color(1,0xF800,0,0,240,320);//RED
		GLCD_DrawString("RED",140,130,0xFFFF, 0xF800); // CHAR->LEFT->TOP->FGND->BGND // RED
		sleep(2);
		push_color(1,0x07E0,0,0,240,320);//PURE GREEN
		//GLCD_DrawString("PURE GREEN",140,130,0x0000, 0x07E0); // CHAR->LEFT->TOP->FGND->BGND // PURE GREEN
		sleep(2);
		push_color(1,0x001F,0,0,240,320);//PURE BLUE
		GLCD_DrawString("PURE BLUE",140,130,0xFFFF, 0x001F); // CHAR->LEFT->TOP->FGND->BGND // PURE BLUE
		sleep(2);
		push_color(1,0x0000,0,0,240,320);//BLACK
		GLCD_DrawString("BLACK",140,130,0xFFFF, 0x0000); // CHAR->LEFT->TOP->FGND->BGND // BLACK
		sleep(2);
		push_color(1,0xFFFF,0,0,240,320);//WHITE
		GLCD_DrawString("WHITE",140,130,0x0000, 0xFFFF); // CHAR->LEFT->TOP->FGND->BGND // WHITE
		sleep(2);
		push_color(1,0xFFE0,0,0,240,320);//YELLOW
		GLCD_DrawString("YELLOW",140,130,0x0000, 0xFFE0); // CHAR->LEFT->TOP->FGND->BGND // YELLOW
		sleep(2);
		push_color(1,0xF81F,0,0,240,320);//MAGENTA
		GLCD_DrawString("MAGENTA",140,130,0x0000,0xF81F); // CHAR->LEFT->TOP->FGND->BGND // MAGENTA
		sleep(2);
		push_color(1,0x07FF,0,0,240,320);//CYAN
		GLCD_DrawString("CYAN",140,130,0x0000, 0x07FF); // CHAR->LEFT->TOP->FGND->BGND // CYAN
		sleep(2);


//
	}
	}





















COMPONENT_INIT
{
	LE_INFO("STARTING ILI9341 BASED TFT LCD");
	start();
}








