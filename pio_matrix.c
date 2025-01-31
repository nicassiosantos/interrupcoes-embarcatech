#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

// arquivo .pio
#include "pio_matrix.pio.h"

#define NUM_PIXELS 25


#define OUT_PIN 7 // pino do led 5x5
#define LED_R 13 // pino led vermelho
#define LED_G 11 // pino led verde
#define LED_B 12 // pino led azul 
#define button_A 5 // pino do botão A

//Variavél global
static volatile uint cont = 0;
static volatile uint32_t last_time = 0;

// FUNÇÕES DAS ANIMAÇÕES
extern void animacao_olho(PIO pio, uint sm);
extern void coracao_batendo(uint32_t valor_led, PIO pio, uint sm, int repeticoes, int delay_ms);
extern void tetrix(PIO pio, uint sm);
extern void seta(PIO pio, uint sm);
extern void carinha(PIO pio, uint sm);
extern void cobra_animacao(uint32_t valor_led, PIO pio, uint sm, int repeticoes, int delay_ms);

extern void espiral_expansiva(PIO pio, uint sm);

static void gpio_irq_handler(uint gpio, uint32_t events);

// rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// rotina para acionar a matrix de leds - ws2812b
void desenho_pio(uint32_t valor_led, PIO pio, uint sm, char caracter_press)
{
    if (caracter_press == 'A')
    {
        for (int16_t i = 0; i < NUM_PIXELS; i++)
        {
            valor_led = matrix_rgb(0.0, 0.0, 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }

    else if (caracter_press == 'B') // Novo caso para tecla 'B'
    {

        for (int16_t i = 0; i < NUM_PIXELS; i++)
        {
            valor_led = matrix_rgb(1.0, 0.0, 0.0); // LEDs na cor azul com 100% de intensidade
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }

    else if (caracter_press == 'C') // Caso para tecla 'C'
    {
        for (int16_t i = 0; i < NUM_PIXELS; i++)
        {
            valor_led = matrix_rgb(0.0, 0.8, 0.0); // LEDs na cor verde com 80% de intensidade
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }

    else if (caracter_press == 'D')
    {
        for (int16_t i = 0; i < NUM_PIXELS; i++)
        {
            valor_led = matrix_rgb(0.0, 0.0, 0.5);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }

    else if (caracter_press == '#')
    {
        for (int16_t i = 0; i < NUM_PIXELS; i++)
        {
            valor_led = matrix_rgb(0.2, 0.2, 0.2);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }
}

// Função para reboot
void reboot()
{
    reset_usb_boot(0, 0);
}

/// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("valor do contador = %d\n", cont);
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000 && cont < 9) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento
        cont++;                                     // incrementa a variavel de verificação
    }
}



int main()
{
    PIO pio = pio0;
    uint32_t valor_led;

    //Inicialização dos pinos 
    gpio_init(LED_R);              // Inicializa o pino do LED
    gpio_set_dir(LED_R, GPIO_OUT); // Configura o pino como saída
    gpio_put(LED_R, 0);            // Garante que o LED inicie apagado

    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN); 
    gpio_pull_up(button_A);          

    stdio_init_all();

    // configurações da PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true)
    {
        // LED piscando
        gpio_put(LED_R, 1); 
        sleep_ms(200);            
        gpio_put(LED_R, 0); 
        sleep_ms(200);   
                     
    }
}