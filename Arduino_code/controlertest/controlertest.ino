#include <IRremote.h>

#define RECEPTOR_IR 7  // Pino do receptor IR (pode mudar dependendo de onde você conectar)

IRrecv irrecv(RECEPTOR_IR);  // Objeto do receptor IR
decode_results results;

void setup() {
    Serial.begin(115200);       // Inicializa a comunicação serial
    irrecv.enableIRIn();        // Inicializa o receptor IR
    Serial.println("Aguardando sinais IR...");
}

void loop() {
    if (irrecv.decode(&results)) {  // Se um sinal IR for detectado
        Serial.print("Código recebido: 0x");
        Serial.println(results.value, HEX); // Exibe o código do botão pressionado
        irrecv.resume();                 // Prepara para o próximo sinal
    }
    delay(200);
}
