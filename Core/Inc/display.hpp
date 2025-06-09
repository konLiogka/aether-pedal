/*
 * display.hpp
 *
 *  Created on: May 30, 2025
 *      Author: kliogka
 */




#include <main.h>
#include <cstdint>

#ifndef INC_DISPLAY_HPP_
#define INC_DISPLAY_HPP_

#define OLED_CS_Port    GPIOD
#define OLED_CS_Pin     GPIO_PIN_2
#define OLED_DC_Port    GPIOD
#define OLED_DC_Pin     GPIO_PIN_4
#define OLED_RST_Port   GPIOD
#define OLED_RST_Pin    GPIO_PIN_3

extern SPI_HandleTypeDef hspi1;

namespace Display {
    void init();
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t* data, uint16_t size);
    void reset();
    void setCursor(uint8_t column, uint8_t page);
    void drawChar(uint8_t c, uint8_t page, uint8_t col);
    void drawString(const char* str, uint8_t page, uint8_t col);
    void drawBitmap(const uint8_t* bitmap, uint8_t page, uint8_t col, uint8_t width, uint8_t heightPages);
    void clear(void);
}



#endif /* INC_DISPLAY_HPP_ */
