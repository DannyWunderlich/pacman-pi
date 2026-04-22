#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include <stdio.h>
#include "queue.h"

// Global column variable
int col = -1;

// Global key state
static bool state[16]; // Are keys pressed/released

// Keymap for the keypad
const char keymap[17] = "DCBA#9630852*741";

// Defined here to avoid circular dependency issues with autotest
// You can see the struct definition in queue.h
KeyEvents kev = { 
    .head = 0, 
    .tail = 0 
};

void keypad_drive_column();
void keypad_isr();

/********************************************************* */
// Implement the functions below.

void keypad_init_pins() {
      // fill in
    // set 2,3,4,5 as inputs (ROWS)
    sio_hw->gpio_oe_clr |= (1 << 2);
    sio_hw->gpio_oe_clr |= (1 << 3);
    sio_hw->gpio_oe_clr |= (1 << 4);
    sio_hw->gpio_oe_clr |= (1 << 5);

    for(int i = 2; i < 6; i++)
    {
        sio_hw->gpio_clr |= (1 << i);
        hw_write_masked(&pads_bank0_hw->io[i],
                   PADS_BANK0_GPIO0_IE_BITS,
                   PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_OD_BITS
        );
        io_bank0_hw->io[i].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
        hw_clear_bits(&pads_bank0_hw->io[i], PADS_BANK0_GPIO0_ISO_BITS);
    }

    // set 6,7,8,9 as outputs (COLS)
    sio_hw->gpio_oe_set |= (1 << 6);
    sio_hw->gpio_oe_set |= (1 << 7);
    sio_hw->gpio_oe_set |= (1 << 8);
    sio_hw->gpio_oe_set |= (1 << 9);

    for(int i = 6; i < 10; i++)
    {
        sio_hw->gpio_set |= (1 << i);
        hw_write_masked(&pads_bank0_hw->io[i],
                   PADS_BANK0_GPIO0_IE_BITS,
                   PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_OD_BITS
        );
        io_bank0_hw->io[i].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
        hw_clear_bits(&pads_bank0_hw->io[i], PADS_BANK0_GPIO0_ISO_BITS);
    }
}

void keypad_init_timer() {
    uint32_t delay_us0 = 1000000; //1 second
    uint32_t delay_us1 = 1100000; //1.10 seconds
    
    hw_set_bits(&timer_hw->inte, 1u << 0); //bit shifted by 0 to indicate alarm 0
    irq_set_exclusive_handler(timer_hardware_alarm_get_irq_num(timer_hw, 0), keypad_drive_column);
    irq_set_enabled(timer_hardware_alarm_get_irq_num(timer_hw, 0), true);
    uint64_t target0 = timer_hw->timerawl + delay_us0;
    timer_hw->alarm[0] = (uint32_t) target0;

    hw_set_bits(&timer_hw->inte, 1u << 1); //bit shifted by 1 to indicate alarm 1
    irq_set_exclusive_handler(timer_hardware_alarm_get_irq_num(timer_hw, 1), keypad_isr);
    irq_set_enabled(timer_hardware_alarm_get_irq_num(timer_hw, 1), true);
    uint64_t target1 = timer_hw->timerawl + delay_us1;
    timer_hw->alarm[1] = (uint32_t) target1;
}

void keypad_drive_column() {
    
    hw_clear_bits(&timer_hw->intr, 1u << 0); //Bitshifted by 0 for alarm 0
    col = (col + 1) % 4;
    
    gpio_put(6, false);
    gpio_put(7, false);
    gpio_put(8, false);
    gpio_put(9, false);

    gpio_put(col + 6, true);

    //Arm alarm 0 again
    uint64_t target = timer_hw -> timerawl + 25000;
    timer_hw -> alarm[0] = (uint32_t) target;

    // gpio_put(6, false);
    // gpio_put(7, false);
    // gpio_put(8, false);
    // gpio_put(9, false);

    // gpio_put(col + 6, true);
    // sleep_ms(25);
    // col = (col + 1) % 4;
}

uint8_t keypad_read_rows() {
    uint8_t row_mask = 0x00;         // ASSUMING MSB is gpio 2
    row_mask |= gpio_get(2) << 0;
    row_mask |= gpio_get(3) << 1;
    row_mask |= gpio_get(4) << 2;
    row_mask |= gpio_get(5) << 3;
    return(row_mask);
}

void keypad_isr() {
    uint8_t row_vals;

    hw_clear_bits(&timer_hw->intr, 1u << 1); //Bitshifted by 1 for alarm 1
    row_vals = keypad_read_rows();
    
    for(int i = 0; i < 4; i++)
    {
        if((row_vals & (1u << i)) && (!state[(col * 4) + i] ))
        {
            
            state[(col * 4) + i] = 1;
            uint16_t push_mask = keymap[(col * 4) + i];
            push_mask |= (1u << 8);
            
            key_push(push_mask);
        }
        else if(!(row_vals & (1u << i)) && (state[(col * 4) + i]))
        {
           state[(col * 4) + i] = 0;
            uint16_t push_mask = keymap[(col * 4) + i];
            push_mask |= (0u << 8);

            key_push(push_mask);
        }
    }

    //Arm alarm 1 again
    uint64_t target = timer_hw -> timerawl + 25000;
    timer_hw -> alarm[1] = (uint32_t) target;
    
    // if(gpio_get_irq_event_mask(2) & GPIO_IRQ_EDGE_RISE){
    //     gpio_acknowledge_irq(2, GPIO_IRQ_EDGE_RISE); //acknowledge
    //     #ifdef STEP3
    //         // TODO: add the function to push the key value into the mailbox FIFO
    //         multicore_fifo_push_blocking(keymap[(col * 4) + 0]);
    //     #else
    //         printf("%c\n", keymap[(col * 4) + 0]);
    //     #endif

    // }
    // if(gpio_get_irq_event_mask(3) & GPIO_IRQ_EDGE_RISE){
    //     gpio_acknowledge_irq(3, GPIO_IRQ_EDGE_RISE); //acknowledge
    //     #ifdef STEP3
    //         // TODO: add the function to push the key value into the mailbox FIFO 
    //         multicore_fifo_push_blocking(keymap[(col * 4) + 1]);
    //     #else
    //         printf("%c\n", keymap[(col * 4) + 1]);
    //     #endif
    // }
    // if(gpio_get_irq_event_mask(4) & GPIO_IRQ_EDGE_RISE){
    //     gpio_acknowledge_irq(4, GPIO_IRQ_EDGE_RISE); //acknowledge
    //     //step 3 below
    //     #ifdef STEP3
    //         // TODO: add the function to push the key value into the mailbox FIFO 
    //         multicore_fifo_push_blocking(keymap[(col * 4) + 2]);
    //     #else
    //         printf("%c\n", keymap[(col * 4) + 2]);
    //     #endif

    // }
    // if(gpio_get_irq_event_mask(5) & GPIO_IRQ_EDGE_RISE){
    //     gpio_acknowledge_irq(5, GPIO_IRQ_EDGE_RISE); //acknowledge
    //     #ifdef STEP3
    //         // TODO: add the function to push the key value into the mailbox FIFO 
    //         multicore_fifo_push_blocking(keymap[(col * 4) + 3]);
    //     #else
    //         printf("%c\n", keymap[(col * 4) + 3]);
    //     #endif
    // }
}