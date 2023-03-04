/*****************************************************************************
* | File      	:  	EPD_2in7.c
* | Author      :   Waveshare team
* | Function    :   2.9inch e-paper V2
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-06-03
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_2in7.h"
#include "Debug.h"
#include "EPD_Device.h"
#include "GUI_Paint.h"
#include "math.h"
#include "time.h"
#include <stdlib.h>

static const unsigned char EPD_2in7_lut_vcom_dc[] = {
    0x00	,0x00,
    0x00	,0x08	,0x00	,0x00	,0x00	,0x02,
    0x60	,0x28	,0x28	,0x00	,0x00	,0x01,
    0x00	,0x14	,0x00	,0x00	,0x00	,0x01,
    0x00	,0x12	,0x12	,0x00	,0x00	,0x01,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00
};
static const unsigned char EPD_2in7_lut_ww[] = {
    0x40	,0x08	,0x00	,0x00	,0x00	,0x02,
    0x90	,0x28	,0x28	,0x00	,0x00	,0x01,
    0x40	,0x14	,0x00	,0x00	,0x00	,0x01,
    0xA0	,0x12	,0x12	,0x00	,0x00	,0x01,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
static const unsigned char EPD_2in7_lut_bw[] = {
    0x40	,0x08	,0x00	,0x00	,0x00	,0x02,
    0x90	,0x28	,0x28	,0x00	,0x00	,0x01,
    0x40	,0x14	,0x00	,0x00	,0x00	,0x01,
    0xA0	,0x12	,0x12	,0x00	,0x00	,0x01,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
static const unsigned char EPD_2in7_lut_bb[] = {
    0x80	,0x08	,0x00	,0x00	,0x00	,0x02,
    0x90	,0x28	,0x28	,0x00	,0x00	,0x01,
    0x80	,0x14	,0x00	,0x00	,0x00	,0x01,
    0x50	,0x12	,0x12	,0x00	,0x00	,0x01,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
static const unsigned char EPD_2in7_lut_wb[] = {
    0x80	,0x08	,0x00	,0x00	,0x00	,0x02,
    0x90	,0x28	,0x28	,0x00	,0x00	,0x01,
    0x80	,0x14	,0x00	,0x00	,0x00	,0x01,
    0x50	,0x12	,0x12	,0x00	,0x00	,0x01,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
    0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};

//////////////////////////////////////full screen update LUT////////////////////////////////////////////
//0~3 gray
static const unsigned char EPD_2in7_gray_lut_vcom[] =
{
0x00	,0x00,
0x00	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x60	,0x14	,0x14	,0x00	,0x00	,0x01,
0x00	,0x14	,0x00	,0x00	,0x00	,0x01,
0x00	,0x13	,0x0A	,0x01	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,				
};
//R21
static const unsigned char EPD_2in7_gray_lut_ww[] ={
0x40	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x90	,0x14	,0x14	,0x00	,0x00	,0x01,
0x10	,0x14	,0x0A	,0x00	,0x00	,0x01,
0xA0	,0x13	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
//R22H	r
static const unsigned char EPD_2in7_gray_lut_bw[] ={
0x40	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x90	,0x14	,0x14	,0x00	,0x00	,0x01,
0x00	,0x14	,0x0A	,0x00	,0x00	,0x01,
0x99	,0x0C	,0x01	,0x03	,0x04	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
//R23H	w
static const unsigned char EPD_2in7_gray_lut_wb[] ={
0x40	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x90	,0x14	,0x14	,0x00	,0x00	,0x01,
0x00	,0x14	,0x0A	,0x00	,0x00	,0x01,
0x99	,0x0B	,0x04	,0x04	,0x01	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
//R24H	b
static const unsigned char EPD_2in7_gray_lut_bb[] ={
0x80	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x90	,0x14	,0x14	,0x00	,0x00	,0x01,
0x20	,0x14	,0x0A	,0x00	,0x00	,0x01,
0x50	,0x13	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};

Display::Display()
{

}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
void Display::Reset(void)
{
    epd->Digital_Write(epd->EPD_RST_PIN, 1);
    epd->Delay_ms(200);
    epd->Digital_Write(epd->EPD_RST_PIN, 0);
    epd->Delay_ms(2);
    epd->Digital_Write(epd->EPD_RST_PIN, 1);
    epd->Delay_ms(200);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
void Display::SendCommand(UBYTE Reg)
{
    epd->Digital_Write(epd->EPD_DC_PIN, 0);
    epd->Digital_Write(epd->EPD_CS_PIN, 0);
    epd->SPI_WriteByte(Reg);
    epd->Digital_Write(epd->EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
void Display::SendData(UBYTE Data)
{
    epd->Digital_Write(epd->EPD_DC_PIN, 1);
    epd->Digital_Write(epd->EPD_CS_PIN, 0);
    epd->SPI_WriteByte(Data);
    epd->Digital_Write(epd->EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send all data
parameter:
    Data : Write screen data
******************************************************************************/
void Display::SendBlockData(UBYTE *Data, int block_size)
{
    epd->Digital_Write(epd->EPD_DC_PIN, 1);
//    device->SPI_Write_nByte(Data, row_size);
    epd->SPI_Write_nByte(Data, block_size);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void Display::ReadBusy(void)
{
    // Debug("e-Paper busy\r\n");
    UBYTE busy;
    do {
        SendCommand(0x71);
        busy = epd->Digital_Read(epd->EPD_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(busy);
    epd->Delay_ms(200);
    // Debug("e-Paper busy release\r\n");
}

/******************************************************************************
function :	set the look-up tables
parameter:
******************************************************************************/
void Display::SetLut(void)
{
    unsigned int count;
    SendCommand(0x20); //vcom
    for(count = 0; count < 44; count++) {
        SendData(EPD_2in7_lut_vcom_dc[count]);
    }

    SendCommand(0x21); //ww --
    for(count = 0; count < 42; count++) {
        SendData(EPD_2in7_lut_ww[count]);
    }

    SendCommand(0x22); //bw r
    for(count = 0; count < 42; count++) {
        SendData(EPD_2in7_lut_bw[count]);
    }

    SendCommand(0x23); //wb w
    for(count = 0; count < 42; count++) {
        SendData(EPD_2in7_lut_bb[count]);
    }

    SendCommand(0x24); //bb b
    for(count = 0; count < 42; count++) {
        SendData(EPD_2in7_lut_wb[count]);
    }
}

void Display::gray_SetLut(void)
{
	unsigned int count;	 
    SendCommand(0x20);							//vcom
		for(count=0;count<44;count++)
			{SendData(EPD_2in7_gray_lut_vcom[count]);}
		
	SendCommand(0x21);							//red not use
	for(count=0;count<42;count++)
		{SendData(EPD_2in7_gray_lut_ww[count]);}

		SendCommand(0x22);							//bw r
		for(count=0;count<42;count++)
			{SendData(EPD_2in7_gray_lut_bw[count]);}

		SendCommand(0x23);							//wb w
		for(count=0;count<42;count++)
			{SendData(EPD_2in7_gray_lut_wb[count]);}

		SendCommand(0x24);							//bb b
		for(count=0;count<42;count++)
			{SendData(EPD_2in7_gray_lut_bb[count]);}

		SendCommand(0x25);							//vcom
		for(count=0;count<42;count++)
			{SendData(EPD_2in7_gray_lut_ww[count]);}
         
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void Display::Init(EPD_Device *device)
{
    epd = device;

    // printf("Reset\r\n");
    Reset();

    // printf("Power setting commands\r\n");
    SendCommand(0x01); // POWER_SETTING
    SendData(0x03); // VDS_EN, VDG_EN
    SendData(0x00); // VCOM_HV, VGHL_LV[1], VGHL_LV[0]
    SendData(0x2b); // VDH
    SendData(0x2b); // VDL
    SendData(0x09); // VDHR
	
    // printf("BOOSTER_SOFT_START commands\r\n");
    SendCommand(0x06);  // BOOSTER_SOFT_START
    SendData(0x07);
    SendData(0x07);
    SendData(0x17);
	
    // Power optimization
    SendCommand(0xF8);
    SendData(0x60);
    SendData(0xA5);
	
    // Power optimization
    SendCommand(0xF8);
    SendData(0x89);
    SendData(0xA5);
		
    // Power optimization
    SendCommand(0xF8);
    SendData(0x90);
    SendData(0x00);
		
    // Power optimization
    SendCommand(0xF8);
    SendData(0x93);
    SendData(0x2A);
		
    // Power optimization
    SendCommand(0xF8);
    SendData(0xA0);
    SendData(0xA5);
		
    // Power optimization
    SendCommand(0xF8);
    SendData(0xA1);
    SendData(0x00);
		
    // Power optimization
    SendCommand(0xF8);
    SendData(0x73);
    SendData(0x41);
		
    SendCommand(0x16); // PARTIAL_DISPLAY_REFRESH
    SendData(0x00);
		
    SendCommand(0x04); // POWER_ON
    ReadBusy();

    SendCommand(0x00); // PANEL_SETTING
    SendData(0xAF); // KW-BF   KWR-AF    BWROTP 0f
    SendCommand(0x30); // PLL_CONTROL
    SendData(0x3A); // 3A 100HZ   29 150Hz 39 200HZ    31 171HZ
    SendCommand(0x82);  // VCM_DC_SETTING_REGISTER
    SendData(0x12);

    SetLut();
}

void Display::Init_4Gray(void)
{
    Reset();
    SendCommand(0x01);			//POWER SETTING
    SendData (0x03);
    SendData (0x00);    
    SendData (0x2b);															 
    SendData (0x2b);		


    SendCommand(0x06);         //booster soft start
    SendData (0x07);		//A
    SendData (0x07);		//B
    SendData (0x17);		//C 

    SendCommand(0xF8);         //boost??
    SendData (0x60);
    SendData (0xA5);

    SendCommand(0xF8);         //boost??
    SendData (0x89);
    SendData (0xA5);

    SendCommand(0xF8);         //boost??
    SendData (0x90);
    SendData (0x00);

    SendCommand(0xF8);         //boost??
    SendData (0x93);
    SendData (0x2A);

    SendCommand(0xF8);         //boost??
    SendData (0xa0);
    SendData (0xa5);

    SendCommand(0xF8);         //boost??
    SendData (0xa1);
    SendData (0x00);

    SendCommand(0xF8);         //boost??
    SendData (0x73);
    SendData (0x41);

    SendCommand(0x16);
    SendData(0x00);	

    SendCommand(0x04);
    ReadBusy();

    SendCommand(0x00);			//panel setting
    SendData(0xbf);		//KW-BF   KWR-AF	BWROTP 0f

    SendCommand(0x30);			//PLL setting
    SendData (0x90);      	//100hz 

    SendCommand(0x61);			//resolution setting
    SendData (0x00);		//176
    SendData (0xb0);     	 
    SendData (0x01);		//264
    SendData (0x08);

    SendCommand(0x82);			//vcom_DC setting
    SendData (0x12);

    SendCommand(0X50);			//VCOM AND DATA INTERVAL SETTING			
    SendData(0x97);
}



/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void Display::Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_2IN7_WIDTH % 8 == 0)? (EPD_2IN7_WIDTH / 8 ): (EPD_2IN7_WIDTH / 8 + 1);
    Height = EPD_2IN7_HEIGHT;

    SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            SendData(0XFF);
        }
    }

    SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            SendData(0XFF);
        }
    }
		
    SendCommand(0x12);
    ReadBusy();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void Display::PaintDisplay(const UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_2IN7_WIDTH % 8 == 0)? (EPD_2IN7_WIDTH / 8 ): (EPD_2IN7_WIDTH / 8 + 1);
    Height = EPD_2IN7_HEIGHT;

    SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            SendData(0XFF);
        }
    }

    SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            SendData(Image[i + j * Width]);
        }
    }
    SendCommand(0x12);
    ReadBusy();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
