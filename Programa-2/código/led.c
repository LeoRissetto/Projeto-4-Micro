#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

// Definições de pinos para o LED RGB
#define LED_R_PIN 15 // Pino do LED vermelho
#define LED_G_PIN 2  // Pino do LED verde
#define LED_B_PIN 4  // Pino do LED azul

// Configurações do PWM
#define LEDC_FREQUENCY 5000              // Frequência do PWM em Hz
#define LEDC_RESOLUTION LEDC_TIMER_8_BIT // Resolução de 8 bits (256 níveis)
#define LEDC_SPEED LEDC_HIGH_SPEED_MODE  // Alta velocidade para controle suave

// Função para inicializar o PWM e configurar os canais para as cores do LED
void ledc_init()
{
    // Configuração do canal para o LED vermelho (R)
    ledc_channel_config_t ledc_channel_r = {
        .gpio_num = LED_R_PIN,
        .speed_mode = LEDC_SPEED,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0, // Duty inicial
        .hpoint = 0};

    // Configuração do canal para o LED verde (G)
    ledc_channel_config_t ledc_channel_g = {
        .gpio_num = LED_G_PIN,
        .speed_mode = LEDC_SPEED,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0, // Duty inicial
        .hpoint = 0};

    // Configuração do canal para o LED azul (B)
    ledc_channel_config_t ledc_channel_b = {
        .gpio_num = LED_B_PIN,
        .speed_mode = LEDC_SPEED,
        .channel = LEDC_CHANNEL_2,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0, // Duty inicial
        .hpoint = 0};

    // Configuração do Timer PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_SPEED,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_RESOLUTION,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    // Inicializando o timer PWM
    ledc_timer_config(&ledc_timer);

    // Inicializando os canais PWM
    ledc_channel_config(&ledc_channel_r);
    ledc_channel_config(&ledc_channel_g);
    ledc_channel_config(&ledc_channel_b);
}

// Função para ajustar o PWM e definir o brilho das cores do LED
void set_pwm(int increment)
{
    // Calculando o duty cycle para cada cor com base no incremento
    int red_duty = increment * 2;  // R = incremento * 2
    int green_duty = increment;    // G = incremento
    int blue_duty = increment * 3; // B = incremento * 3

    // Garantindo que o valor do duty não ultrapasse 255
    if (red_duty > 255)
        red_duty = 255;
    if (green_duty > 255)
        green_duty = 255;
    if (blue_duty > 255)
        blue_duty = 255;

    // Aplicando o duty cycle nas cores do LED
    ledc_set_duty(LEDC_SPEED, LEDC_CHANNEL_0, red_duty);
    ledc_set_duty(LEDC_SPEED, LEDC_CHANNEL_1, green_duty);
    ledc_set_duty(LEDC_SPEED, LEDC_CHANNEL_2, blue_duty);

    // Atualizando o PWM para as cores
    ledc_update_duty(LEDC_SPEED, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_SPEED, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_SPEED, LEDC_CHANNEL_2);
}

// Função para exibir os valores do PWM no Monitor Serial
void print_pwm_values(int increment)
{
    ESP_LOGI("PWM", "Incremento: %d, Duty R: %d, Duty G: %d, Duty B: %d",
             increment, increment * 2, increment, increment * 3);
}

// Função principal
void app_main()
{
    int increment = 0;

    // Inicializando a configuração do PWM para o LED RGB
    ledc_init();

    while (1)
    {
        // Ajustando o PWM com base no incremento
        set_pwm(increment);

        // Exibindo os valores do PWM no monitor serial
        print_pwm_values(increment);

        // Incrementando o valor de 5 em 5 até 255 (máximo)
        increment = (increment + 5) % 256;

        // Atraso de 100ms antes de atualizar os valores
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay de 100ms
    }
}
