#include <IRremote.h>
#include <LiquidCrystal.h>
#include <math.h>

// Definições dos pinos
#define PWM_SAIDA 10   // Pino PWM para controle de potência
#define RECEPTOR_IR 7   // Pino do receptor IR
#define TERMISTOR_NOMINAL 11190   // Resistência do termistor a 25°C
#define TEMPERATURA_NOMINAL 25    // Temperatura de referência (25°C)
#define BETA 3950                 // Beta do termistor
#define RESISTOR_SERIE 11190      // Resistor fixo do divisor de tensão

// Constantes do PID
const float KP = 60;
const float KI = 0;
const float KD = 0.0;
float integral = 0.0;
float erroAnterior = 0.0;
unsigned long tempoAnterior = 0;

// Temperaturas desejada e real
float temperaturaDesejada = 30.0; 

// Inicialização do LCD e do sensor IR
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
IRrecv irrecv(RECEPTOR_IR);
decode_results results;

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);
    lcd.print("Temp. Atual:");
    
    pinMode(PWM_SAIDA, OUTPUT);
    pinMode(A5, OUTPUT);  // Configura A5 como saída
    digitalWrite(A5, HIGH); // Mantém A5 sempre ligado

    irrecv.enableIRIn();
    
    // Cabeçalho CSV
    Serial.println("Tempo (s),Temp Atual (C),Temp Desejada (C),PWM");
}

void loop() {
    float temperaturaAtual = lerTemperatura();

    // Controle via IR
    if (irrecv.decode(&results)) {
        if (results.value == 0xFF906F && temperaturaDesejada < 35.0) {
            temperaturaDesejada += 0.1;
        } else if (results.value == 0xFFE01F && temperaturaDesejada > 24.0) {
            temperaturaDesejada -= 0.1;
        }
        irrecv.resume();
    }
    
    // Atualiza o LCD
    lcd.setCursor(0, 1);
    lcd.print("Desejada: ");
    lcd.print(temperaturaDesejada, 2);
    lcd.print(" C  ");

    lcd.setCursor(0, 0);
    lcd.print("Atual:    ");
    lcd.print(temperaturaAtual, 2);
    lcd.print(" C  ");

    // Controle PID
    float erro = temperaturaDesejada - temperaturaAtual;
    unsigned long tempoAtual = millis();
    float deltaTempo = (tempoAtual - tempoAnterior) / 1000.0;

    float proporcional = KP * erro;
    integral += erro * deltaTempo;
    float derivativo = KD * (erro - erroAnterior) / deltaTempo;

    // Evita integral excessiva
    if (integral > 10000.0) integral = 10000.0;
    if (integral < -10000.0) integral = -10000.0;

    int saidaPID = (int)(proporcional + (KI * integral) + derivativo);
    
    if (saidaPID > 255) saidaPID = 255;
    if (saidaPID < 0) saidaPID = 0;
    
    analogWrite(PWM_SAIDA, saidaPID);

    // Atualiza valores anteriores
    erroAnterior = erro;
    tempoAnterior = tempoAtual;

    // Saída no formato CSV
    Serial.print(tempoAtual / 1000.0, 3);
    Serial.print(",");
    Serial.print(temperaturaAtual, 2);
    Serial.print(",");
    Serial.print(temperaturaDesejada, 2);
    Serial.print(",");
    Serial.println(saidaPID);
    
    delay(250);
}

float lerTemperatura() {
    int leitura = analogRead(A0);
    float tensao = leitura * (5.0 / 1023.0);
    float resistencia = RESISTOR_SERIE * (5.0 / tensao - 1.0);

    float temperatura = log(resistencia / TERMISTOR_NOMINAL) / BETA;
    temperatura += 1.0 / (TEMPERATURA_NOMINAL + 273.15);
    temperatura = 1.0 / temperatura - 273.15;

    return temperatura;
}

