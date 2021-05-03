/*
 * ili9341.c
 *
 *  Created on: 9 f�vr. 2021
 *      Author: Nirgal
 */


#include "sdk_common.h"
#include "appli/config.h"

#if USE_ILI9341

#include "nrf_lcd.h"
#include "bsp/nrf52_spi.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "appli/common/gpio.h"
#include "appli/common/systick.h"
#include "ili9341.h"
#include "stdarg.h"
#include "../appli/common/gpio.h"



#define ILI9341_delay	SYSTICK_delay_ms

#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC					0x36
#define ILI9341_PIXEL_FORMAT		0x3A
#define ILI9341_WDB					0x51
#define ILI9341_WCD					0x53
#define ILI9341_RGB_INTERFACE		0xB0
#define ILI9341_FRC					0xB1
#define ILI9341_BPC					0xB5
#define ILI9341_DFC					0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC					0xF7

typedef enum{				// Division ration for internal clocks
	LCD_FRC_DIVA_1	= 0x00,	// Fosc		(default)
	LCD_FRC_DIVA_2	= 0x01,	// Fosc/2
	LCD_FRC_DIVA_4	= 0x02,	// Fosc/4
	LCD_FRC_DIVA_8	= 0x03	// Fosc/8
}FRC_param1_DIVA;


typedef enum{					// Frame Rate
	LCD_FRC_RTNA_61HZ	= 0x1F,	// 61 Hz
	LCD_FRC_RTNA_63HZ	= 0x1E,	// 63 Hz
	LCD_FRC_RTNA_65HZ	= 0x1D,	// 65 Hz
	LCD_FRC_RTNA_68HZ	= 0x1C,	// 68 Hz
	LCD_FRC_RTNA_70HZ	= 0x1B,	// 70 Hz	(default)
	LCD_FRC_RTNA_73HZ	= 0x1A,	// 73 Hz
	LCD_FRC_RTNA_76HZ	= 0x19,	// 76 Hz
	LCD_FRC_RTNA_79HZ	= 0x18,	// 79 Hz
	LCD_FRC_RTNA_83HZ	= 0x17,	// 83 Hz
	LCD_FRC_RTNA_86HZ	= 0x16,	// 86 Hz
	LCD_FRC_RTNA_90HZ	= 0x15,	// 90 Hz
	LCD_FRC_RTNA_95HZ	= 0x14,	// 95 Hz
	LCD_FRC_RTNA_100HZ	= 0x13,	// 100 Hz
	LCD_FRC_RTNA_106HZ	= 0x12,	// 106 Hz
	LCD_FRC_RTNA_112HZ	= 0x11,	// 112 Hz
	LCD_FRC_RTNA_119HZ	= 0x10	// 119 Hz
}FRC_param2_RTNA;

// Set of commands described in ILI9341 datasheet.
#define ILI9341_NOP         0x00
#define ILI9341_SWRESET     0x01
#define ILI9341_RDDID       0x04
#define ILI9341_RDDST       0x09

#define ILI9341_SLPIN       0x10
#define ILI9341_SLPOUT      0x11
#define ILI9341_PTLON       0x12
#define ILI9341_NORON       0x13

#define ILI9341_RDMODE      0x0A
#define ILI9341_RDMADCTL    0x0B
#define ILI9341_RDPIXFMT    0x0C
#define ILI9341_RDIMGFMT    0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF      0x20
#define ILI9341_INVON       0x21
#define ILI9341_GAMMASET    0x26
#define ILI9341_DISPOFF     0x28
#define ILI9341_DISPON      0x29

#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_RAMRD       0x2E

#define ILI9341_PTLAR       0x30
#define ILI9341_MADCTL      0x36
#define ILI9341_PIXFMT      0x3A

#define ILI9341_FRMCTR1     0xB1
#define ILI9341_FRMCTR2     0xB2
#define ILI9341_FRMCTR3     0xB3
#define ILI9341_INVCTR      0xB4
#define ILI9341_DFUNCTR     0xB6

#define ILI9341_PWCTR1      0xC0
#define ILI9341_PWCTR2      0xC1
#define ILI9341_PWCTR3      0xC2
#define ILI9341_PWCTR4      0xC3
#define ILI9341_PWCTR5      0xC4
#define ILI9341_VMCTR1      0xC5
#define ILI9341_VMCTR2      0xC7
#define ILI9341_PWCTRSEQ    0xCB
#define ILI9341_PWCTRA      0xCD
#define ILI9341_PWCTRB      0xCF

