#include "modPedal.hpp" 
#include <cstring>

void drawValBar(float val, uint8_t x)
{
    if(val<0.1f)
        Display::drawBitmap(mod_pedal_val_bitmap_0, x, 11);
    else if(val<0.2f)
        Display::drawBitmap(mod_pedal_val_bitmap_1, x, 11);
    else if(val<0.3f)
        Display::drawBitmap(mod_pedal_val_bitmap_2, x, 11);
    else if(val<0.4f)
        Display::drawBitmap(mod_pedal_val_bitmap_3, x, 11);
    else if(val<0.5f)
        Display::drawBitmap(mod_pedal_val_bitmap_4, x, 11);
    else if(val<0.6f)
        Display::drawBitmap(mod_pedal_val_bitmap_5, x, 11);
    else if(val<0.7f)
        Display::drawBitmap(mod_pedal_val_bitmap_6, x, 11);
    else if(val<0.8f)
        Display::drawBitmap(mod_pedal_val_bitmap_7, x, 11);
    else if(val<0.9f)
        Display::drawBitmap(mod_pedal_val_bitmap_8, x, 11);
    else if(val<1.0f)
        Display::drawBitmap(mod_pedal_val_bitmap_9, x, 11);
    else
        Display::drawBitmap(mod_pedal_val_bitmap_10, x, 11);
}



void displayPedalSettings(Pedal *selectedPedal, uint8_t page)
{
    if (selectedPedal == nullptr) return;

    Display::drawBitmap(mod_pedal_bitmap, 0, 0);

    const char* const* names = selectedPedal->getMemberNames();
    float* params = new float[selectedPedal->getMemberSize()];
    selectedPedal->getParams(params);
    uint8_t numParams = selectedPedal->getMemberSize();
    uint8_t char_width = 6;
    uint8_t index = 0;
    if (page == 1)
    {
        index = 3;
    }



    for (uint8_t i = 0; i < 3; i++) {
        if (i + index >= numParams) break;  
        uint8_t bitmap_x = (i % 3) * 40 + 10;
        uint8_t string_width = strlen(names[i + index]) * char_width;
        uint8_t string_x = bitmap_x + (30 - string_width) / 2;

        drawValBar(params[i + index], bitmap_x);
        Display::drawString(names[i + index], string_x, 11);
        Display::drawFloat(params[i + index], bitmap_x + 5, 12);

     }
    if (page == 0)
    {
        Display::drawBitmap(arrow_right_bitmap, 120, 3);
    }
    else
    {
        Display::drawBitmap(arrow_left_bitmap, 0, 3);
    }

}
