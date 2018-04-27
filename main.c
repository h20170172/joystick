
#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "usbdrv/usbdrv.h"

// Report format: Y, X, buttons (up to 8)
static uint16_t report [2]; // current
static uint16_t report_out [2]; // last sent over USB
uint16_t val2=2;
uint16_t val1;
//enum command { VENDOR_RQ_READ_BUFFER0};

static void adc_init()
{
   // ADC Multiplexer Selection Register
   ADMUX = (1 << REFS0);

   // ADCSRA - ADC control and status Register A
   // ADEN - ADC enable
   ADCSRA = (1 << ADEN) |
      (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);
}

static void led_init()
{
  DDRB = 0b00000001;
  PORTB |= 0x00;
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

static int read_joy( void )
{
	//char buffer[20];
	report [0] = 0;
	report [1] = 0;
	//report [2] = 0;
	//Y
        val1 = adc_read(0);
       // report[0] = val1;
        //val2 = adc_read(1);
        //report[1] = val2;
     // return val1;
	//sprintf(buffer, "%d, %d", report[0], report[1]);
	
	// Y
	/*if ( ! (PINB & 0x04) ) report [0] = -127;
	if ( ! (PINB & 0x08) ) report [0] = +127;
	
	// X
	if ( ! (PINB & 0x10) ) report [1] = -127;
	if ( ! (PINB & 0x20) ) report [1] = +127;
	
	// Buttons
	if ( ! (PIND & 0x01) ) report [2] |= 0x01;
	if ( ! (PIND & 0x02) ) report [2] |= 0x02;
	// ...*/
}

// X/Y joystick w/ 8-bit readings (-127 to +127), 8 digital buttons


//static uchar buffer[10] ;
usbMsgLen_t usbFunctionSetup(uchar data [8] )
{
	usbRequest_t const* rq = (void *) data;

	//if ( (rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_CLASS )
	//	return 0;
	unsigned char replybuf[8];
	switch ( rq->bRequest )
	{
	case 0:  PORTB = 0B00000001;// HID joystick only has to handle this
		replybuf[0]=2;
                usbMsgPtr = (unsigned char *) replybuf;
		return sizeof report;
	
	//case USBRQ_HID_SET_REPORT: // LEDs on joystick?
	
	default:
		return 0;
	}
}
/*static uchar  currentPosition, bytesRemaining;
 
usbMsgLen_t usbFunctionSetup(uchar setupData[8])
{
    usbRequest_t *rq = (void *)setupData;   // cast to structured data for parsing
    switch(rq->bRequest){
    case VENDOR_RQ_READ_BUFFER:
        currentPosition = 0;                // initialize position index
        bytesRemaining = rq->wLength.word;  // store the amount of data requested
        return USB_NO_MSG;                          // tell driver to use usbFunctionRead()
    }
    return 0;                               // ignore all unknown requests
}
 
uchar usbFunctionRead(uchar *data, uchar len)
{
    uchar i;
    if(len > bytesRemaining)                // len is max chunk size
        len = bytesRemaining;               // send an incomplete chunk
    bytesRemaining -= len;
    for(i = 0; i < len; i++)
        data[i] = getData(currentPosition); // copy the data to the buffer
    return len;                             // return real chunk size
}*/

/*static void toggle_led( void )
{
	DDRC  |= 1;
	PORTC ^= 1;
}
*/
int main( void )
{
	//uint16_t adc_val;
	usbInit();
	sei();
	led_init();
	adc_init();
    /*   while(1)
    {
     adc_val=read_joy();
     if (adc_val > 800)
   {
      PORTB = 0X01;
   }
  else PORTB = 0X00;
 }*/
	for ( ;; )
	{
		usbPoll();
		read_joy();
		// Don't bother reading joy if previous changes haven't gone out yet.
		// Forces delay after changes which serves to debounce controller as well.
		/*if ( usbInterruptIsReady() )
		{
			read_joy();
			
			// Don't send update unless joystick changed
			if ( memcmp( report_out, report, sizeof report ) )
			{
				memcpy( report_out, report, sizeof report );
				usbSetInterrupt( report_out, sizeof report_out );
				//toggle_led();
			}
		}*/
	}
	
	return 0;
}