#define ILI9341_RDID1       0xDA
#define ILI9341_RDID2       0xDB
#define ILI9341_RDID3       0xDC
#define ILI9341_RDID4       0xDD

#define ILI9341_GMCTRP1     0xE0
#define ILI9341_GMCTRN1     0xE1
#define ILI9341_DGMCTR1     0xE2
#define ILI9341_DGMCTR2     0xE3
#define ILI9341_TIMCTRA     0xE8
#define ILI9341_TIMCTRB     0xEA

#define ILI9341_ENGMCTR     0xF2
#define ILI9341_INCTR       0xF6
#define ILI9341_PUMP        0xF7

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

#define ILI9341_SS_RESET()			GPIO_write(ILI9341_SS_PIN, 0)
#define ILI9341_SS_SET()			GPIO_write(ILI9341_SS_PIN, 1)
#define ILI9341_DC_SET()			GPIO_write(ILI9341_DC_PIN, 1)
#define ILI9341_DC_RESET()			GPIO_write(ILI9341_DC_PIN, 0)

uint16_t ILI9341_x;
uint16_t ILI9341_y;
ILI931_Options_t ILI9341_Opts;
uint8_t ILI9341_INT_CalledFromPuts = 0;

static inline void write_command(uint8_t c)
{
    ILI9341_DC_RESET();
    ILI9341_SS_RESET();
    SPI_write(&c, sizeof(c));
    ILI9341_SS_SET();
}

static inline void write_data(uint8_t c)
{

    ILI9341_DC_SET();
    ILI9341_SS_RESET();
    SPI_write(&c, sizeof(c));
    ILI9341_SS_SET();
}

static void set_addr_window(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1)
{
    ASSERT(x_0 <= x_1);
    ASSERT(y_0 <= y_1);

    write_command(ILI9341_CASET);
    write_data(x_0 >> 8);
    write_data(x_0);
    write_data(x_1 >> 8);
    write_data(x_1);
    write_command(ILI9341_PASET);
    write_data(y_0 >> 8);
    write_data(y_0);
    write_data(y_1 >> 8);
    write_data(y_1);
    write_command(ILI9341_RAMWR);
}

