#include <LiquidCrystal.h>

// Define os pinos do LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
    lcd.begin(16, 2); // Inicializa LCD 16x2
    lcd.print("Hello, Arduino!"); // Escreve no LCD
}

void loop() {
    lcd.setCursor(0, 1); // Move para segunda linha
    lcd.print(millis() / 1000); // Mostra o tempo em segundos
    delay(1000); // Atualiza a cada segundo
}
