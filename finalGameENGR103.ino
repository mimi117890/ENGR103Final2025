
#include <Adafruit_CircuitPlayground.h> //cpx library

float midi[127] ;
int A_four = 440 ; 

volatile int num , points=0 , notPoint=0 , i , note= 60 , roundLength = 1100 , pointComp=0 , roundAccel ;
const byte leftButtonPin=4 , rightButtonPin=5 , switchPin=7 ;
volatile bool leftFlag = false , rightFlag = false , switchState = false , switchFlag = false ; 
int delayTime = 300 ;
float X , Y , Z , totalAccel ; //from Author: Carter Nelson

//************************************************************************************************************************************************************************
void randNumPick() { //select random number 1, 2 or 3.
  num = random(1,4) ;
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void lightSequence(int i) { //activates portion, counts points
  for(int n=0; n<i; n++) {
    pointComp = 0 ; 
    soundOrLight(n) ;
    shake() ;
    if(leftFlag) {
      lightResponse() ;
    }
    if(rightFlag) {
      soundResponse() ;
    }
    if(roundAccel>4) {
      shakeResponse() ;
    }
    if(pointComp==num) {
      points = points+1 ;
      Serial.println("+1 point! ") ;
    }
    else {
      notPoint= notPoint+1 ;
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void soundOrLight(int light) { //run random number function, and depending on the response, run a different mix of light and sound
  randNumPick() ;
  if(num==1) {
    CircuitPlayground.playTone(midi[note], delayTime) ;
  }
  if(num == 2) {
    CircuitPlayground.setPixelColor(light , 255 , 255 , 255) ;
    delay(delayTime) ;
    CircuitPlayground.setPixelColor(light , 0 , 0 , 0) ;
  }
  if(num==3) {
    CircuitPlayground.setPixelColor(light , 255 , 255 , 255) ;
    CircuitPlayground.playTone(midi[note], delayTime) ;
    CircuitPlayground.setPixelColor(light , 0 , 0 , 0) ;

  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void lightResponse() { //for measuring points - response to only light
    delay(5) ;
    pointComp=2 ;
    leftFlag=false ;
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void soundResponse() { //for measuring points - response to only sounds
    delay(5) ;
    pointComp=1 ;
    rightFlag=false ;
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void shakeResponse() { //for measuring points - response to both.
  pointComp=3 ; 
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void shake() {  //detect and count shakes
  roundAccel=0 ;
  //Serial.println(roundLength) ;
  for(int j=0 ; j<roundLength/10 ; j++) {
    X = 0 ;  //from Author: Carter Nelson **
    Y = 0 ;
    Z = 0 ;
    for (int i=0; i<10; i++) {
      X += CircuitPlayground.motionX() ;
      Y += CircuitPlayground.motionY() ;
      Z += CircuitPlayground.motionZ() ;
      delay(1) ;
    }
    X /= 10 ;
    Y /= 10 ;
    Z /= 10 ;

    totalAccel = sqrt(X*X + Y*Y + Z*Z) ;
    //**
    if(totalAccel>15) {
      roundAccel=roundAccel+1 ;
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void generateMIDI()   {  //sound functions
  for (int x = 0; x < 127; ++x)   {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0))  ;
    Serial.println(midi[x]) ;
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------


void rightButtonInterrupt() { // reset button interrupt
  rightFlag = true ;
}
void leftButtonInterrupt() { // reset button interrupt
  leftFlag = true ;
}
void switchInterrupt() { //switch interrupt
  switchFlag = true ;
}

//*********************************************************************************************************************************************************************************

void setup() {
  Serial.begin(9600) ;
  CircuitPlayground.begin() ;
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_8_G) ;
  attachInterrupt(digitalPinToInterrupt(leftButtonPin), leftButtonInterrupt , RISING) ; 
  attachInterrupt(digitalPinToInterrupt(rightButtonPin), rightButtonInterrupt , RISING) ; 
  attachInterrupt(digitalPinToInterrupt(switchPin), switchInterrupt , CHANGE) ;
  generateMIDI()  ;
}
//**********************************************************************************************************************************************************************************
void loop() {
  if(switchFlag) {
    delay(5);
    switchState = true ; 
    switchFlag = false ; 
  }
  if(switchState) {
    roundLength=1100 ;
    points = 0 ;
    for( i=1; i<10; i++) { //number of rounds
      Serial.print(" level:  ") ;
      Serial.println(i) ;
      lightSequence(i) ;
      Serial.print("current points: ") ;
      Serial.println(points) ;
      delay(delayTime*5) ;
      roundLength=roundLength-100 ;
    }
      Serial.print("total points(max 45): ") ;
      Serial.println(points) ;
      Serial.print("Accuracy:  ") ;
      Serial.print((int)((float)points/45)*100) ;
      Serial.println("%") ;
 
      switchState = false ;
  }
}
