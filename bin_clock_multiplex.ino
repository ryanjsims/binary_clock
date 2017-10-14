#define DATA_PIN 2
#define DATA_CLOCK 3
#define LATCH_PIN 4

volatile uint16_t currTime;
volatile byte dataClkMask, latchMask, dataMask, data, currRow;
volatile uint32_t counter;

static inline void initTimer1();
ISR(TIMER1_COMPA_vect);

void setup() {
  DDRB |= 1 << DATA_PIN | 1 << DATA_CLOCK | 1 << LATCH_PIN;
  dataClkMask = 1 << DATA_CLOCK; //00001000
  latchMask = 1 << LATCH_PIN;    //00010000
  dataMask = ~(1 << DATA_PIN);   //11111011
  initTimer1();
  counter = 0;
  currTime = 0;
  data = 0;
  currRow = 0xEE;  //11101110
}

void loop() {
  data |= currRow << 4;
  switch(currRow){
  case 0xEE: //11101110
    data |= 0x000F & (currTime >> 12);
    break;
  case 0xDD: //11011101
    data |= 0x000F & (currTime >> 8);
    break;
  case 0xBB: //10111011
    data |= 0x000F & (currTime >> 4);
    break;
  case 0x77: //01110111
    data |= 0x000F & currTime;
    break;
  }
  for(int i = 0; i < 8; i++){
    if((data << i) & 0x80){
      PORTB |= ~dataMask;
    } else {
      PORTB &= dataMask;
    }
    PORTB ^= dataClkMask;
    PORTB ^= dataClkMask;
  }
  PORTB ^= latchMask;
  PORTB ^= latchMask;
  data = 0;
  currRow = (currRow << 1) | (currRow >> 7);
}

static inline void initTimer1(){
  TCCR1 |= (1 << CTC1);
  TCCR1 |= (1 << CS10);
  OCR1C = 130;
  TIMSK |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect){
  counter++;
  if(counter == /*16875 32959*/21093){
    OCR1C = 93;
  } else if(OCR1C == 93){
    currTime++;
    OCR1C = 130;
    counter = 0;
  }
}
