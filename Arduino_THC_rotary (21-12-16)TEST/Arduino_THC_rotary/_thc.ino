unsigned int delayTime;
unsigned int SetVx10;

long z=0;
void Setup_THC()
{
  pinMode(outputUpPin, OUTPUT);
  pinMode(outputOkPin, OUTPUT);
  pinMode(outputDnPin, OUTPUT);
  pinMode(13, OUTPUT); // CKD test signal.
}
//
#define step_pin A4
#define dir_pin A3

//#define ENABLE_PIN       45

//#define STEP_HIGH       PORTL |=  0b00000100;
//#define STEP_LOW        PORTL &= ~0b00000100;
#define TIMER1_INTERRUPTS_ON TIMSK1 |= (1 << OCIE1A);
#define TIMER1_INTERRUPTS_OFF TIMSK1 &= ~(1 << OCIE1A);

unsigned int c0;
volatile int dir = 0;
volatile unsigned int max_speed = 100;  
volatile unsigned long n = 0;
volatile float d;
volatile unsigned long step_count = 0;
volatile unsigned long ramp_up_step_count = 0;
volatile unsigned long total_steps = 0;
volatile int step_position = 0;
volatile bool movement_Done = false;
void setup_step() {
  // put your setup code here, to run once:
  //pinMode(ENABLE_PIN, OUTPUT);
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 1000;
  TCCR1B |= (1 << WGM12) ;
  TCCR1B |= ((1 << CS11) | (1 << CS10));  // prescale = 64 and CTC mode 4
  interrupts();
  //c0 = 2000;
  c0 =  2000 * sqrt( 2 * 1.8 / 10 );
  //was 2000 * sqrt( 2 * angle / accel )
}



ISR(TIMER1_COMPA_vect) {
  if (step_count < total_steps) {
    //STEP_HIGH;
    //STEP_LOW;
digitalWrite(step_pin,HIGH);
digitalWrite(step_pin,LOW);
    step_count++;
    step_position += dir;

  }

  else {
    movement_Done = true;
    TIMER1_INTERRUPTS_OFF
  }
  if (ramp_up_step_count == 0)
  {
    n++;
    d = d - (2 * d) / (4 * n + 1);
    if (d <= max_speed)
    {
      d = max_speed;
      ramp_up_step_count = step_count;
    }
    if ( step_count >= total_steps / 2)
    {
      ramp_up_step_count = step_count;
    }
  }
  else if ( step_count >= total_steps - ramp_up_step_count)
  {
    n--;
    d = (d * (4 * n + 1)) / (4 * n + 1 - 2);
  }
  OCR1A = d;
}
void moveNsteps( long steps) {
  digitalWrite (dir_pin, steps < 0 ? HIGH : LOW);
  dir = steps > 0 ? 1 : -1;
  total_steps = abs(steps);
  d = c0;
  OCR1A = d;
  step_count= 0;
  n = 0;
  ramp_up_step_count = 0;
  movement_Done = false;
  TIMER1_INTERRUPTS_ON
}
void moveToPosition(long p, bool wait = true)
{
  moveNsteps(p - step_position);
  while (wait && !movement_Done);
}
//
void doTHC()
{
  if (Do)
  {
    Do = false;
    LCDtime ++;
    show ++;
    // CKD test signal.
    bitWrite(PORTB, 5, !bitRead(PORTB, 5));
 
    if (LCDtime > defaultLCDtime)
    {
      menu = 0;
      pos  = 0;
      LCDtime = 0;
      encoderVal = SetV;
    }
    if ((500 < ArcV) & (ArcV < 2500))
    {
      if (ArcV > StV * 10) delayTime ++;
      
      if (delayTime >= DT * 10)
      {
        SetVx10 = SetV * 10;
        delayTime = DT * 10;
        
        bitWrite(PORTC, 5, 1);
        
        if (ArcV >= SetVx10 + HyS)
        {
         moveToPosition(z+=100);
        }
        else if (ArcV <= SetVx10 - HyS)
        {
          moveToPosition(z-=100);
        }
        else
        {
          bitWrite(PORTC, 4, 0);
          bitWrite(PORTC, 3, 0);
        }       
      }
    }
    else if (menu != 12)
    {
      delayTime = 0;
      bitWrite(PORTC, 4, 0);
      bitWrite(PORTC, 5, 0);
      bitWrite(PORTC, 3, 0);
    }
  }
}

void RS232()
{
  Serial.print("SetV:");
  Serial.print(SetVx10);
  Serial.print(" ArcV:");
  Serial.print(ArcV);
  Serial.print(" StV:");
  Serial.print(StV);
  Serial.print(" DTime:");
  Serial.print(delayTime);
  Serial.print(" divRatio:");
  Serial.print(divRatio);
  Serial.println("");
}
