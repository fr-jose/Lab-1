// Parâmetros do termistor (seus valores medidos)
#define TERMISTOR_NOMINAL 11190   // Resistência do termistor a 25°C = 11,19kΩ
#define TEMPERATURA_NOMINAL 25    // Temperatura de referência (25°C)
#define BETA 3950                 // Beta do termistor (ajuste se necessário)
#define RESISTOR_SERIE 11190      // Resistor fixo do divisor de tensão = 11,19kΩ

#include <LiquidCrystal.h>

// Inicializa o LCD com os pinos conectados
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int pinoTermistor = A0;



void setup() {
  Serial.begin(9600);
  pinMode(pinoPeltier, OUTPUT);
  lcd.begin(16, 2);
  
}

void loop() {
  // Mede a temperatura atual
  float temperaturaAtual = lerTemperatura();
  int valorLido = analogRead(A1);
  // Converte o valor lido para tensão
  float tensao = (valorLido * 5.0) / 1023.0;
  float temp_ajustada = tensao*10;
  float temp_ajustada1 = (temp_ajustada*6/24+23.98);
  // Exibe "Hello, World!" na primeira linha
  lcd.print("Atual: ");
  lcd.print(temperaturaAtual);
  lcd.print(" C");
  // Move o cursor para a segunda linha (coluna 0, linha 1)
  lcd.setCursor(0, 1);
  // Exibe "Arduino LCD!" na segunda linha
  lcd.print("Desejada: ");
  lcd.print(temp_ajustada1);
  lcd.print(" C");

  delay(500); // Intervalo entre leituras
}

// Função para ler a temperatura do termistor
float lerTemperatura() {
  int leitura = analogRead(pinoTermistor);
  float tensao = leitura * (5.0 / 1023.0);
  float resistencia = RESISTOR_SERIE * (5.0 / tensao - 1.0);

  // Equação Beta para converter resistência em temperatura
  float temperatura = resistencia / TERMISTOR_NOMINAL;
  temperatura = log(temperatura);
  temperatura /= BETA;
  temperatura += 1.0 / (TEMPERATURA_NOMINAL + 273.15);
  temperatura = 1.0 / temperatura;
  temperatura -= 273.15;

  return temperatura;
}