static void command_list(void)
{
	/*	//command list from driver NRF
    write_command(ILI9341_SWRESET);
    nrf_delay_ms(120);
    write_command(ILI9341_DISPOFF);
    nrf_delay_ms(120);
    write_command(ILI9341_PWCTRB);
    write_data(0x00);
    write_data(0XC1);
    write_data(0X30);

    write_command(ILI9341_TIMCTRA);
    write_data(0x85);
    write_data(0x00);
    write_data(0x78);

    write_command(ILI9341_PWCTRSEQ);
    write_data(0x39);
    write_data(0x2C);
    write_data(0x00);
    write_data(0x34);
    write_data(0x02);

    write_command(ILI9341_PUMP);
    write_data(0x20);

    write_command(ILI9341_TIMCTRB);
    write_data(0x00);
    write_data(0x00);

    write_command(ILI9341_PWCTR1);
    write_data(0x23);

    write_command(ILI9341_PWCTR2);
    write_data(0x10);

    write_command(ILI9341_VMCTR1);
    write_data(0x3e);
    write_data(0x28);

    write_command(ILI9341_VMCTR2);
    write_data(0x86);

    write_command(ILI9341_MADCTL);
    write_data(0x48);

    write_command(ILI9341_PIXFMT);
    write_data(0x55);

    write_command(ILI9341_FRMCTR1);
    write_data(0x00);
    write_data(0x18);

    write_command(ILI9341_DFUNCTR);
    write_data(0x08);
    write_data(0x82);
    write_data(0x27);

    write_command(ILI9341_ENGMCTR);
    write_data(0x00);

    write_command(ILI9341_GAMMASET);
    write_data(0x01);

    write_command(ILI9341_GMCTRP1);
    write_data(0x0F);
    write_data(0x31);
    write_data(0x2B);
    write_data(0x0C);
    write_data(0x0E);
    write_data(0x08);
    write_data(0x4E);
    write_data(0xF1);
    write_data(0x37);
    write_data(0x07);
    write_data(0x10);
    write_data(0x03);
    write_data(0x0E);
    write_data(0x09);
    write_data(0x00);

    write_command(ILI9341_GMCTRN1);
    write_data(0x00);
    write_data(0x0E);
    write_data(0x14);
    write_data(0x03);
    write_data(0x11);
    write_data(0x07);
    write_data(0x31);
    write_data(0xC1);
    write_data(0x48);
    write_data(0x08);
    write_data(0x0F);
    write_data(0x0C);
    write_data(0x31);
    write_data(0x36);
    write_data(0x0F);

    write_command(ILI9341_SLPOUT);
    nrf_delay_ms(120);
    write_command(ILI9341_DISPON);

*/




// Command list from driver F103

    write_command(ILI9341_SLEEP_OUT);
    	ILI9341_delay(7);

    	// Power control A
    	write_command(ILI9341_POWERA);
    	write_data(0x39);
    	write_data(0x2C);
    	write_data(0x00);
    	write_data(0x34);
    	write_data(0x02);

    	//ILI9341_sendMultipleData((Uint8 []){0x39, 0x2C, 0x00, 0x34, 0x02}, 5);

    	// Power control B
    	write_command(ILI9341_POWERB);
    	write_data(0x00);
    	write_data(0xC1);
    	write_data(0x30);

    	//ILI9341_sendMultipleData((Uint8 []){0x00, 0xC1, 0x30}, 3);

    	// Driver timing control A
    	write_command(ILI9341_DTCA);
    	write_data(0x85);
    	write_data(0x00);
    	write_data(0x78);

    	//ILI9341_sendMultipleData((Uint8 []){0x85, 0x00, 0x78}, 3);


    	// Driver timing control B
    	write_command(ILI9341_DTCB);
    	write_data(0x00);
    	write_data(0x00);

    	//ILI9341_sendMultipleData((Uint8 []){0x00, 0x00}, 2);

    	// Power on sequence control
    	write_command(ILI9341_POWER_SEQ);
    	write_data(0x64);
    	write_data(0x03);
    	write_data(0x12);
    	write_data(0x81);

    	//ILI9341_sendMultipleData((Uint8 []){0x64, 0x03, 0x12, 0x81}, 4);

    	// Pump ratio control
    	write_command(ILI9341_PRC);
    	write_data(0x20);

    	// Power control 1
    	write_command(ILI9341_POWER1);
    	write_data(0x03);

    	// Power control 2
    	write_command(ILI9341_POWER2);
    	write_data(0x10);

    	// VCOM control 1
    	write_command(ILI9341_VCOM1);
    	write_data(0x3E);
    	write_data(0x28);

    	//ILI9341_sendMultipleData((Uint8 []){0x3E, 0x28}, 2);

    	// VCOM control 2
    	write_command(ILI9341_VCOM2);
    	write_data(0x86);

    	// Memory access control
    	write_command(ILI9341_MAC);
    	write_data(0x48);

    	// Pixel format set
    	write_command(ILI9341_PIXEL_FORMAT);
    	write_data(0x55);

    	// Frame rate control
    	write_command(ILI9341_FRC);
    	write_data(LCD_FRC_DIVA_1);
    	write_data(LCD_FRC_RTNA_119HZ);

    	//ILI9341_sendMultipleData((Uint8 []){LCD_FRC_DIVA_1, LCD_FRC_RTNA_119HZ}, 2);

    	// Display function control
    	write_command(ILI9341_DFC);
    	write_data(0x08);
    	write_data(0x82);
    	write_data(0x27);

    	//ILI9341_sendMultipleData((Uint8 []){0x08, 0x82, 0x27}, 3);

    	// 3Gamma function disable
    	write_command(ILI9341_3GAMMA_EN);
    	write_data(0x00);

    	// Gamma curve selected
    	write_command(ILI9341_GAMMA);
    	write_data(0x01);

    	// Set positive gamma
    	write_command(ILI9341_PGAMMA);
    	write_data(0x0F);
    	write_data(0x31);
    	write_data(0x2B);
    	write_data(0x0C);
    	write_data(0x0E);
    	write_data(0x08);
    	write_data(0x4E);
    	write_data(0xF1);
    	write_data(0x37);
    	write_data(0x07);
    	write_data(0x10);
    	write_data(0x03);
    	write_data(0x0E);
    	write_data(0x09);
    	write_data(0x00);

    	//ILI9341_sendMultipleData((Uint8 []){0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00}, 15);

    	// Set negative gama
    	write_command(ILI9341_NGAMMA);
    	write_data(0x00);
    	write_data(0x0E);
    	write_data(0x14);
    	write_data(0x03);
    	write_data(0x11);
    	write_data(0x07);
    	write_data(0x31);
    	write_data(0xC1);
    	write_data(0x48);
    	write_data(0x08);
    	write_data(0x0F);
    	write_data(0x0C);
    	write_data(0x31);
    	write_data(0x36);
    	write_data(0x0F);

    	//ILI9341_sendMultipleData((Uint8 []){0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F}, 15);

    	ILI9341_delay(5);

    	write_command(ILI9341_DISPLAY_ON);
}

