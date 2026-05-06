#include <Key.h>
#include <Keypad.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define trigger 11
#define echo 12
#define buzzer 13


LiquidCrystal_I2C lcd(0x27, 20, 4);

float getDistance();
void displayPage(char pageKeyPressed, char lastKey, int currentPage);
void setPage(char pageIndex);

const byte LINHAS = 4; // Linhas do teclado
const byte COLUNAS = 4; // Colunas do teclado

const char TECLAS_MATRIZ[LINHAS][COLUNAS] = { // Matriz de caracteres (mapeamento do teclado)
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

const byte PINOS_LINHAS[LINHAS] = {9, 8, 7, 6}; // Pinos de conexao com as linhas do teclado
const byte PINOS_COLUNAS[COLUNAS] = {5, 4, 3, 2}; // Pinos de conexao com as colunas do teclado

Keypad keypadMap = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS, LINHAS, COLUNAS); // Inicia teclado
char keyPressed, lastKeyPressed;
int currentDisplay = 0;
int cursor[2] = {0,0};

float configDistance=0;
float configOffset=0;
float distance=0;

unsigned long startSprintTime;
int sprintTimeSeconds =0;
int sprintTimeMinutes=0;
int sprintTimeTotalSeconds =0;
bool startSprint=false;
bool sprintcount=false;
int firstDistanceDecimal, secondDistanceDecimal, thirdDistanceDecimal;
int firstOffsetDecimal, secondOffsetDecimal, thirdOffsetDecimal;

void setup()
{
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(buzzer, OUTPUT);
  keypadMap.setDebounceTime(150);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  displayPage(0, 0, 0);
}

void loop(){
  Serial.println(getDistance());
  distance=getDistance();
  //Serial.println(configOffset);
  setPage(currentDisplay);
  keyPressed = keypadMap.getKey();

  if (keyPressed) {
    tone(buzzer, 5000, 100);
    Serial.println(keyPressed);
    displayPage(keyPressed, lastKeyPressed, currentDisplay);
    lastKeyPressed=keyPressed;
  }
	
}

float getDistance(){
  float distance;
  float seconds;
  
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  seconds = pulseIn(echo, HIGH);
  distance = ((seconds * (344 * 0.0001))/2);
  if(distance>99.99)
    distance = 99.99;
  return distance;
}


