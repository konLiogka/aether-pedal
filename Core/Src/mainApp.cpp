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

// External ADC values updated by TIM8 interrupt
extern uint32_t potValues[3];

 



EffectsChain loadedChain;
uint8_t i = 0;
uint8_t bPressed = 0;

 

 
void mainApp(void)
{
	// HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t*)dac_buf, BUFFER_SIZE, DAC_ALIGN_12B_R);

    Display::init();
    Display::clear();
    Display::drawBitmap(base_chain_bitmap, 0, 0);
    
 
    if(QSPIFlash::init() != HAL_OK)
    {
        Display::displayError("QSPI Flash", QSPIFlash::init() );
        Error_Handler();
    }

    EffectsChain chain;

    chain.setPedal(0, PedalType::OVERDRIVE_DISTORTION);
    chain.setPedal(1, PedalType::REVERB);
    chain.setPedal(2, PedalType::ECHO);
    chain.setPedal(3, PedalType::PASS_THROUGH);

    if (QSPIFlash::saveEffectsChain(&chain) != HAL_OK) {
        Display::displayError("QSPI save Flash", QSPIFlash::saveEffectsChain(&chain)  );
        Error_Handler();
    }

    if (QSPIFlash::loadEffectsChain(&loadedChain) != HAL_OK) {
        Display::displayError("QSPI load Flash", QSPIFlash::loadEffectsChain(&loadedChain)  );
        Error_Handler();
    }
    HAL_Delay(2000);

    loadedChain.draw();
    loadedChain.selectedPedal = 0;

    

    while(true)
    {
        HAL_Delay(10);


        if(i==3)
        {
            Display::setBrightness(0x40);

            displayPedalSettings(loadedChain.getPedal(0), 0);  
            HAL_Delay(4000);
            displayPedalSettings(loadedChain.getPedal(0), 1);
            Display::setBrightness(0x10);
            HAL_Delay(4000);


        }
    }
      

}



extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t last_tick = 0;
    if (GPIO_Pin == GPIO_PIN_0)
    {
        uint32_t tick = HAL_GetTick();
        if ((tick - last_tick) > 150)  
        {
            if(bPressed==0)
            {
            displaySelectedPedal(&loadedChain); 
            bPressed++;

            }
            else if(bPressed==1)
            {
                displayPedalSettings(loadedChain.getPedal(loadedChain.selectedPedal), 0);  
                bPressed++;
            }
            else
            {
                Display::drawBitmap(base_chain_bitmap, 0, 0);
                loadedChain.draw();
                loadedChain.selectedPedal = 0;
                bPressed=0;
            }

            last_tick = tick;
        }
    }
}




