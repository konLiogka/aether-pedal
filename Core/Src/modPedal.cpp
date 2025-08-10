#include "modPedal.hpp" 
#include <cstring>

void loadPedalSettings(Pedal *pedal)
{
    const char* const* names = pedal->getMemberNames();
    float* params = new float[pedal->getMemberSize()];
    pedal->getParams(params);

    for (uint8_t i = 0; i < 3; i++) {
        int bitmap_x = i * 45;
        int char_width = 6;
        int string_width = strlen(names[i]) * char_width;
        int string_x = bitmap_x + (30 - string_width) / 2;
        Display::drawBitmap(mod_pedal_val_bitmap, bitmap_x, 11);
        Display::drawString(names[i], string_x, 11);

        Display::drawFloat(params[i], bitmap_x + 5, 12);
    }

    if(pedal->getMemberSize()>3)
    {
        Display::drawBitmap(arrow_right_bitmap, 120, 3);
        HAL_Delay(2000);
        Display::clear();
        Display::drawBitmap(mod_pedal_bitmap, 0, 0);
        for (uint8_t i = 0; i < pedal->getMemberSize() - 3; i++) {
            int bitmap_x = i * 45;
            int char_width = 6;
            int string_width = strlen(names[i+3]) * char_width;
            int string_x = bitmap_x + (30 - string_width) / 2;
            Display::drawBitmap(mod_pedal_val_bitmap, bitmap_x, 11);
            Display::drawString(names[i+3], string_x, 11);

            Display::drawFloat(params[i+3], bitmap_x + 5, 12);
        }
        HAL_Delay(2000);
    }
}

void displayPedalSettings(Pedal *selectedPedal)
{
    if (selectedPedal == nullptr) return;

    Display::drawBitmap(mod_pedal_bitmap, 0, 0);
    if(selectedPedal->getMemberSize()>3)
    {
        Display::drawBitmap(arrow_right_bitmap, 130, 6);
    }
    loadPedalSettings(selectedPedal);
}
