#include <stdio.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uinstd.h>
#include <avr/pgmspace.h>
#include "usbdrv.h"

#define AVR_USB_FIRMWARE
//#include "common.h"

//#include "spi.h"
//#include "adc.h"

static uint8_t replybuf[5];
enum command {ADC_INIT, ADC_READ, GPIO_WRITE, ADC_END,};
static void
adc_init()
{
   // ADC Multiplexer Selection Register
   ADMUX = (1 << REFS0);

   // ADCSRA - ADC control and status Register A
   // ADEN - ADC enable
   ADCSRA = (1 << ADEN) |
      (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);
}

static uint16_t
adc_read(uint8_t ch)
{
   // make sure value remains between 0 and 7
   ch &= 0b00000111;
    // clears the bottom 3 bit before ORing
   ADMUX = (ADMUX & 0xF8) | ch;

   // Write 1 to ADSC
   ADCSRA |= (1 << ADSC);

   //Wait till ADSC becomes 0
   while (ADCSRA & (1 << ADSC));

   return ADC;
}
static void
adc_end()
{
	ADCSRA = 0;
	ADMUX = 0;
}


/* static inline uint8_t _adjust_gpio(uint8_t no)
{
   if (no == 3) no = 4;
   else if (no == 4) no = 6;
   else if (no == 5) no = 7;
   else if (no == 6) no = 8;
   return no;
}
*/
/*
static void
_gpio_init(uint8_t no, uint8_t input)
{
   switch (no)
     {
      case 1 ... 6:
         //some adjustments since PD2 and PD4 are not available.
         if (no > 2)
           no = _adjust_gpio(no);

         if (!input)
           DDRD |= (1 << (no - 1));
         else
           DDRD &= ~(1 << (no - 1));
         break;
      case 7 ... 12:
         if (!input)
           DDRB |= (1 << (no - 7));
         else
           DDRB &= ~(1 << (no - 7));
         break;
      default:
         break;
     }
}
*/

/*
static void
_gpio_access(uint8_t no, uint8_t write, uint8_t *val)
{
   switch(no)
     {
      case 1 ... 6:
         if (no > 2)
           no = _adjust_gpio(no);

         if (write)
           {
              if (*val)
                PORTD |= (1 << (no - 1));
              else
                PORTD &= ~(1 << (no - 1));
           }
         else
           {
              if (bit_is_clear(PIND, (no - 1)))
                *val = 0;
              else
                *val = 1;
           }
         break;
      case 7 ... 12:
         if (write)
           {
              if  (*val)
                PORTB |= (1 << (no - 7));
              else
                PORTB &= ~(1 << (no - 7));
           }
         else
           {
              if (bit_is_clear(PINB, (no - 7)))
                *val = 0;
              else
                *val = 1;
           }
         break;
      default:
         break;
     }
}
*/
/*
#define MOSI PB3
#define MISO PB4
#define SCK PB5
#define SS PB3
 */
static void gpio_val(uint8_t no,uint8_t value)
{
DDRB &=(1<<no);
PORTB &=(1<<value);
}

usbMsgLen_t;
USB_PUBLIC uchar usbFunctionSetup(uchar data[8])
{
   usbRequest_t *rq = (void *)data;
   uint8_t len = 0;
   uint16_t adc_val;


   replybuf[0] = rq->bRequest;

   switch (rq->bRequest)
     {

      case GPIO_WRITE:
         replybuf[1] = rq->wValue.bytes[0]; //gpio no.
         replybuf[2] = rq->wValue.bytes[1]; // gpio value
        // gpio_val(replybuf[1], 1, &replybuf[2]);
           gpio_val(replybuf[1], &replybuf[2]);
         len = 3;
         break;

      case ADC_INIT:
         adc_init();

         len = 1;
         break;
      case ADC_READ:
              replybuf[1] = rq->wValue.bytes[0]; // ADC gpio no ( 0 - 5)
              adc_val = adc_read(rq->wValue.bytes[0]);
              replybuf[2] = adc_val & 0xFF;
              replybuf[3] = (adc_val >> 8);

              len = 4;
         break;

      case ADC_END:
         adc_end();

         len = 1;
         break;

      default:
         break;
     }

   usbMsgPtr = (unsigned char *)replybuf;

   return len; // should not get here
}

int main(void)
{
   uchar i = 0;
   wdt_enable(WDTO_2S);
   usbInit();
   usbDeviceDisconnect();

   while(--i)
     {
        wdt_reset();
        _delay_ms(1);
     }

   usbDeviceConnect();
   sei();

   while(1)
     {
        wdt_reset();
        usbPoll();
     }
}
