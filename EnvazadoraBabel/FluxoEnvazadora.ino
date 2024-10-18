// Definição dos pinos
const int pistonPin = 2;             // Pistão
const int openBeerPin = 3;           // Abre-cerveja
const int closeBeerPin = 4;          // Fecha-cerveja
const int enterCO2Pin = 5;           // Entra-CO2
const int exitCO2Pin = 6;            // Saída-CO2
const int startButtonPin = A0;       // Botão de start
const int pistonUpSensorPin = A1;    // Sensor pistão levantado
const int pistonDownSensorPin = A2;  // Sensor pistão recolhido
const int emergencyButtonPin = A3;   // Botão de emergência

// Definição dos estados
enum States { INICIO, PREPARA_GARRAFA, ENCHER_GARRAFA, TERMINA_CICLO, AGUARDA_FIM };
States currentState = INICIO;

void setup() {
  // Configuração dos pinos de saída
  pinMode(pistonPin, OUTPUT);
  pinMode(openBeerPin, OUTPUT);
  pinMode(closeBeerPin, OUTPUT);
  pinMode(enterCO2Pin, OUTPUT);
  pinMode(exitCO2Pin, OUTPUT);

  // Configuração dos pinos de entrada com INPUT_PULLUP
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(pistonUpSensorPin, INPUT_PULLUP);
  pinMode(pistonDownSensorPin, INPUT_PULLUP);
  pinMode(emergencyButtonPin, INPUT_PULLUP); // Botão de emergência
 
  // Inicializa todos os dispositivos desligados
  stopAll();

  // Inicializa a comunicação serial
  Serial.begin(9600);
}

void loop() {
  // Leitura dos sensores e do botão (com INPUT_PULLUP, o estado LOW indica ativado)
  int startButtonState = digitalRead(startButtonPin);
  int pistonUpState = digitalRead(pistonUpSensorPin);
  int pistonDownState = digitalRead(pistonDownSensorPin);
  int emergencyButtonState = digitalRead(emergencyButtonPin);

  // Verifica se o botão de emergência foi pressionado - enquanto estiver pressionado ficará em looping aqui:
  if (emergencyButtonState == LOW) {
    stopAll();
    Serial.println("Emergência ativada! Processo cancelado!!! ");
    delay(1000);
    currentState = INICIO;
    return;
  }

  delay(100);

  switch (currentState) {
    case INICIO:
      stopAll();
      if (startButtonState == LOW) { // Botão pressionado
        Serial.println("Estado: Prepara-Garrafa");
        currentState = PREPARA_GARRAFA;
        digitalWrite(pistonPin, HIGH); // Aciona pistão
      } else {
        delay(500);
      }
      break;

    case PREPARA_GARRAFA:
      // Aguarda o pistão levantar
      if (pistonUpState == LOW) {
        Serial.println("Pistão levantado, aguardando 500ms");
        delay(500); 
        Serial.println("Abrindo Entra-CO2");
        digitalWrite(enterCO2Pin, HIGH); // Abre Entra-CO2
        delay(3000);                     
        digitalWrite(enterCO2Pin, LOW);  // Fecha Entra-CO2

        Serial.println("Garrafa pressurizada, indo para Encher-Garrafa");
        currentState = ENCHER_GARRAFA;
      } else {
        delay(500);
      }
      break;

    case ENCHER_GARRAFA:
      // Aciona os dispositivos para encher a garrafa
      digitalWrite(openBeerPin, HIGH);   // Abre cerveja
      delay(200);
      digitalWrite(closeBeerPin, LOW);   // Fecha-cerveja desligado
      delay(200);
      digitalWrite(exitCO2Pin, HIGH);    // Aciona Saída-CO2
      delay(25000);                      // Aguarda 25 segundos - AQUI TEM O PROBLEMA DO EFEITO BLOQUEANTE, EMERGÊNCIA NÃO OPERA!

      Serial.println("Terminando enchimento, indo para Termina-Ciclo");
      currentState = TERMINA_CICLO;
      break;

    case TERMINA_CICLO:
      // Desliga os dispositivos e encerra o ciclo
      digitalWrite(openBeerPin, LOW);    // Desliga Abre-Cerveja
      digitalWrite(exitCO2Pin, LOW);     // Desliga Saída-CO2
      delay(250);

      // TODO: REVISAR!!!  AQUI FIQUEI MEIO EM DÚVIDA, FAVOR ATENÇÃO MAIOR:
      Serial.println("Fechando cerveja");
      digitalWrite(closeBeerPin, HIGH);  // Aciona Fecha-Cerveja
      delay(250);                        // Aguarda 250ms

      Serial.println("Desacionando Fecha-Cerveja e Pistão");
      digitalWrite(closeBeerPin, LOW);   // Desaciona Fecha-Cerveja // <- ??????
      digitalWrite(pistonPin, LOW);      // Desaciona Pistão
      delay(500);                
      Serial.println("Ciclo terminou, indo para Aguarda-Fim");        // Aguarda 500ms
      currentState = AGUARDA_FIM;
      break;

      case AGUARDA_FIM:
      // Aguarda o pistão recolhido
      if (pistonDownState == LOW) {
        Serial.println("Pistão recolhido, retornando ao estado Início");
        currentState = INICIO;           // Volta ao estado inicial
      } else {
        delay(500);
      }
      break;
    
  }
}
  // Função que desliga todos os componentes
  void stopAll() {
    digitalWrite(pistonPin, LOW);    // Desliga Pistão
    digitalWrite(openBeerPin, LOW);  // Desliga Abre-Cerveja
    digitalWrite(closeBeerPin, LOW); // Desliga Fecha-Cerveja
    digitalWrite(enterCO2Pin, LOW);  // Desliga Entra-CO2
    digitalWrite(exitCO2Pin, LOW);   // Desliga Saída-CO2
  }

  