static void hardware_init(void)
{
    nrf_gpio_cfg_output(ILI9341_DC_PIN);
    nrf_gpio_cfg_output(ILI9341_SS_PIN);

    SPI_init();

    //RESET TFT...
    nrf_gpio_cfg_output(ILI9341_RST_PIN);
    GPIO_write(ILI9341_RST_PIN, 0);
    SYSTICK_delay_ms(2);	//> 10us
    GPIO_write(ILI9341_RST_PIN, 1);
    SYSTICK_delay_ms(10);	// > 5 ms before sending command !
}

ret_code_t ILI9341_init(void)
{
    hardware_init();
    command_list();
    return NRF_SUCCESS;
}

static void ILI9341_uninit(void)
{
    SPI_uninit();
}

static void ILI9341_pixel_draw(uint16_t x, uint16_t y, uint32_t color)
{
    set_addr_window(x, y, x, y);

    const uint8_t data[2] = {color >> 8, color};

    nrf_gpio_pin_set(ILI9341_DC_PIN);

    SPI_write(data, sizeof(data));

    nrf_gpio_pin_clear(ILI9341_DC_PIN);
}

static void ILI9341_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    set_addr_window(x, y, x + width - 1, y + height - 1);

    const uint8_t data[2] = {color >> 8, color};

    ILI9341_DC_SET();
    ILI9341_SS_RESET();

    // Duff's device algorithm for optimizing loop.
    uint32_t i = (height * width + 7) / 8;

/*lint -save -e525 -e616 -e646 */
    switch ((height * width) % 8) {
        case 0:
            do {
                SPI_write(data, sizeof(data));
        case 7:
                SPI_write(data, sizeof(data));
        case 6:
                SPI_write(data, sizeof(data));
        case 5:
                SPI_write(data, sizeof(data));
        case 4:
                SPI_write(data, sizeof(data));
        case 3:
                SPI_write(data, sizeof(data));
        case 2:
                SPI_write(data, sizeof(data));
        case 1:
                SPI_write(data, sizeof(data));
            } while (--i > 0);
        default:
            break;
    }
/*lint -restore */

    ILI9341_SS_SET();
    ILI9341_DC_RESET();
}

static void ILI9341_dummy_display(void)
{
    /* No implementation needed. */
}

