// Parâmetros do termistor (seus valores medidos)
#define TERMISTOR_NOMINAL 11190   // Resistência do termistor a 25°C = 11,19kΩ
#define TEMPERATURA_NOMINAL 25    // Temperatura de referência (25°C)
#define BETA 3950                 // Beta do termistor (ajuste se necessário)
#define RESISTOR_SERIE 11190      // Resistor fixo do divisor de tensão = 11,19kΩ

const int pinoTermistor = A0;
const int pinoPeltier = 9;        // Pino PWM para controlar o Peltier

float temperaturaDesejada = 40.0; // Setpoint (27°C)
float Kp = 50.0;                  // Ganho proporcional (ajuste empiricamente)

void setup() {
  Serial.begin(9600);
  pinMode(pinoPeltier, OUTPUT);
}

void loop() {
  // Mede a temperatura atual
  float temperaturaAtual = lerTemperatura();

  // Calcula o erro (diferença para o setpoint)
  float erro = temperaturaDesejada - temperaturaAtual;

  // Controle Proporcional (P)
  int sinalControle = constrain(Kp * erro, 0, 255); // Limita entre 0 e 255 (PWM)

  // Aplica o sinal ao Peltier
  analogWrite(pinoPeltier, sinalControle);

  // Exibe dados no serial
  Serial.print("Temperatura: ");
  Serial.print(temperaturaAtual);
  Serial.print(" °C | PWM: ");
  Serial.println(sinalControle);

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