int Display::DisplayAll(const UBYTE *Image)
{
    UWORD Width, Height, block_size;
    UBYTE buffer[(EPD_2IN7_HEIGHT / NUM_BLOCKS) * (EPD_2IN7_WIDTH / 8)];   // half the screen
    Width = (EPD_2IN7_WIDTH % 8 == 0)? (EPD_2IN7_WIDTH / 8 ): (EPD_2IN7_WIDTH / 8 + 1);
    Height = EPD_2IN7_HEIGHT;
    block_size = Width * Height / NUM_BLOCKS;

    // time_t t1, t2;

    // time(&t1);

    this->SendCommand(0x10);
    for (UWORD i = 0; i < block_size; i++) {
        buffer[i] = 0XFF;
    }
    for (UWORD i = 0; i < NUM_BLOCKS; i++) {
        this->SendBlockData(buffer, block_size);
        // device->Delay_ms(10);
    }

    // time(&t2);
    // printf("Time to clear %d\n", t2-t1);
    // time(&t1);

    this->SendCommand(0x13);
    for (UWORD k = 0; k < NUM_BLOCKS; k++) {
        // printf("Block %d\n", k);
        for (UWORD j = 0; j < block_size; j++) {
            buffer[j] = Image[k * block_size + j];
            // printf("%x ", Image[k * block_size + j]);
        }
        this->SendBlockData(buffer, block_size);
        // device->Delay_ms(10);
    }

    // time(&t2);
    // printf("Time to send screen %d\n", t2-t1);
    // time(&t1);

    this->SendCommand(0x12);
    // int result = this->ReadBusy();

    // time(&t2);
    // printf("Time to refresh %d\n", t2-t1);
    // return result;
    return 1;
}

