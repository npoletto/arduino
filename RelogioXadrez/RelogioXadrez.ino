#include "LiquidCrystal.h"
#include <EEPROM.h>

const int buzzerPin = 12;
const int buttonPinPreta = 3;
const int buttonPinBranca = 2;
const int ledPinBranca = 6;
const int ledPinPreta = 7;

const int beepsTotais = 3;
const int AGUARDANDO_INICIO = 1;
const int BRANCA_JOGA = 2;
const int PRETA_JOGA = 4;
const int PAUSA = 6;
const int TERMINO = 7;


LiquidCrystal lcd(9, 8, 5, 4, 10, 11);

int estado = AGUARDANDO_INICIO;
long tempoTotal;
long ultimaAtualizacao;
long tempoBrancas, tempoPretas;
long startBrancas, startPretas;
long tempoRestanteBrancas, tempoRestantePretas;
int indexSetup, contadorJogadas=0; 
int ultimoEstadoAntesDaPausa;
long configs[] = { 15000, 60000, 180000, 300000, 600000, 900000, 1200000, 1800000, 3600000, 7200000 } ;
int memoria;

void setup() {
  lcd.begin(16, 2); 
  lcd.clear();
  memoria = EEPROM.read(0); 
  tempoTotal = configs[memoria];
  indexSetup = memoria;
  pinMode(buzzerPin, OUTPUT); 
  pinMode(ledPinBranca, OUTPUT); 
  pinMode(ledPinPreta, OUTPUT); 
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
      ligaLeds(false,true);
      if(digitalRead(buttonPinBranca)==0) {
        delay(250);
        if(indexSetup >= 9) {
          indexSetup = 0;
          tempoTotal = configs[0]; 
        } else {
          tempoTotal = configs[++indexSetup];
        }
        tempoRestanteBrancas = tempoTotal;
        tempoRestantePretas = tempoTotal;
      }
      else if(digitalRead(buttonPinPreta)==0) {
        if(memoria!=indexSetup) {
          EEPROM.write(0,indexSetup);
        }
        contadorJogadas++;
        startBrancas = tempoAtual;
        estado = BRANCA_JOGA;
        ligaLeds(true,false);
      }
      delay(100);
    break;

    case BRANCA_JOGA:
      
      tempoBrancas = tempoAtual - startBrancas;
      atualizaDisplay(tempoRestanteBrancas - tempoBrancas, tempoRestantePretas);
      if(tempoRestanteBrancas - tempoBrancas <= 0) {
        tempoRestanteBrancas = tempoRestanteBrancas - tempoBrancas;
        beepNVezes(3, 500);
        estado = TERMINO;
        
      }
      else if(digitalRead(buttonPinBranca)==0 && !digitalRead(buttonPinPreta)==0) {
        tempoRestanteBrancas = tempoRestanteBrancas - tempoBrancas;
        startPretas = tempoAtual;
        contadorJogadas++;
        estado = PRETA_JOGA;
        ligaLeds(false,true);
      } else  if(digitalRead(buttonPinBranca)==0 && digitalRead(buttonPinPreta)==0) {
        ultimoEstadoAntesDaPausa = BRANCA_JOGA;
        tempoRestanteBrancas = tempoRestanteBrancas - tempoBrancas;
        ligaLeds(true,true);
        estado = PAUSA;
        beepNVezes(2,150);
        delay(2000);     
      }
  
      delay(100);
    break;

    case PRETA_JOGA:
        tempoPretas = tempoAtual - startPretas;
        atualizaDisplay(tempoRestanteBrancas, tempoRestantePretas - tempoPretas);
        if(tempoRestantePretas - tempoPretas <= 0) {
          tempoRestantePretas = tempoRestantePretas - tempoPretas;
          beepNVezes(3, 500);
          ligaLeds(false,false);
          estado = TERMINO;
        }
        else if(digitalRead(buttonPinPreta)==0 && !digitalRead(buttonPinBranca)==0) {
          tempoRestantePretas = tempoRestantePretas - tempoPretas;
          startBrancas = tempoAtual;
          contadorJogadas++;
          estado = BRANCA_JOGA;
          ligaLeds(true,false);
        } else if(digitalRead(buttonPinBranca)==0 && digitalRead(buttonPinPreta)==0) {
          ultimoEstadoAntesDaPausa = PRETA_JOGA;
          tempoRestantePretas = tempoRestantePretas - tempoPretas;
          estado = PAUSA;  
          ligaLeds(true,true);
          beepNVezes(2,150);
          delay(2000);
        }  
        delay(100);  
        break;

    case PAUSA:
      if(digitalRead(buttonPinBranca)==0 && digitalRead(buttonPinPreta)==0) {
        if(ultimoEstadoAntesDaPausa == BRANCA_JOGA) {
           startBrancas = tempoAtual;
           tone(buzzerPin,250,200);
           ligaLeds(true,false);
           estado = BRANCA_JOGA;
        } else if(ultimoEstadoAntesDaPausa == PRETA_JOGA) {
           startPretas = tempoAtual;
           tone(buzzerPin,250,200);
           ligaLeds(false,true);
           estado = PRETA_JOGA;
        }
        delay(500);
      }
      break;
      
    case TERMINO:
      atualizaDisplay(tempoRestanteBrancas,tempoRestantePretas);
      if(digitalRead(buttonPinBranca)==0 && digitalRead(buttonPinPreta)==0) {
        delay(1000);
        if(digitalRead(buttonPinBranca)==0 && digitalRead(buttonPinPreta)==0) {
          reset();
          estado=AGUARDANDO_INICIO;
          beepNVezes(1,200);
          delay(2000);
        }
      }
      delay(200);
      break;
  }


}

void reset() {
  lcd.clear();
  tempoRestanteBrancas = tempoTotal;
  tempoRestantePretas = tempoTotal;
  contadorJogadas=0;
  ultimoEstadoAntesDaPausa = AGUARDANDO_INICIO;
  ligaLeds(false,true);
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

void ligaLeds(boolean branca, boolean preta) {
  if(branca) {
    digitalWrite(ledPinBranca, HIGH);
  } else {
    digitalWrite(ledPinBranca, LOW);
  }

  if(preta) {
    digitalWrite(ledPinPreta, HIGH);
  } else {
    digitalWrite(ledPinPreta, LOW);
  }
  
}

void beepNVezes(int vezes, int tom) {
  for(int i=0;i<vezes;i++) {
      tone(buzzerPin,tom,500);
      delay(1000);
    }
}
