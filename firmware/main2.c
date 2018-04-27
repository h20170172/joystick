#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

#include "usbdrv/usbdrv.h"

//static uint8_t report [3]; // current
//static uint8_t report_out [3]; // last sent over USB
int ADC_INIT,ADC_READ,GPIO_WRITE,ADC_END;
static uint8_t replybuf[5];

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
static void gpio_val(uint8_t no,uint8_t value)
{
DDRB &=(1<<no);
PORTB &=(1<<value);
}
static void
adc_end()
{
	ADCSRA = 0;
	ADMUX = 0;
}
usbMsgLen_t
usbFunctionSetup(uchar data[8])
{
   usbRequest_t *rq = (void *)data;
   uint8_t len = 0;

   replybuf[0] = rq->bRequest;

   switch(rq->bRequest)
     {

      case GPIO_WRITE:
         replybuf[1] = rq->wValue.bytes[0]; //gpio no.
         replybuf[2] = rq->wValue.bytes[1]; // gpio value
        gpio_val(replybuf[1],replybuf[2]);    
	len = 3;
         break;

      case ADC_INIT:
         adc_init();

         len = 1;
         break;
      case ADC_READ:
           {
              uint16_t adc_val;
              replybuf[1] = rq->wValue.bytes[0]; // ADC gpio no ( 0 - 5)
              adc_val = adc_read(rq->wValue.bytes[0]);
              replybuf[2] = adc_val & 0xFF;
              replybuf[3] = (adc_val >> 8);

              len = 4;
            }
         break;

      case ADC_END:
         adc_end();

         len = 1;
         break;

      default:
         break;
     }

 usbMsgPtr = (unsigned char *) replybuf;

   return len; // should not get here
}

int __attribute__((noreturn))
main(void)
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
    

