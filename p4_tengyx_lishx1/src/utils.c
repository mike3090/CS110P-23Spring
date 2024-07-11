#include "utils.h"
#include "systick.h"
#include "gd32vf103_libopt.h"

// store the last visit time of each button.
// note: we're using version-0 board, so
// JOY_LEFT/DOWN/RIGHT/UP/CTR = GPIO_PIN_0/1/2/5/4,
// BUTTON_1/2 = GPIO_PIN_6/7,
// so 8 elements is enough.
// Here [3] is for Boot0.
int lastVisit[8]={0};

/* -----------------------------
 Description: Return 1 if button number ch is pressed
 			  Return 0 otherwise
----------------------------- */
int Get_Button(int ch)
{
    /* hack for new board*/

    // as required, 
    // if a button is triggered at time $t_0$,
    // then the button cannot be triggered again before $t_0+0.3$.
    // I'm not sure whether it's the correct implementation...
    
    int if_pressed = 0;
    int press_time = get_timer_value();
    if (ch != GPIO_PIN_13){
        if_pressed = (int)(gpio_input_bit_get(GPIOA, ch));
    }
    else{
        if_pressed = (int)(gpio_input_bit_get(GPIOC, ch));
    }
    
    if(if_pressed == 0){
        return 0;
    }

    if(press_time - lastVisit[ch]<SystemCoreClock/4000.0 *300){
        // 300ms is not delayed
        return 0;
    }
    else{
        lastVisit[ch]=press_time;
        return 1;
    }
}

/* -----------------------------
 Description: Return 1 if button BOOT0 ch is pressed
 			  Return 0 otherwise
----------------------------- */
int Get_BOOT0(void)
{   
    int if_pressed = 0;
    int press_time = get_timer_value();
    if_pressed =  (int)(gpio_input_bit_get(GPIOA, GPIO_PIN_8));
    if(if_pressed == 0){
        return 0;
    }

    if(press_time - lastVisit[3]<SystemCoreClock/4000.0 *300){
        // 300ms is not delayed
        return 0;
    }
    else{
        lastVisit[3]=press_time;
        return 1;
    }
}