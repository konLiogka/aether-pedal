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

enum class displayView {
    PEDALCHAIN_VIEW,
    PEDALSELECT_VIEW,
    PEDALEDIT_VIEW,
};  
EffectsChain loadedChain;
displayView currentView =  displayView::PEDALCHAIN_VIEW;

#define BUFFER_SIZE 1024
uint16_t adc_buf[BUFFER_SIZE] __attribute__((aligned(4)));  
uint16_t dac_buf[BUFFER_SIZE] __attribute__((aligned(4))); 

uint32_t potValues[3]; 
Pedal* selectedPedal =  Pedal::createPedal(PedalType::PASS_THROUGH);

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

    loadedChain.draw();
    loadedChain.selectedPedal = 0;
    selectedPedal = loadedChain.getPedal(loadedChain.selectedPedal); 

    currentView = displayView::PEDALCHAIN_VIEW;

    while(true)
    {
        HAL_Delay(100);
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
            switch(currentView)
            {
                case displayView::PEDALCHAIN_VIEW:
                    displaySelectedPedal(selectedPedal); 
                    currentView = displayView::PEDALSELECT_VIEW;
                    break;
                    
                case displayView::PEDALSELECT_VIEW:
                    Display::drawBitmap(mod_pedal_bitmap, 0, 0);

                    displayPedalSettings(selectedPedal, 0);
                    currentView = displayView::PEDALEDIT_VIEW;
                    break;
                    
                case displayView::PEDALEDIT_VIEW:
                    Display::drawBitmap(base_chain_bitmap, 0, 0);
                    loadedChain.draw();
                    currentView = displayView::PEDALCHAIN_VIEW;
                    break;
            }

            last_tick = tick;
        }
    }
}

void displayVolume(void)
{
    
}

extern "C"  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM8) {
        for (int channel = 0; channel < 3; channel++) {
            ADC_ChannelConfTypeDef sConfig = {0};
            switch(channel) {
                case 0: sConfig.Channel = ADC_CHANNEL_18; break; // PA2
                case 1: sConfig.Channel = ADC_CHANNEL_14; break; // PA3
                case 2: sConfig.Channel = ADC_CHANNEL_15; break; // PA4  
            }
            sConfig.Rank = ADC_REGULAR_RANK_1;
            sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
            sConfig.SingleDiff = ADC_SINGLE_ENDED;
            sConfig.OffsetNumber = ADC_OFFSET_NONE;
            sConfig.Offset = 0;
            
            HAL_ADC_ConfigChannel(&hadc2, &sConfig);
            
            HAL_ADC_Start(&hadc2);
            if (HAL_ADC_PollForConversion(&hadc2, 10) == HAL_OK) {
                uint32_t rawValue = HAL_ADC_GetValue(&hadc2);
                potValues[channel] = (((rawValue * 20) >> 12) * 5) + 5;}    
            HAL_ADC_Stop(&hadc2);
        }
        switch(currentView)
        {
            case displayView::PEDALCHAIN_VIEW:
                displayVolume();
                break;
                
            case displayView::PEDALSELECT_VIEW:
                displayVolume();
                break;
                
            case displayView::PEDALEDIT_VIEW:
            float* params = new float[selectedPedal->getMemberSize()];
            selectedPedal->getParams(params);
                if (potValues[0] != params[0] * 100 || 
                    potValues[1] != params[1] * 100 || 
                    potValues[2] != params[2] * 100 ) {
                    changePedalSettings(selectedPedal, 0, potValues);
                }
                break;
        }


         
    }
}