static void ILI9341_rotation_set(nrf_lcd_rotation_t rotation)
{
    write_command(ILI9341_MADCTL);
    switch (rotation) {
        case NRF_LCD_ROTATE_0:
            write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
            break;
        case NRF_LCD_ROTATE_90:
            write_data(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            break;
        case NRF_LCD_ROTATE_180:
            write_data(ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
            break;
        case NRF_LCD_ROTATE_270:
            write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            break;
        default:
            break;
    }
}

/**
 * @brief  Fills entire LCD with color
 * @param  color: Color to be used in fill
 * @retval None
 */
void ILI9341_Fill(uint16_t color) {
	/* Fill entire screen */
	ILI9341_INT_Fill(0, 0, ILI9341_Opts.width - 1, ILI9341_Opts.height, color);
}

/**
 * @brief  Fills chosen area with a color
 * @param  x0: X coordinate of top left point
 * @param  y0: Y coordinate of top left point
 * @param  x1: X coordinate of bottom right point
 * @param  y1: Y coordinate of bottom right point
 * @param  color: Color to be used in fill
 * @retval None
 */
void ILI9341_INT_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	uint32_t pixels_count;
	uint8_t datas[2];
	datas[0] = HIGHINT(color);
	datas[1] = LOWINT(color);

	/* Set cursor position */
	set_addr_window(x0, y0, x1, y1);

	/* Set command for GRAM data */
	write_command(ILI9341_GRAM);

	/* Calculate pixels count */
	pixels_count = (x1 - x0 + 1) * (y1 - y0 + 1);

	/* Send everything */
	ILI9341_SS_RESET();
	ILI9341_DC_SET();

	/* Go to 16-bit SPI mode */
	//TM_SPI_SetDataSize(ILI9341_SPI, TM_SPI_DataSize_16b);
	/* Send first 65535 bytes, SPI MUST BE IN 16-bit MODE */
	//SPI_DMA_SendHalfWord(ILI9341_SPI, color, (pixels_count > 0xFFFF) ? 0xFFFF : pixels_count);
	int32_t i;
	for(i=0;i<pixels_count;i++){
		SPI_write(datas, 2);
	}
	/* Wait till done */
	//while (SPI_DMA_Working(ILI9341_SPI));

	/* Check again */
	//if (pixels_count > 0xFFFF) {
		/* Send remaining data */
		//SPI_DMA_SendHalfWord(ILI9341_SPI, color, pixels_count - 0xFFFF);
		/* Wait till done */
		//while (SPI_DMA_Working(ILI9341_SPI));
	//}

	ILI9341_SS_SET();

	/* Go back to 8-bit SPI mode */
	//TM_SPI_SetDataSize(ILI9341_SPI, TM_SPI_DataSize_8b);
}



void ILI9341_Delay(volatile unsigned int delay) {
	for (; delay != 0; delay--);
}

/**
 * @brief  Puts string to LCD
 * @param  x: X position of top left corner of first character in string
 * @param  y: Y position of top left corner of first character in string
 * @param  *str: Pointer to first character
 * @param  *font: Pointer to @ref FontDef_t used font
 * @param  foreground: Color for string
 * @param  background: Color for string background
 * @retval None
 */
void ILI9341_Puts(uint16_t x, uint16_t y, char *str, FontDef_t *font, uint16_t foreground, uint16_t background) {
	uint16_t startX = x;

	/* Set X and Y coordinates */
	ILI9341_x = x;
	ILI9341_y = y;

	while (*str) {
		/* New line */
		if (*str == '\n') {
			ILI9341_y += font->FontHeight + 1;
			/* if after \n is also \r, than go to the left of the screen */
			if (*(str + 1) == '\r') {
				ILI9341_x = 0;
				str++;
			} else {
				ILI9341_x = startX;
			}
			str++;
			continue;
		} else if (*str == '\r') {
			str++;
			continue;
		}
		if(ILI9341_x > ILI9341_Opts.width - font->FontWidth)
		{
			//on passe � la ligne suivante, en s'alignant sous le d�but de la premi�re ligne
			ILI9341_y += font->FontHeight + 1;
			ILI9341_x = startX;
		}
		/* Put character to LCD */
		ILI9341_Putc(ILI9341_x, ILI9341_y, *str++, font, foreground, background);
	}
}

/*
 *	permet l'affichage de "gros caract�res" ('bigger' fois plus gros que la 'font' fournie)
 *	full_in_bigger d�fini le taux de remplissage des gros pixels g�n�r�s. Testez pour comprendre et appr�cier le style !
 *	@pre : full_in_bigger doit �tre inf�rieur ou �gal � bigger.
 *	exemples :
 *		ILI9341_PutBigs(20, 40, "tst4,3",&Font_16x26, ILI9341_COLOR_BLUE,ILI9341_COLOR_BLACK, 4, 3);
 *	 	ILI9341_PutBigs(20, 40, "test2,2",&Font_16x26, ILI9341_COLOR_BLUE,ILI9341_COLOR_BLACK, 2, 2);
 */
void ILI9341_PutBigs(uint16_t x, uint16_t y, char *str, FontDef_t *font, uint16_t foreground, uint16_t background, uint8_t bigger, uint8_t full_in_bigger) {
	uint16_t startX = x;

	/* Set X and Y coordinates */
	ILI9341_x = x;
	ILI9341_y = y;

	while (*str) {
		/* New line */
		if (*str == '\n') {
			ILI9341_y += font->FontHeight*bigger + bigger;
			/* if after \n is also \r, than go to the left of the screen */
			if (*(str + 1) == '\r') {
				ILI9341_x = 0;
				str++;
			} else {
				ILI9341_x = startX;
			}
			str++;
			continue;
		} else if (*str == '\r') {
			str++;
			continue;
		}
		if(ILI9341_x > ILI9341_Opts.width - font->FontWidth*bigger)
		{
			//on passe � la ligne suivante, en s'alignant sous le d�but de la premi�re ligne
			ILI9341_y += font->FontHeight*bigger + bigger;
			ILI9341_x = startX;
		}
		/* Put character to LCD */
		ILI9341_PutBigc(ILI9341_x, ILI9341_y, *str++, font, foreground, background, bigger, full_in_bigger);
	}
}

/**
 * @brief  Gets width and height of box with text
 * @param  *str: Pointer to first character
 * @param  *font: Pointer to @ref FontDef_t used font
 * @param  *width: Pointer to variable to store width
 * @param  *height: Pointer to variable to store height
 * @retval None
 */
void ILI9341_GetStringSize(char *str, FontDef_t *font, uint16_t *width, uint16_t *height) {
	uint16_t w = 0;
	*height = font->FontHeight;
	while (*str++) {
		w += font->FontWidth;
	}
	*width = w;
}

/**
 * @brief  Puts single character to LCD
 * @param  x: X position of top left corner
 * @param  y: Y position of top left corner
 * @param  c: Character to be displayed
 * @param  *font: Pointer to @ref FontDef_t used font
 * @param  foreground: Color for char
 * @param  background: Color for char background
 * @retval None
 */
void ILI9341_Putc(uint16_t x, uint16_t y, char c, FontDef_t *font, uint16_t foreground, uint16_t background) {
	uint32_t i, b, j;
	/* Set coordinates */
	ILI9341_x = x;
	ILI9341_y = y;

	if ((ILI9341_x + font->FontWidth) > ILI9341_Opts.width) {
		/* If at the end of a line of display, go to new line and set x to 0 position */
		ILI9341_y += font->FontHeight;
		ILI9341_x = 0;
	}

	/* Draw rectangle for background */
	ILI9341_INT_Fill(ILI9341_x, ILI9341_y, ILI9341_x + font->FontWidth, ILI9341_y + font->FontHeight, background);


	/* Draw font data */
	for (i = 0; i < font->FontHeight; i++) {


		if(font->datasize == 1)
		{
			uint8_t * data;
			data = (uint8_t *)font->data;
			b = (uint32_t)(data[(c - 32) * font->FontHeight + i]) << 8;
		}
		else if(font->datasize == 2)
		{
			uint16_t * data;
			data = (uint16_t *)font->data;
			b = data[(c - 32) * font->FontHeight + i];
		}
		else
			b = 0;	//should never happen
		for (j = 0; j < font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				ILI9341_pixel_draw(ILI9341_x + j, (ILI9341_y + i), foreground);
			}
		}
	}

	/* Set new pointer */
	ILI9341_x += font->FontWidth;
}


