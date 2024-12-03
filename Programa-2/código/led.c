#include <Arduino.h>
#include "driver/ledc.h"

// Definições de pinos e configurações PWM
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_RESOLUTION LEDC_TIMER_8_BIT // 8-bit resolution
#define LEDC_FREQUENCY 5000              // 5 kHz frequency

#define LEDC_CHANNEL_RED LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN LEDC_CHANNEL_1
#define LEDC_CHANNEL_BLUE LEDC_CHANNEL_2

#define GPIO_RED 25
#define GPIO_GREEN 26
#define GPIO_BLUE 27

void init_ledc()
{
    // Configurar o timer PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_RESOLUTION,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    // Configurar canais PWM para R, G e B
    ledc_channel_config_t ledc_channel[3] = {
        {.gpio_num = GPIO_RED,
         .speed_mode = LEDC_MODE,
         .channel = LEDC_CHANNEL_RED,
         .timer_sel = LEDC_TIMER,
         .duty = 0,
         .hpoint = 0},
        {.gpio_num = GPIO_GREEN,
         .speed_mode = LEDC_MODE,
         .channel = LEDC_CHANNEL_GREEN,
         .timer_sel = LEDC_TIMER,
         .duty = 0,
         .hpoint = 0},
        {.gpio_num = GPIO_BLUE,
         .speed_mode = LEDC_MODE,
         .channel = LEDC_CHANNEL_BLUE,
         .timer_sel = LEDC_TIMER,
         .duty = 0,
         .hpoint = 0},
    };

    // Aplicar configurações nos canais
    for (int ch = 0; ch < 3; ch++)
    {
        ledc_channel_config(&ledc_channel[ch]);
    }
}

void ledc_fade()
{
    int increment = 5;
    int red_duty = 0; // Inicializa a variável
    int green_duty = 0;
    int blue_duty = 0;

    while (1)
    {
        // Calcular o valor de duty para R, G e B, com o limite de 255
        red_duty += increment * 2;
        green_duty += increment;
        blue_duty += increment * 3;

        // Verificar se qualquer cor atingiu 255 e reiniciar para 0
        if (red_duty > 255)
            red_duty = 0;
        if (green_duty > 255)
            green_duty = 0;
        if (blue_duty > 255)
            blue_duty = 0;

        // Aplicar os valores de duty nos canais PWM
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, red_duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, green_duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, blue_duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE);

        // Exibir a mensagem no Monitor Serial
        Serial.print("Incremento: ");
        Serial.print(increment);
        Serial.print(" | Duty Cycle (R, G, B): ");
        Serial.print(red_duty);
        Serial.print(", ");
        Serial.print(green_duty);
        Serial.print(", ");
        Serial.println(blue_duty);

        delay(100); // Atraso para variação gradual
    }
}

void setup()
{
    Serial.begin(115200); // Inicializa a comunicação serial
    init_ledc();          // Inicializa o LEDC
}

void loop()
{
    ledc_fade(); // Executa o efeito de fade
}
