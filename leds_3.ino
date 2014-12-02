
volatile unsigned char signal = 0;
volatile unsigned char bang = 0;
volatile unsigned char bits;
volatile unsigned char clock = 1; 
#define NOP __asm__ __volatile__ ("nop\n\t")

// ISR routine that gets called each cycle
ISR(TIMER2_COMPA_vect)
{
  if ( clock == 1 )
  {
    PORTB |= 0x01;
    NOP;
    PORTB &= 0xFE;
  }
  if ( bang > 0)
  {
    PORTB |= 0x01;
    NOP;
    PORTB &= 0xFE;
  }
  signal = 1;
}

void setup()
{
  pinMode(8, OUTPUT);
  PORTB &= 0xFE;

  TCCR2A = 0;            // set entire TCCR0A register to 0
  TCCR2B = 0;            // same for TCCR0B
  TIMSK2 = 0;            // turn off interupt
  TCNT0  = 0;            //initialize counter value to 0
  OCR2A  = 99;
  TCCR2A = (1<<WGM21);   
  TCCR2B = ( 1<<CS11 );  // set prescale to 8
  TIMSK2 = (1<<OCIE0A);

}

void shift(unsigned char shifting) 
{
  for( bits = 0; bits < 8; bits++ ) 
  {
    while ( signal == 0 );
    signal = 0;
    bang = shifting & ( 0x80 >> bits );
  }
  while(signal == 0);
  
}

void eos( boolean glat )
{
  clock = 0;
  int count;
  if ( glat )
    count = 8;
  else
    count = 4;
   
  for ( int i = 0; i < count; i++)
  {
    while ( signal == 0 );
    signal = 0;
  }
  clock = 1;
}

void light( unsigned char r, unsigned char g, unsigned b, boolean latch )
{
  shift(0x3A);
  shift(g);
  shift(r);
  shift(b);
  if (latch)
    eos(false);
  else
    eos(true);
}

void loop()
{
  light( 0x0F, 0x00, 0x00, false );
  light( 0x00, 0x00, 0x00, false );
  light( 0x00, 0x00, 0x00, true );
  
  delay(1000);
  light( 0x00, 0x00, 0x00, false );
  light( 0x00, 0x0F, 0x00, false );
  light( 0x00, 0x00, 0x00, true  );
  
  delay(1000);
  light( 0x00, 0x00, 0x00, false );
  light( 0x00, 0x00, 0x00, false );
  light( 0x00, 0x00, 0x0F, true  );
  
  delay(1000);
}