/*
 * bigger : amplification de la taille du caract�re
 * full_in_bigger : sorte de "pourcentage de remplissage" : mettre la m�me valeur que bigger... ou moins (essayez pour voir si �a vous plait)
 */
void ILI9341_PutBigc(uint16_t x, uint16_t y, char c, FontDef_t *font, uint16_t foreground, uint16_t background, uint8_t bigger, uint8_t full_in_bigger) {
	uint32_t i, b, j;
	/* Set coordinates */
	ILI9341_x = x;
	ILI9341_y = y;

	if ((ILI9341_x + font->FontWidth) > ILI9341_Opts.width) {
		/* If at the end of a line of display, go to new line and set x to 0 position */
		ILI9341_y += font->FontHeight;
		ILI9341_x = 0;
	}

	/* Draw rectangle for background */
	ILI9341_INT_Fill(ILI9341_x, ILI9341_y, ILI9341_x + bigger*font->FontWidth, ILI9341_y + bigger*font->FontHeight, background);


	/* Draw font data */
	for (i = 0; i < font->FontHeight; i++) {


		if(font->datasize == 1)
		{
			uint8_t * data;
			data = (uint8_t *)font->data;
			b = (uint32_t)(data[(c - 32) * font->FontHeight + i]) << 8;
		}
		else if(font->datasize == 2)
		{
			uint16_t * data;
			data = (uint16_t *)font->data;
			b = data[(c - 32) * font->FontHeight + i];
		}
		else
			b = 0;	//should never happen

		for (j = 0; j < font->FontWidth; j++)
		{
			if ((b << j) & 0x8000)
			{
				uint32_t k, l;

				for(k=0;k<full_in_bigger;k++)
					for(l=0;l<full_in_bigger;l++)
						ILI9341_pixel_draw(ILI9341_x + bigger*j+l, (ILI9341_y + bigger*i+k), foreground);
			}
		}
	}

	/* Set new pointer */
	ILI9341_x += font->FontWidth*bigger;
}


/**
 * @brief  Draws line to LCD
 * @param  x0: X coordinate of starting point
 * @param  y0: Y coordinate of starting point
 * @param  x1: X coordinate of ending point
 * @param  y1: Y coordinate of ending point
 * @param  color: Line color
 * @retval None
 */
void ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    //WindowMax();
    int   dx = 0, dy = 0;
    int   dx_sym = 0, dy_sym = 0;
    int   dx_x2 = 0, dy_x2 = 0;
    int   di = 0;


    dx = x1-x0;
    dy = y1-y0;

    if (dx == 0) {        /* vertical line */
        if (y1 > y0) ILI9341_INT_Fill(x0, y0, x0, y1, color);
        else ILI9341_INT_Fill(x0, y1, x0, y0, color);
        return;
    }

    if (dx > 0) {
        dx_sym = 1;
    } else {
        dx_sym = -1;
    }
    if (dy == 0) {        /* horizontal line */
        if (x1 > x0) ILI9341_INT_Fill(x0, y0, x1, y0, color);
        else  ILI9341_INT_Fill(x1, y0, x0, y0, color);
        return;
    }

    if (dy > 0) {
        dy_sym = 1;
    } else {
        dy_sym = -1;
    }

    dx = dx_sym*dx;
    dy = dy_sym*dy;

    dx_x2 = dx*2;
    dy_x2 = dy*2;

    if (dx >= dy) {
        di = dy_x2 - dx;
        while (x0 != x1) {

        	ILI9341_pixel_draw(x0, y0, color);
            x0 += dx_sym;
            if (di<0) {
                di += dy_x2;
            } else {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }
        ILI9341_pixel_draw(x0, y0, color);
    } else {
        di = dx_x2 - dy;
        while (y0 != y1) {
        	ILI9341_pixel_draw(x0, y0, color);
            y0 += dy_sym;
            if (di < 0) {
                di += dx_x2;
            } else {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }
        ILI9341_pixel_draw(x0, y0, color);
    }
    return;
}

/**
 * @brief  Draws circle on LCD
 * @param  x0: X coordinate of center circle point
 * @param  y0: Y coordinate of center circle point
 * @param  r: Circle radius
 * @param  color: Circle color
 * @retval None
 */
void ILI9341_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ILI9341_pixel_draw(x0, y0 + r, color);
	ILI9341_pixel_draw(x0, y0 - r, color);
	ILI9341_pixel_draw(x0 + r, y0, color);
	ILI9341_pixel_draw(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ILI9341_pixel_draw(x0 + x, y0 + y, color);
        ILI9341_pixel_draw(x0 - x, y0 + y, color);
        ILI9341_pixel_draw(x0 + x, y0 - y, color);
        ILI9341_pixel_draw(x0 - x, y0 - y, color);

        ILI9341_pixel_draw(x0 + y, y0 + x, color);
        ILI9341_pixel_draw(x0 - y, y0 + x, color);
        ILI9341_pixel_draw(x0 + y, y0 - x, color);
        ILI9341_pixel_draw(x0 - y, y0 - x, color);
    }
}

/**
 * @brief  Draws filled circle on LCD
 * @param  x0: X coordinate of center circle point
 * @param  y0: Y coordinate of center circle point
 * @param  r: Circle radius
 * @param  color: Circle color
 * @retval None
 */
void ILI9341_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ILI9341_pixel_draw(x0, y0 + r, color);
	ILI9341_pixel_draw(x0, y0 - r, color);
	ILI9341_pixel_draw(x0 + r, y0, color);
	ILI9341_pixel_draw(x0 - r, y0, color);
    ILI9341_DrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ILI9341_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        ILI9341_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        ILI9341_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        ILI9341_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

/**
 * @brief  Returns ILI9341 options
 * @retval ILI9341_Opts : options
 */

ILI931_Options_t ILI9341_getOptions(void){
	return ILI9341_Opts;
}

/**
 * @brief  Puts formated string to LCD
 * @param  x: X position of top left corner of first character in string
 * @param  y: Y position of top left corner of first character in string
 * @param  *font: Pointer to @ref FontDef_t used font
 * @param  foreground: Color for string
 * @param  background: Color for string background
 * @param  format : Formated string like printf
 * @retval None
 */
void ILI9341_printf(int16_t x, int16_t y, FontDef_t *font, int16_t foreground, int16_t background, const char *format, ...)
{
	char buffer[256];

	va_list args_list;
	va_start(args_list, format);
	vsnprintf(buffer, 256, format, args_list);
	va_end(args_list);

	ILI9341_Puts(x, y, buffer, font, foreground, background);
}

/**
 * @brief  Put Image on LCD
 * @param  x0: X coordinate of starting point
 * @param  y0: Y coordinate of starting point
 * @param  width: width of image
 * @param  height: height of image
 * @param  img: Pointeur sur le tableau des pixels de l'image en BMP 565
 * @param  size: Nombre d'�l�ment dans le tableau
 * @retval None
 */

