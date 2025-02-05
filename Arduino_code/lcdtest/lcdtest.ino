#include <LiquidCrystal.h>

// Inicializa o LCD com os pinos conectados
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // Configura o LCD para 16 colunas e 2 linhas
  lcd.begin(16, 2);
  
  // Exibe "Hello, World!" na primeira linha
  lcd.print("E oficial!");
  
  // Move o cursor para a segunda linha (coluna 0, linha 1)
  lcd.setCursor(0, 1);
  
  // Exibe "Arduino LCD!" na segunda linha
  lcd.print("Lab de Fisica 1!");
}

void loop() {
  // Nada a fazer aqui
}
