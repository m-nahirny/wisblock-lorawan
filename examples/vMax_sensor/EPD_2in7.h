/*****************************************************************************
* | File      	:   EPD_2in7.h
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
#ifndef __EPD_2IN7_H_
#define __EPD_2IN7_H_

// Display resolution
#define EPD_2IN7_WIDTH       176
#define EPD_2IN7_HEIGHT      264 //46464

#define NUM_BLOCKS          1

#ifdef __cplusplus
 extern "C" {
#endif

#include "EPD_Device.h"
#include "CalculateVmaxDef.h"

class Display
{
    public: 
        Display();
        void Init(EPD_Device *device);
        void Clear(void);
        void PaintDisplay(const UBYTE *Image);
        int DisplayAll(const UBYTE *Image);
        void Sleep(void);

        void Init_4Gray(void);
        void GrayDisplay(const UBYTE *Image);

        int DrawDisplay(uint8_t *pageImage, EventStatus *status, const char *deviceName, struct tm now);

    private:
        void Reset(void);
        void SendCommand(UBYTE Reg);
        void SendData(UBYTE Data);
        void SendBlockData(UBYTE *Data, int block_size);
        void ReadBusy(void);
        void SetLut(void);
        void gray_SetLut(void);

        EPD_Device *epd;

};

#ifdef __cplusplus
}
#endif

#endif
