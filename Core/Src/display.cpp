/*
 * display.cpp
 *
 *  Created on: May 30, 2025
 *      Author: kliogka
 */

#include <display.hpp>


namespace Display {

void reset() {
    HAL_GPIO_WritePin(OLED_RST_Port, OLED_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(OLED_RST_Port, OLED_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
}

void clear(void) {
    setCursor(0, 0);
    for(uint16_t i = 0; i < 128 * 8; i++) {
        uint8_t zero = 0x00;
        writeData(&zero, 1);
    }
}

void init() {
    reset();

    writeCommand(0xAE); // Display OFF
    writeCommand(0xD5); writeCommand(0x80); // Clock div
    writeCommand(0xA8); writeCommand(0x3F); // Mux ratio 64
    writeCommand(0xD3); writeCommand(0x00); // Offset
    writeCommand(0x40); // Start line 0
    writeCommand(0x8D); writeCommand(0x14); // Charge pump ON
    writeCommand(0x20); writeCommand(0x00); // Horizontal addressing mode
    writeCommand(0xA1); // Segment remap
    writeCommand(0xC8); // COM scan direction remapped
    writeCommand(0xDA); writeCommand(0x12); // COM pins
    writeCommand(0x81); writeCommand(0xCF); // Contrast
    writeCommand(0xD9); writeCommand(0xF1); // Precharge
    writeCommand(0xDB); writeCommand(0x40); // VCOM deselect level
    writeCommand(0xA4); // Resume RAM content display
    writeCommand(0xA6); // Normal display
    writeCommand(0x2E); // Deactivate scroll
    writeCommand(0xAF); // Display ON
}

void writeCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

void writeData(uint8_t* data, uint16_t size) {
    HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

void setCursor(uint8_t column, uint8_t page) {
    writeCommand(0x21); // Set column address
    writeCommand(column);
    writeCommand(127);
    writeCommand(0x22); // Set page address
    writeCommand(page);
    writeCommand(7);
    HAL_Delay(1);
}


int getFontIndex(char c) {
    if (c < 0x20 || c > 0x7E)
        return -1;
    return c - 0x20;
}

void drawChar(char c, uint8_t page, uint8_t col) {
    int index = getFontIndex(c);
    if (index < 0) return;

    setCursor(col, page);
    writeData((uint8_t *)font_5x7[index], 5);

    uint8_t space = 0x00;
    writeData(&space, 1);
}

void drawString(const char* str, uint8_t page, uint8_t col) {
    while (*str && col < 128) {
        drawChar(*str, page, col);
        col += 6;
        str++;
    }
}

void drawBitmap(const uint8_t* bitmap, uint8_t width, uint8_t height, uint8_t x, uint8_t pageStart) {
    // SSD1309 uses PAGE addressing mode (default after init)
    // Each byte = 8 vertical pixels (MSB at top)

    uint16_t byteIndex = 0;
    uint8_t pagesNeeded = (height + 7) / 8; // Round up for partial pages

    for (uint8_t page = 0; page < pagesNeeded; page++) {
        setCursor(x, pageStart + page);

        // Send one full row (width bytes)
        writeData((uint8_t*)&bitmap[byteIndex], width);
        byteIndex += width;
    }
}

}
