/*
 * mainApp.cpp
 *
 *  Created on: May 30, 2025
 *      Author: kliogka
 */

#include "mainApp.hpp"
#include "editPedal.hpp"
#include "modPedal.hpp"
#include "qspi_flash.hpp"
#include <cstdio>


#define BUFFER_SIZE 1024
uint16_t adc_buf[BUFFER_SIZE];
uint16_t dac_buf[BUFFER_SIZE];
 
 
 
void mainApp(void)
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, BUFFER_SIZE);
	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t*)dac_buf, BUFFER_SIZE, DAC_ALIGN_12B_R);

    Display::init();
    Display::clear();
    Display::drawBitmap(base_chain_bitmap, 0, 0);
    
    if(QSPIFlash::init() != HAL_OK)
    {
        Error_Handler();
    }

    EffectsChain chain;

    chain.setPedal(0, PedalType::OVERDRIVE_DISTORTION);
    chain.setPedal(1, PedalType::REVERB);
    chain.setPedal(2, PedalType::ECHO);
    chain.setPedal(3, PedalType::PASS_THROUGH);

    chain.draw();
    Display::setContrast(0x04);

    HAL_Delay(1000);

    if (QSPIFlash::saveEffectsChain(&chain) != HAL_OK) {
        Error_Handler();
    }
    
    EffectsChain loadedChain;

    if (QSPIFlash::loadEffectsChain(&loadedChain) != HAL_OK) {
        Error_Handler();
    }
    HAL_Delay(2000);


    uint8_t i=0;
    while(true)
    {
        loadedChain.selectedPedal = i;
        displaySelectedPedal(&loadedChain);
        i = (i + 1) % (static_cast<uint8_t>(PedalType::PASS_THROUGH) + 1);

        HAL_Delay(1000);

        if(i==3)
        {
            displayPedalSettings(loadedChain.getPedal(0), 0);  
            HAL_Delay(4000);
             Display::setContrast(0x01);
            displayPedalSettings(loadedChain.getPedal(0), 1);  
            HAL_Delay(4000);
        }
    }
      

}