void Display::GrayDisplay(const UBYTE *Image)
{
    UDOUBLE i,j,k;
    UBYTE temp1,temp2,temp3;

    SendCommand(0x10);	       
    for(i=0;i<5808;i++)	               //5808*4  46464
    {
        temp3=0;
        for(j=0;j<2;j++)	
        {
            temp1 = Image[i*2+j];
            for(k=0;k<2;k++)	
            {
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)
                    temp3 |= 0x01;//white
                else if(temp2 == 0x00)
                    temp3 |= 0x00;  //black
                else if(temp2 == 0x80) 
                    temp3 |= 0x01;  //gray1
                else //0x40
                    temp3 |= 0x00; //gray2
                temp3 <<= 1;	
                
                temp1 <<= 2;
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)  //white
                    temp3 |= 0x01;
                else if(temp2 == 0x00) //black
                    temp3 |= 0x00;
                else if(temp2 == 0x80)
                    temp3 |= 0x01; //gray1
                else    //0x40
                        temp3 |= 0x00;	//gray2	
                if(j!=1 || k!=1)				
                    temp3 <<= 1;
                
                temp1 <<= 2;
            }
            
         }
        SendData(temp3);			
    }
    // new  data
    SendCommand(0x13);	       
    for(i=0;i<5808;i++)	               //5808*4  46464
    {
        temp3=0;
        for(j=0;j<2;j++)	
        {
            temp1 = Image[i*2+j];
            for(k=0;k<2;k++)	
            {
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)
                    temp3 |= 0x01;//white
                else if(temp2 == 0x00)
                    temp3 |= 0x00;  //black
                else if(temp2 == 0x80) 
                    temp3 |= 0x00;  //gray1
                else //0x40
                    temp3 |= 0x01; //gray2
                temp3 <<= 1;	
                
                temp1 <<= 2;
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)  //white
                    temp3 |= 0x01;
                else if(temp2 == 0x00) //black
                    temp3 |= 0x00;
                else if(temp2 == 0x80)
                    temp3 |= 0x00; //gray1
                else    //0x40
                        temp3 |= 0x01;	//gray2
                if(j!=1 || k!=1)					
                    temp3 <<= 1;
                
                temp1 <<= 2;
            }
            
         }
        SendData(temp3);	
    }
    
    gray_SetLut();
    SendCommand(0x12);
    epd->Delay_ms(200);
    ReadBusy();
}


