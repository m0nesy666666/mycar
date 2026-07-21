#include "key.h"
 extern int status;
//高电平有效
uint8_t get_key_status(uint32_t key) {
    uint32_t high_bits = DL_GPIO_readPins(KEY_PORT, key); //0x00000040 0b01000000 PB6 0~31
    if((high_bits & key) != 0) return 1;
    else return 0;
}



// void GROUP1_IRQHandler()
// {
//     switch (DL_GPIO_getPendingInterrupt(GPIOB))
//     {
//     case KEY_KEY1_IIDX:
        
//         status = (status + 1) % 3;
//         break;
//     case KEY_KEY2_IIDX:
//         status = (status + 3 -1) % 3;
        
//         break;
    
    
//     default:
//         break;
//     }

    

// }