void setPage(char pageIndex){
  char pageKey = pageIndex;
  switch(pageKey){
    case 0:
      lcd.setCursor(4,1);
      lcd.print("SPRINT TIMER");
      break;

    case 1:
      lcd.setCursor(0,0);
      lcd.print("A - INICIAR SPRINT");
      lcd.setCursor(0,1);
      lcd.print("B - CONFIG. SPRINT");
      lcd.setCursor(0,2);
      lcd.print("C - ULTIMO SPRINT");
      lcd.setCursor(0,3);
      lcd.print("D - CONFIG. SISTEMA");
      break;

    case 2:
      if(startSprint){
        lcd.setCursor(4, 2);
        lcd.print("PARAR  ");
        if(distance<configOffset){
          if(!sprintcount){
            sprintcount=true;
            startSprintTime=millis();
          }else if(sprintTimeSeconds>5){
            startSprint=false;
            sprintcount=false;
            lcd.setCursor(4, 2);
            lcd.print("FINALIZADO");
            delay(2000);
            lcd.clear();
            currentDisplay=4;
          }
        }
        if(sprintcount){
          Serial.print("segundos: ");
          Serial.println(sprintTimeSeconds);
          Serial.print("tempo: ");
          Serial.println(millis());
          sprintTimeTotalSeconds = (millis() - startSprintTime)/1000;
          sprintTimeSeconds = sprintTimeTotalSeconds%60;
          sprintTimeMinutes = (sprintTimeTotalSeconds/60)%60;
          lcd.setCursor(7, 1);
          if(sprintTimeMinutes<10){
            lcd.print("0");
            lcd.setCursor(8, 1);
          }
          lcd.print(sprintTimeMinutes);

          lcd.setCursor(9, 1);
          lcd.print(":");
            
          lcd.setCursor(10, 1);
          if(sprintTimeSeconds<10){
            lcd.print("0");
            lcd.setCursor(11, 1);
          }
          lcd.print(sprintTimeSeconds);
        }
      }else{
        lcd.setCursor(0,0);      
        lcd.print("DISTANCIA: ");
        lcd.setCursor(15,0);      
        lcd.print("m");
        lcd.setCursor(0,1);
        lcd.print("TEMPO: ");
        lcd.setCursor(0,2);
        lcd.print("A - INICIAR");
      }

      break;

    case 3:
      lcd.setCursor(0,0);
      lcd.print("DISTANCIA: ");
      lcd.setCursor(15,0);
      lcd.print("m");
      lcd.setCursor(0,1);
      lcd.print("OFFSET: ");
      lcd.setCursor(12,1);
      lcd.print("cm");
      lcd.setCursor(0,3);
      lcd.print("[C] -> Mover cursor");
      if(cursor[0]==0){
        cursor[0]=10;
      }
      lcd.setCursor(cursor[0], cursor[1]);
      break;

    case 4:
      lcd.setCursor(0,0);
      lcd.print("DISTANCIA: ");
      lcd.setCursor(11,0);
      lcd.print(configDistance);
      lcd.setCursor(0,1);
      lcd.print("TEMPO: ");
      lcd.setCursor(7,1);lcd.setCursor(7, 1);
      if(sprintTimeMinutes<10){
          lcd.print("0");
          lcd.setCursor(8, 1);
      }
      lcd.print(sprintTimeMinutes);

      lcd.setCursor(9, 1);
      lcd.print(":");
            
      lcd.setCursor(10, 1);
      if(sprintTimeSeconds<10){
        lcd.print("0");
        lcd.setCursor(11, 1);
      }
      lcd.print(sprintTimeSeconds);
      lcd.setCursor(0,2);
      lcd.print("VEL.: ");
      lcd.setCursor(6,2);
      lcd.print(configDistance/((float) sprintTimeTotalSeconds));
      lcd.setCursor(10,2);
      lcd.print("m/s");
      
      break;

    case 5:
      lcd.setCursor(0,0);
      lcd.print("UN. DISTANCIA: m/s");
      lcd.setCursor(0,1);
      lcd.print("UN. TEMPO: segundo");
      lcd.setCursor(0,2);
      lcd.print("UN. VELOC.: m/s");
      lcd.setCursor(5,3);
      lcd.print("[C] <[D]");
      break;
  }
}