/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void Display::Sleep(void)
{
    SendCommand(0X50);
    SendData(0xf7);
    SendCommand(0X02);  	//power off
    SendCommand(0X07);  	//deep sleep
    SendData(0xA5);
}

int Display::DrawDisplay(uint8_t *pageImage, EventStatus *status, const char *deviceName, struct tm now)
{
    char buff[50];
    struct tm event_time;
    float last_sa;

    Paint_NewImage(pageImage, EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
    // Paint_SelectImage(pageImage);
    Paint_Clear(WHITE);

    sprintf(buff, "%.1f", status->MaxEvent);
	if (round(status->MaxEvent) < 10)
    {
		if (status->MaxEvent > 0 and status->MaxEvent < 5)
			Paint_DrawString(30, 0, "< 5", &Font64, BLACK, WHITE);
		else
			Paint_DrawString(30, 0, buff, &Font64, BLACK, WHITE);
    }
	else
		Paint_DrawString(0, 0, buff, &Font64, BLACK, WHITE);
    Paint_DrawString(140, 12, "cm/", &Font20, BLACK, WHITE);
    Paint_DrawString(140, 30, "sec", &Font20, BLACK, WHITE);

    strftime(buff, 20, "at %H:%M", &status->MaxEventDateTime);
    Paint_DrawString_Centered(53, buff, &Font24, BLACK, WHITE);
    strftime(buff, 20, "%Y-%m-%d", &status->MaxEventDateTime);
    Paint_DrawString_Centered(77, buff, &Font20, BLACK, WHITE);

    Paint_DrawString_Centered(108, "Most Recent", &Font20, BLACK, WHITE);

    last_sa = 0;
    if (status->LastEventX > status->LastEventY)
    {
        last_sa = status->LastEventX;
        event_time = status->LastEventXDateTime;
    }
    else
    {
        last_sa = status->LastEventY;
        event_time = status->LastEventYDateTime;
    }
    sprintf(buff, "%.1f", last_sa);
 	if (last_sa < 10)
    {
		if (last_sa > 0 and last_sa < 5)
			Paint_DrawString(30, 130, "< 5", &Font32, BLACK, WHITE);
		else
			Paint_DrawString(30, 130, buff, &Font32, BLACK, WHITE);
    }
	else
		Paint_DrawString(20, 130, buff, &Font32, BLACK, WHITE);

	Paint_DrawString(95, 137, "cm/sec", &Font20, BLACK, WHITE);
    if (last_sa > 0)
    {
        strftime(buff, 20, "at %H:%M:%S", &event_time);
        Paint_DrawString_Centered(158, buff, &Font20, BLACK, WHITE);
        strftime(buff, 20, "%Y-%m-%d", &event_time);
        Paint_DrawString_Centered(178, buff, &Font20, BLACK, WHITE);
    }

	Paint_DrawString_Centered(202, deviceName, &Font20, BLACK, WHITE);

    strftime(buff, 20, "%Y-%m-%d %H:%M", &now);
    Paint_DrawString_Centered(224, buff, &Font20, BLACK, WHITE);

    int ret = 0;
//    EPD_2IN7_Display(pageImage);
    // ret = DisplayAll(pageImage);
    // epd->Display(pageImage);
    PaintDisplay(pageImage);
    // epd->Delay_ms(2000);

    return ret;
}
