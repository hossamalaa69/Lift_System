byte column=0,row=0;
const byte Output[4]={A0,A1,A2,A3};
const byte Input[2]={A4,3};
int arrPins [3] = {4,5,6};
int arr_values [8][3] = { {0,0,0},
					            	  {0,0,1},
                          {0,1,0},
                          {0,1,1},
                          {1,0,0},
                          {1,0,1},
                          {1,1,0},
                          {1,1,1} };           
int cycle[4][4]={{1,1,0,0},{0,1,1,0},{0,0,1,1},{1,0,0,1}};
int Motor[4]={0,1,7,8};
unsigned long LastStep =0;
unsigned long kdelay=0;  
unsigned long previousMillisSwitch=0;
int arrEncoder[3]={10,11,12};
bool arrFloors [8] = {false,false,false,false,false,false,false,false};
int currentFloor = 0;
int stepsToFloor = 0;
int floorDirection = 1;
unsigned long liftWaitTimer = 0;

void setup() {
  for(int i=0;i<3;i++)
  pinMode(arrPins[i],OUTPUT);
  for(int i=0;i<4;i++)
    pinMode(Motor[i],OUTPUT);
  for(byte i=0;i<4;i++)  
    pinMode(Output[i],OUTPUT);
  for(byte s=0;s<2;s++)
    pinMode(Input[s],INPUT_PULLUP);
  pinMode(A5,INPUT);  
  pinMode(9,OUTPUT);  
  pinMode(2,INPUT);
  attachInterrupt(digitalPinToInterrupt(2), readySwitch, RISING); 
  for(int i=0;i<3;i++)
    pinMode(arrEncoder[i],INPUT);
  displayDigit(currentFloor);
}

void loop() {
 if(digitalRead(2) && ((unsigned long)(millis()-previousMillisSwitch) >= 200))
    addRequest(digitalRead(arrEncoder[0]) + 2*digitalRead(arrEncoder[1]) + 4*digitalRead(arrEncoder[2]));
 
 digitalWrite(9,analogRead(A5)>716);
 
 if(millis()-kdelay>10){
    kdelay=millis();
    int key = keypad();
    if(key != -1)
      addRequest(key);
  }

  if(millis()-liftWaitTimer > 1000 && (analogRead(A5)<716)){
      if(stepsToFloor % 360 == 0){
        if(arrFloors[currentFloor]){
            removeFromQueue(currentFloor);
            liftWaitTimer=millis();
            return ;
        }
        checkMinDirection();    
      }
     
      if(stepsToFloor > 0)
          stepMotor(floorDirection);  
  } 
}

void addRequest(int index){
    arrFloors[index] = true;
  }
void removeFromQueue(int index){
    arrFloors[index] = false;
  }

void checkMinDirection(){
    int minUp = 9;
    int minDown = 9;
    for(int i=currentFloor+1;i<8;i++){
        if(arrFloors[i])
            if((i-currentFloor) < minUp)
              minUp = i-currentFloor;
    }
    for(int i=0;i<currentFloor;i++){
          if(arrFloors[i])
            if((currentFloor-i) < minDown)
              minDown = currentFloor;
    }
    
    if(minUp<minDown){
        stepsToFloor = minUp * 360;
        floorDirection = 1;  
      }else if(minDown<minUp){
        stepsToFloor = minDown*360;
        floorDirection = -1;
      }else if(minUp==minDown && minUp != 9){
        stepsToFloor=minUp*360;
        floorDirection = 1;  
      }else
        stepsToFloor=0;
}

void readySwitch(){
    previousMillisSwitch = millis();   
}

void stepMotor(int mDirection){
        if(millis()-LastStep >=30){
            if(mDirection == 1)
              for(int i=0;i<4;i++)
                for(int j=0;j<4;j++)
                  digitalWrite(Motor[3-j],cycle[i][j]);
            else
              for(int i=0;i<4;i++)
                for(int j=0;j<4;j++)
                  digitalWrite(Motor[j],cycle[i][j]);                     
            LastStep=millis();
            stepsToFloor -= 4;

            if(stepsToFloor % 360 == 0){
              currentFloor += mDirection;
              displayDigit(currentFloor);
              
              }
        }
}

// function used to detect wcolumniccolumn button is used
int keypad()  
{
  static bool no_press_flag=0;
  for(byte x=0;x<2;x++) 
  {
     if (digitalRead(Input[x])== HIGH);  
     else
      break;
     if(x==1)  
     {
      no_press_flag=1;
      column=0;
      row=0;
     }
  }
  if(no_press_flag==1)
  {
    for(byte r=0;r<4;r++) 
    digitalWrite(Output[r],LOW);
    for(column=0;column<2;column++)  
    {
      if(digitalRead(Input[column])==HIGH) 
      continue;
      else    //if one of inputs is low
      {
          for (row=0;row<4;row++)   
          {
          digitalWrite(Output[row],HIGH);   
          if(digitalRead(Input[column])==HIGH)  
          {
            no_press_flag=0;                
            for(byte w=0;w<4;w++) 
            digitalWrite(Output[w],LOW);
            return row*2+column;   
          }
          }
      }
    }
  }
 return -1;
}

void displayDigit(int index){
      for(int i=0;i<3;i++)
        digitalWrite(arrPins[i],arr_values[index][i]);
} 
