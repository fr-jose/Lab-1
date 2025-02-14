#include <IRremote.h>
#include <LiquidCrystal.h>
#include <math.h>

#define PWM_SAIDA 10
#define RECEPTOR_IR 7
#define TERMISTOR_NOMINAL 11190
#define TEMPERATURA_NOMINAL 25
#define BETA 3950
#define RESISTOR_SERIE 11190

const float KP = 60;
const float KI = 0;
const float KD = 0.0;
float integral = 0.0;
float erroAnterior = 0.0;
unsigned long tempoAnterior = 0;

float temperaturaDesejada = 30.0;
float temperaturaAtual = 0.0;
bool sistemaLigado = false;
bool telaLigada = false;
bool pwmLigado = false;
int pwmValor = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
IRrecv irrecv(RECEPTOR_IR);
decode_results results;

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);
    pinMode(PWM_SAIDA, OUTPUT);
    pinMode(A5, OUTPUT);
    digitalWrite(A5, LOW);
    irrecv.enableIRIn();
    atualizarLCD();
    Serial.println("Tempo (s),Temp Atual (C),Temp Desejada (C),PWM");
}

void loop() {
    temperaturaAtual = lerTemperatura();

    if (irrecv.decode(&results)) {
        processarComando(results.value);
        irrecv.resume();
    }

    if (sistemaLigado) {
        atualizarPID();
    } else {
        analogWrite(PWM_SAIDA, 0);
        pwmValor = 0;
    }

    atualizarLCD();
    logSerial();
    delay(100);
}

void atualizarPID() {
    float erro = temperaturaDesejada - temperaturaAtual;
    unsigned long tempoAtual = millis();
    float deltaTempo = (tempoAtual - tempoAnterior) / 1000.0;

    float proporcional = KP * erro;
    integral += KI * erro * deltaTempo;
    float derivativo = KD * (erro - erroAnterior) / deltaTempo;

    integral = constrain(integral, -10000.0, 10000.0);

    pwmValor = (int)(proporcional + integral + derivativo);
    if (!pwmLigado) pwmValor = 0;
    pwmValor = constrain(pwmValor, 0, 255);
    analogWrite(PWM_SAIDA, pwmValor);

    erroAnterior = erro;
    tempoAnterior = tempoAtual;
}

void processarComando(unsigned long comando) {
    switch(comando) {
        case 0xFFA25D: // Liga/Desliga Sistema
            telaLigada = !telaLigada;
            digitalWrite(A5, !telaLigada ? HIGH : LOW);
            if(!telaLigada) pwmLigado = false;
            if(!telaLigada) sistemaLigado = false;
            break;
            
        case 0xFFE21D: // Alterna estado do sistema (mesma função do A25D)
            sistemaLigado = !sistemaLigado;
            
            // Controle automático do PWM
            if(sistemaLigado) {
                pwmLigado = true;  // Liga PWM ao ligar sistema
            } else {
                pwmLigado = false; // Desliga PWM ao desligar sistema
                pwmValor = 0;
                analogWrite(PWM_SAIDA, 0);
            }
            break;
            
        case 0xFF18E7: // -1.00 (Botão 2)
            ajustarTemperatura(-1.00);
            break;
            
        case 0xFF7A85: // +1.00 (Botão 3)
            ajustarTemperatura(+1.00);
            break;
            
        case 0xFF38C7: // -0.10 (Botão 5)
            ajustarTemperatura(-0.10);
            break;
            
        case 0xFF5AA5: // +0.10 (Botão 6)
            ajustarTemperatura(+0.10);
            break;
            
        case 0xFF4AB5: // -0.01 (Botão 8)
            ajustarTemperatura(-0.01);
            break;
            
        case 0xFF52AD: // +0.01 (Botão 9)
            ajustarTemperatura(+0.01);
            break;
    }
}

void ajustarTemperatura(float delta) {
    float novaTemp = temperaturaDesejada + delta;
    temperaturaDesejada = constrain(novaTemp, 27.0, 35.0);
}

void atualizarLCD() {
    lcd.clear();
    
    // Linha 1: NowXX.XX PWMXXX
    lcd.setCursor(0, 0);
    lcd.print("Now ");
    lcd.print(temperaturaAtual, 2);
    lcd.print(" PWM");
    
    // Ajusta a posição do PWM conforme o número de dígitos
    if(pwmValor < 10) {
        lcd.setCursor(15, 0); // 1 dígito: posição 12
        lcd.print(pwmValor);
        lcd.print("  "); // Limpa possíveis dígitos anteriores
    } else if(pwmValor < 100) {
        lcd.setCursor(14, 0); // 2 dígitos: posição 11
        lcd.print(pwmValor);
        lcd.print(" "); // Limpa possível dígito anterior
    } else {
        lcd.setCursor(13, 0); // 3 dígitos: posição 10
        lcd.print(pwmValor);
    }
    
    // Linha 2: WantXX.XX ON/OFF
    lcd.setCursor(0, 1);
    lcd.print("Goal ");
    lcd.print(temperaturaDesejada, 2);
    lcd.print("   ");
    lcd.print(sistemaLigado ? " ON" : "OFF");
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

void logSerial() {
    static unsigned long ultimoLog = 0;
    if(millis() - ultimoLog >= 1000) {
        Serial.print(millis()/1000.0, 2);
        Serial.print(",");
        Serial.print(temperaturaAtual, 2);
        Serial.print(",");
        Serial.print(temperaturaDesejada, 2);
        Serial.print(",");
        Serial.println(pwmValor);
        ultimoLog = millis();
    }
}
