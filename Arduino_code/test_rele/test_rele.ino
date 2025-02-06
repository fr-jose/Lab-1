int pinoRele = 7; // Pino conectado ao IN do módulo relé

void setup() {
  pinMode(pinoRele, OUTPUT); // Define o pino como saída
}

void loop() {
  digitalWrite(pinoRele, HIGH); // Liga o relé (resistor aquece)                // Mantém desligado por 5 segundos
}