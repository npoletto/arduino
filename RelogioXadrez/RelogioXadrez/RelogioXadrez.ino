#include "LiquidCrystal.h"

const int buzzerPin = 12;
const int buttonPinPreta = 3;
const int buttonPinBranca = 2;
const int beepsTotais = 3;
const int AGUARDANDO_INICIO = 1;
const int BRANCA_JOGA = 2;
const int PRETA_JOGA = 4;
const int PAUSA = 6;
const int TERMINO = 7;


LiquidCrystal lcd(9, 8, 5, 4, 10, 11);

int contadorDeBeeps = 0;
int estado = AGUARDANDO_INICIO;
long tempoTotal;
long ultimaAtualizacao;
long tempoBrancas, tempoPretas;
long startBrancas, startPretas;
long tempoRestanteBrancas, tempoRestantePretas;
int indexSetup=0, contadorJogadas=0; 

long configs[] = { 15000, 60000, 180000, 300000, 600000, 900000, 1200000, 1800000, 3600000, 7200000 } ;


void setup() {
  lcd.begin(16, 2); 
  lcd.clear();
  tempoTotal = configs[0];
  pinMode(buzzerPin, OUTPUT); 
  pinMode(buttonPinPreta, INPUT_PULLUP);
  pinMode(buttonPinBranca, INPUT_PULLUP);
  Serial.begin(9600);
  tempoRestanteBrancas = tempoTotal;
  tempoRestantePretas = tempoTotal;
}
 
void loop() { 
  long tempoAtual = millis();
  
 
  switch(estado) {
   
    case AGUARDANDO_INICIO:     
      lcd.clear();
      lcd.print("INICIE PRETAS");
      lcd.setCursor(0,1);
      lcd.print("SETUP: ");
      printTime(tempoTotal);
      if(digitalRead(buttonPinBranca)==0) {
        delay(250);
        if(indexSetup == 9) {
          indexSetup = 0;
          tempoTotal = configs[0]; 
        } else {
          tempoTotal = configs[++indexSetup];
        }
        tempoRestanteBrancas = tempoTotal;
        tempoRestantePretas = tempoTotal;
      }
      else if(digitalRead(buttonPinPreta)==0) {
        contadorJogadas++;
        startBrancas = tempoAtual;
        estado = BRANCA_JOGA;
      }
      delay(100);
    break;

    case BRANCA_JOGA:
      tempoBrancas = tempoAtual - startBrancas;
      atualizaDisplay(tempoRestanteBrancas - tempoBrancas, tempoRestantePretas);
      if(tempoRestanteBrancas - tempoBrancas <= 0) {
        tempoRestanteBrancas = tempoRestanteBrancas - tempoBrancas;
        beep3Vezes();
        estado = TERMINO;
        
      }
      else if(digitalRead(buttonPinBranca)==0) {
        tempoRestanteBrancas = tempoRestanteBrancas - tempoBrancas;
        startPretas = tempoAtual;
        contadorJogadas++;
        estado = PRETA_JOGA;
      }
      delay(100);
    break;

    case PRETA_JOGA:
        tempoPretas = tempoAtual - startPretas;
        atualizaDisplay(tempoRestanteBrancas, tempoRestantePretas - tempoPretas);
        if(tempoRestantePretas - tempoPretas <= 0) {
          tempoRestantePretas = tempoRestantePretas - tempoPretas;
          beep3Vezes();
          estado = TERMINO;
        }
        else if(digitalRead(buttonPinPreta)==0) {
          tempoRestantePretas = tempoRestantePretas - tempoPretas;
          startBrancas = tempoAtual;
          contadorJogadas++;
          estado = BRANCA_JOGA;
        }
        delay(100);    
        break;

    case TERMINO:
      atualizaDisplay(tempoRestanteBrancas,tempoRestantePretas);
      delay(5000);
      break;
  }


}

void atualizaDisplay(long tempoBrancas, long tempoPretas) {
  
  if(ultimaAtualizacao + 250 < millis()) {
    int espacamento = 0;
    if(tempoBrancas > 3600000 || tempoPretas > 3600000) {
      espacamento = 2;
    }
   ultimaAtualizacao = millis();
   lcd.clear();
   lcd.print("WHITE| ");
   if(contadorJogadas<10) lcd.print("0");
   lcd.print(contadorJogadas);
   if(contadorJogadas<100) lcd.print(" ");   
   lcd.print("|BLACK");  
   lcd.setCursor(0,1);   
   if(tempoBrancas<0) {
    lcd.print("LOSER|"); 
    lcd.setCursor(11-espacamento,1);
   } else {
    printTime(tempoBrancas);
    lcd.print("|");
   }
      lcd.setCursor(10-espacamento,1);
      lcd.print("|");
    if(tempoPretas<0) {
      lcd.print("LOSER"); 
    } else {
      printTime(tempoPretas);
    }
  }
}

void printTime(long time) {
    time = time / 1000;
    int horas = time/3600;
    int minutos = (time - horas*3600)/60;
    int segundos = time - (horas*3600) - (minutos * 60);
    if(horas>0) {
      lcd.print(horas);
      lcd.print(":");
    }
    if(minutos<10) lcd.print("0");
    lcd.print(minutos);
    lcd.print(":");
    if(segundos<10) lcd.print("0");
    lcd.print(segundos);
}

void beep3Vezes() {
  for(int i=0;i<3;i++) {
      tone(buzzerPin,500,500);
      delay(1000);
    }
}
