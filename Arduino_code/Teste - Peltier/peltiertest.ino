const int pinoPeltier = 9; // Pino PWM conectado ao Peltier

void setup() {
  pinMode(pinoPeltier, OUTPUT);
  Serial.begin(9600);
  Serial.println("Teste do Peltier - Digite um valor de PWM (0 a 255):");
}

void loop() {
  // Verifica se há dados disponíveis na Serial
  if (Serial.available() > 0) {
    // Lê o valor digitado pelo usuário
    int pwm = Serial.parseInt(); // Lê um número inteiro

    // Limita o valor de PWM entre 0 e 255
    pwm = constrain(pwm, 0, 255);

    // Aplica o valor de PWM ao Peltier
    analogWrite(pinoPeltier, pwm);

    // Exibe o valor aplicado
    Serial.print("PWM aplicado: ");
    Serial.println(pwm);
  }
}