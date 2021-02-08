#include "LiquidCrystal.h"
const int buzzerPin = 12;

LiquidCrystal lcd(9, 8, 5, 4, 10, 11);
long tempoTotal;
int beepsTotais = 3;
int contadorDeBeeps = 0;

void setup() {
  lcd.begin(16, 2); 
  tempoTotal = 5;
  pinMode(buzzerPin, OUTPUT); // Digital Pin 12
  
}
 
void loop() { 

  lcd.clear();
  lcd.print("BRANCAS X PRETAS");  
  lcd.setCursor(0,1);   
  long tempoAtual = millis() / 1000;
  long tempoRestante = tempoTotal - tempoAtual;
  int horas, minutos, segundos;
  
  
  if(tempoRestante>=0) {
    horas = tempoRestante / 60 / 60;
    minutos = tempoRestante / 60;
    segundos = tempoRestante % 60;
    printTime(horas , minutos , segundos);
    lcd.print("--");
    printTime(0 , 0 , 30);
  } else {
    lcd.print("PERDE  -"); 
    lcd.setCursor(9,1);
    printTime(0 , 0 , 30);
    if(contadorDeBeeps++ <= beepsTotais) {
      tone(buzzerPin,500,500);
      delay(1000);
    }
    
  }
  
  delay(10);

}



void printTime(int horas, int minutos, int segundos) {
    lcd.print(horas);
    lcd.print(":");
    if(minutos<10) lcd.print("0");
    lcd.print(minutos);
    lcd.print(":");
    if(segundos<10) lcd.print("0");
    lcd.print(segundos);
}