void displayPage(char pageKeyPressed, char lastKey, int currentPage){
  char pageKey = pageKeyPressed;
  if(currentPage == 0){
    lcd.setCursor(4,1);
    lcd.print("SPRINT TIMER");
    
    switch(pageKey){
      case '#':
        currentDisplay=1;
        lcd.clear();
          break;
    } 

  }else if(currentPage == 1){
    switch(pageKey){
      case 'A':
        lcd.clear();
        currentDisplay=2;
        lcd.setCursor(11, 0);
        if(configDistance<10.0){
          lcd.print("0");
          lcd.setCursor(12, 0);
        }
        lcd.print(configDistance, 1);
        
        lcd.setCursor(7, 1);
        if(sprintTimeMinutes<10){
          lcd.print("0");
          lcd.setCursor(8, 1);
        }
        lcd.print(sprintTimeMinutes);

        lcd.setCursor(9, 1);
        lcd.print(":");
          
        lcd.setCursor(10, 1);
        if(sprintTimeSeconds<10){
          lcd.print("0");
          lcd.setCursor(11, 1);
        }
        lcd.print(sprintTimeMinutes);
        break;

      case 'B':
        currentDisplay=3;   
        lcd.clear();
        lcd.setCursor(11, 0);
        if(configDistance<10.0){
          lcd.print("0");
          lcd.setCursor(12, 0);
        }
        lcd.print(configDistance, 1);
        lcd.setCursor(8, 1);
        if(configOffset<10.0){
          lcd.print("0");
          lcd.setCursor(9, 1);
        }
        lcd.print(configOffset, 1);
        break;

      case 'C':
        currentDisplay=4;    
        lcd.clear();
        break;

      case 'D':
        currentDisplay=5;    
        lcd.clear();
        break;

      case '*':
        currentDisplay=0;  
        lcd.clear();
        break;

      case '#':
        break;
    }
  }else if(currentPage == 2){
    switch(pageKey){
      case '*':
        startSprint=false;
        sprintcount=false;
        sprintTimeTotalSeconds=0;
        lcd.clear();
        currentDisplay=1;
        break;

      case 'A':
        startSprint=true;   
        break;
    }
  }else if(currentPage == 3){
    switch(pageKey){
      case '*':
        currentDisplay=1;
        configDistance = (firstDistanceDecimal*10)+secondDistanceDecimal+(thirdDistanceDecimal/10.0);
        configOffset = (firstOffsetDecimal*10)+secondOffsetDecimal+(thirdOffsetDecimal/10.0);
        Serial.println(configDistance);
        lcd.clear();
        break;

      case 'C':
        if(lastKey == 'C'){
          if(cursor[1]==0){
            switch(cursor[0]){
              case 11:   
                lcd.print(firstDistanceDecimal);
                break;
              case 12:
                lcd.print(secondDistanceDecimal);
                break;
              case 14:
                lcd.print(thirdDistanceDecimal);
                break;
            }
          }else if(cursor[1]==1){
            switch(cursor[0]){
              case 8:   
                lcd.print(firstOffsetDecimal);
                break;
              case 9:
                lcd.print(secondOffsetDecimal);
                break;
              case 11:
                lcd.print(thirdOffsetDecimal);
                break;
            }
          }
        }
        cursor[0]++;
        if(((cursor[0]==13) && (cursor[1]==0) || (cursor[0]==10) && (cursor[1]==1))){
          cursor[0]++;
        }else if((cursor[0]==15) && (cursor[1]==0)){
          cursor[0]=8;
          cursor[1]=1;
        }else if((cursor[0]==12) && (cursor[1]==1)){
          cursor[0]=11;
          cursor[1]=0;
        }
        lcd.setCursor(cursor[0], cursor[1]);
        lcd.print("_");
        break;	

      default:
        lcd.print(pageKey);
        if((cursor[0]==11)&&(cursor[1]==0)){
          firstDistanceDecimal=pageKey - '0';
        }else if((cursor[0]==12)&&(cursor[1]==0)){
          secondDistanceDecimal=pageKey - '0';
        }else if((cursor[0]==14)&&(cursor[1]==0)){
          thirdDistanceDecimal=pageKey - '0';
        }else if((cursor[0]==8)&&(cursor[1]==1)){
          firstOffsetDecimal=pageKey - '0';
        }else if((cursor[0]==9)&&(cursor[1]==1)){
          secondOffsetDecimal=pageKey - '0';
        }else if((cursor[0]==11)&&(cursor[1]==1)){
          thirdOffsetDecimal=pageKey - '0';
        }
        break;
    }
  }else if(currentPage == 4){ 
    switch(pageKey){
      case '*':
        currentDisplay=1;    
        lcd.clear();
        break;
    }
  }else if(currentPage == 5){
    switch(pageKey){
      case '*':
        currentDisplay=1;    
        lcd.clear();
        break;
    }
  }
}

void moveCursorRight(int currentPage, char key){
  switch(currentPage){
    case 3:
      if(key==1024){
        cursor[0]++;
        lcd.setCursor(cursor[0], cursor[1]);
        lcd.print(key);
      }
      break;

    case 4:
      lcd.setCursor(0,0);
      lcd.print("DISTANCIA: 0,0m");
      lcd.setCursor(0,1);
      lcd.print("TEMPO: 0,0s");
      lcd.setCursor(0,2);
      lcd.print("VELOCIDADE: 0,0m/s");
      break;

    case 5:
      lcd.setCursor(0,0);
      lcd.print("UN. DISTANCIA: m/s");
      lcd.setCursor(0,1);
      lcd.print("UN. TEMPO: segundo");
      lcd.setCursor(0,2);
      lcd.print("UN. VELOC.: m/s");
      lcd.setCursor(5,3);
      lcd.print(">[C] <[D]");
      break;
  }
}
