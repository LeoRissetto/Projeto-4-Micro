#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// Configurações do Display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pinos
#define SERVO_PIN 4
#define BUTTON_LOOP 13
#define BUTTON_MANUAL 12
#define BUTTON_STOP 14
#define POTENTIOMETER_PIN 34

// Servo
Servo servo;
int angle = 0;         // Ângulo atual do servo
bool autoMode = false; // Modo automático ou manual
bool systemOn = true;  // Sistema ligado ou desligado
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200; // 200 ms de debounce

// Variáveis para o debounce
bool lastLoopButton = false, lastManualButton = false, lastStopButton = false;

// Funções
void updateDisplay(String mode, int angle);
bool detectButtonPress(int buttonPin, bool &lastState); // Verifica os botões

// Configurações iniciais
void setup()
{
    Serial.begin(115200);

    // Inicialização do Display OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("Falha ao inicializar o display OLED!");
        while (true)
            ; // Trava o sistema se o display falhar
    }
    display.clearDisplay();
    display.display();

    // Configuração do servo
    servo.attach(SERVO_PIN, 500, 2500); // Sinal PWM de 50Hz (500 a 2500 µs)

    // Configuração dos botões
    pinMode(BUTTON_LOOP, INPUT_PULLUP);
    pinMode(BUTTON_MANUAL, INPUT_PULLUP);
    pinMode(BUTTON_STOP, INPUT_PULLUP);

    // Mensagem inicial
    updateDisplay("Sistema Pronto", 0);
}

void loop()
{
    // Leitura dos botões com debounce
    if (detectButtonPress(BUTTON_LOOP, lastLoopButton))
    {
        autoMode = true;
        systemOn = true;
    }
    else if (detectButtonPress(BUTTON_MANUAL, lastManualButton))
    {
        autoMode = false;
        systemOn = true;
    }
    else if (detectButtonPress(BUTTON_STOP, lastStopButton))
    {
        systemOn = false;
        updateDisplay("Sistema Parado", angle);
    }

    // Executa apenas se o sistema estiver ligado
    if (systemOn)
    {
        if (autoMode)
        {
            // Modo Automático: movimenta de 0° a 180° e volta
            autoModeLoop();
        }
        else
        {
            // Modo Manual
            int potValue = analogRead(POTENTIOMETER_PIN); // Lê o potenciômetro
            angle = map(potValue, 0, 4095, 0, 180);       // Mapeia o valor para o ângulo
            servo.write(angle);
            updateDisplay("Modo Manual", angle);
        }
    }
}

// Loop do Modo Automático
unsigned long lastServoMoveTime = 0;
int direction = 1; // 1 para subir, -1 para descer

void autoModeLoop()
{
    if (millis() - lastServoMoveTime >= 50) // Controla o tempo entre os movimentos
    {
        angle += direction;
        if (angle >= 180 || angle <= 0)
        {
            direction = -direction; // Inverte direção
        }
        servo.write(angle);
        updateDisplay("Modo Automatico", angle);
        lastServoMoveTime = millis();
    }
}

// Verifica a transição de estado do botão (pressionado/despressionado)
bool detectButtonPress(int buttonPin, bool &lastState)
{
    bool currentState = digitalRead(buttonPin) == LOW;
    if (currentState != lastState)
    {
        delay(50);                         // Pequeno delay para debounce
        if (digitalRead(buttonPin) == LOW) // Verifica se ainda está pressionado
        {
            lastState = true;
            return true; // Botão foi pressionado
        }
    }
    lastState = currentState;
    return false; // Não houve mudança no estado do botão
}

// Atualiza o display OLED
void updateDisplay(String mode, int angle)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Controle Servo Motor");
    display.println("Modo: " + mode);
    display.println("Angulo: " + String(angle) + " graus");
    display.display();
}