void ILI9341_putImage(int16_t x0, int16_t y0, int16_t width, int16_t height, const int16_t *img, int32_t size){
	set_addr_window(x0, y0, x0 + width-1, y0 + height-1);

	uint8_t datas[2];

	/* Set command for GRAM data */
	write_command(ILI9341_GRAM);

	/* Send everything */
	ILI9341_SS_RESET();
	ILI9341_DC_SET();

	/* Go to 16-bit SPI mode */
	//TM_SPI_SetDataSize(ILI9341_SPI_INSTANCE, TM_SPI_DataSize_16b);

#ifndef LCD_DMA
	int32_t i;
	for(i=0; i < size; i++){
		datas[1] = HIGHINT(img[i]);
		datas[0] = LOWINT(img[i]);
		SPI_write(datas, 2);
		//SPI_writeNoRegister(ILI9341_SPI,0x07E0);
		//SPI_write(img[i]);
	}
#else
	SPI2_DMA_send16BitArray((Uint16 *)img, size);
#endif


	//TM_SPI_SetDataSize(ILI9341_SPI_INSTANCE, TM_SPI_DataSize_8b);
}

#ifndef LCD_DMA
//size : nombre de pixel (1 pixel = 1 bit !)
//img : tableau contenant une image en monochrome, 1 octet = 8 pixels ! (bit de poids fort = premier pixel)
//    tout bit � 1 est du blanc. tout bit � 0 est du noir.
//La taille (width et height) doit �tre multiple de 8 !
void ILI9341_putImage_monochrome(uint16_t color_front, uint16_t color_background, int16_t x0, int16_t y0, int16_t width, int16_t height, const uint8_t *img, int32_t size)
{
	set_addr_window(x0, y0, x0 + width-1, y0 + height-1);

	uint8_t datas_front[2];
	uint8_t datas_background[2];
	datas_front[1] = HIGHINT(color_front);
	datas_front[0] = LOWINT(color_front);
	datas_background[1] = HIGHINT(color_background);
	datas_background[0] = LOWINT(color_background);
	uint8_t * datas;
	/* Set command for GRAM data */
	write_command(ILI9341_GRAM);

	/* Send everything */
	ILI9341_SS_RESET();
	ILI9341_DC_SET();

	/* Go to 16-bit SPI mode */
	//TM_SPI_SetDataSize(ILI9341_SPI_INSTANCE, TM_SPI_DataSize_16b);


	int32_t i;
	for(i=0; i < size; i++){
		if((img[i/8]>>(7-i%8))&1)	//Hooo, la belle ligne. Prenez 5mn pour comprendre ce qui est fait ici ^^
			datas = datas_background;
		else
			datas = datas_front;

		SPI_write(datas, 2);
	}


	//TM_SPI_SetDataSize(ILI9341_SPI_INSTANCE, TM_SPI_DataSize_8b);
}
#endif	//ndef LCD_DMA

static void ILI9341_display_invert(bool invert)
{
    write_command(invert ? ILI9341_INVON : ILI9341_INVOFF);
}

static lcd_cb_t ILI9341_cb = {
    .height = ILI9341_HEIGHT,
    .width = ILI9341_WIDTH
};


const nrf_lcd_t nrf_lcd_ili9341 = {
    .lcd_init = ILI9341_init,
    .lcd_uninit = ILI9341_uninit,
    .lcd_pixel_draw = ILI9341_pixel_draw,
    .lcd_rect_draw = ILI9341_rect_draw,
    .lcd_display = ILI9341_dummy_display,
    .lcd_rotation_set = ILI9341_rotation_set,
    .lcd_display_invert = ILI9341_display_invert,
	.lcd_fill = ILI9341_Fill,
	.lcd_int_fill = ILI9341_INT_Fill,
	.lcd_delay = ILI9341_Delay,
	.lcd_puts = ILI9341_Puts,
	.lcd_putbigs = ILI9341_PutBigs,
	.lcd_getstringsize = ILI9341_GetStringSize,
	.lcd_putc = ILI9341_Putc,
	.lcd_putbigc = ILI9341_PutBigc,
	.lcd_drawline = ILI9341_DrawLine,
	.lcd_drawcircle = ILI9341_DrawCircle,
	.lcd_drawfilledcircle = ILI9341_DrawFilledCircle,
	.lcd_getoptions = ILI9341_getOptions,
	.lcd_printf = ILI9341_printf,
	.lcd_putimage = ILI9341_putImage,
	.lcd_putimagemonochrome = ILI9341_putImage_monochrome,
    .p_lcd_cb = &ILI9341_cb
};

#endif // USE_ILI9341
