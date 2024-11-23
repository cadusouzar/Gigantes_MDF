/**
 *@file 	 GiganteMDF_Codigo.ino
 *@brief	 Projeto da matéria Programação de Hardware de engenharia de computação da Universidade Santa Cecília
 *@author Carlos Eduardo de Souza Ribeiro
 *@see https://github.com/cadusouzar/Gigantes_MDF
 *@date 21/11/2024
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define MOTOR_ESQ_FRENTE PD2
#define MOTOR_ESQ_TRAS   PD3
#define MOTOR_DIR_FRENTE PD4
#define MOTOR_DIR_TRAS   PD7
#define MOTOR_PWM_ESQ    PD5
#define MOTOR_PWM_DIR    PD6
#define LED1 PC5
#define LED2 PC3
#define LED3 PC2
#define DIODO_LASER PC1
#define LDR PC0
#define BOTAO_FRENTE PB0
#define BOTAO_DIR_FRENTE PB1
#define BOTAO_ESQ_FRENTE PB2
#define BOTAO_RE PB3
#define BOTAO_LIGAR_LEDS PB4

uint8_t modoRe = 0;  // Para alternar entre frente e ré (0 = frente, 1 = ré)
uint8_t ledsAtivos = 3; // Quantidade de LEDs ligados
volatile uint8_t laserEstado = 0; // Estado do laser (0 = desligado, 1 = ligado)
uint8_t motorParado = 0;  // Flag para indicar se o motor está parado devido ao LDR
uint8_t giroRealizado = 0; // Flag para indicar que o giro já foi realizado
uint8_t reiniciarLEDs = 0; // Flag para reiniciar LEDs

void setup();
void moverMotores(uint8_t re, uint8_t moverEsq, uint8_t moverDir);
void pararMotores();
void controleLaser();
void controleLEDs();
void initTimer1();
void printLDRValue(uint16_t value);
void giro180();

// Interrupção do Timer1 (1 segundo)
ISR(TIMER1_COMPA_vect) {
    laserEstado = !laserEstado; // Alterna o estado do laser
    if (laserEstado) {
        PORTC |= (1 << DIODO_LASER); // Liga o laser
    } else {
        PORTC &= ~(1 << DIODO_LASER); // Desliga o laser
    }
}

int main(void) {
    setup();

    while (1) {
        // Controle dos LEDs baseado no LDR
        controleLEDs();

        // Controle dos motores
        if (!(PINB & (1 << BOTAO_RE))) {
            modoRe = !modoRe; // Alterna entre frente e ré
            _delay_ms(200);   // Debounce
        }

        if (!(PINB & (1 << BOTAO_FRENTE))) {
            if (!motorParado) moverMotores(modoRe, 1, 1); // Ambos os motores
        } else if (!(PINB & (1 << BOTAO_DIR_FRENTE))) {
            if (!motorParado) moverMotores(modoRe, 0, 1); // Apenas motor direito
        } else if (!(PINB & (1 << BOTAO_ESQ_FRENTE))) {
            if (!motorParado) moverMotores(modoRe, 1, 0); // Apenas motor esquerdo
        } else {
            pararMotores(); // Para os motores
        }

        // Se o botão PB4 for pressionado e todos os LEDs estiverem desligados, reinicia os LEDs
        if (!(PINB & (1 << BOTAO_LIGAR_LEDS)) && ledsAtivos == 0) {
            reiniciarLEDs = 1; // Ativa a flag para reiniciar os LEDs
            _delay_ms(200); // Debounce do botão
        }

        if (reiniciarLEDs) {
            // Liga todos os LEDs
            PORTC |= (1 << LED1) | (1 << LED2) | (1 << LED3);
            ledsAtivos = 3; // Atualiza a quantidade de LEDs ligados
            reiniciarLEDs = 0; // Reseta a flag de reinício
        }
    }
}

void setup() {
    // Configura os pinos dos motores como saída
    DDRD |= (1 << MOTOR_ESQ_FRENTE) | (1 << MOTOR_ESQ_TRAS) |
            (1 << MOTOR_DIR_FRENTE) | (1 << MOTOR_DIR_TRAS) |
            (1 << MOTOR_PWM_ESQ) | (1 << MOTOR_PWM_DIR);

    // Configura os LEDs como saída e inicializa ligados
    DDRC |= (1 << LED1) | (1 << LED2) | (1 << LED3);
    PORTC |= (1 << LED1) | (1 << LED2) | (1 << LED3);

    // Configura o diodo laser como saída e inicializa desligado
    DDRC |= (1 << DIODO_LASER);
    PORTC &= ~(1 << DIODO_LASER);

    // Configura o LDR como entrada (ADC)
    DDRC &= ~(1 << LDR);

    // Configura os botões como entrada com pull-up
    DDRB &= ~((1 << BOTAO_FRENTE) | (1 << BOTAO_DIR_FRENTE) |
              (1 << BOTAO_ESQ_FRENTE) | (1 << BOTAO_RE) | (1 << BOTAO_LIGAR_LEDS));
    PORTB |= (1 << BOTAO_FRENTE) | (1 << BOTAO_DIR_FRENTE) |
             (1 << BOTAO_ESQ_FRENTE) | (1 << BOTAO_RE) | (1 << BOTAO_LIGAR_LEDS);

    // Configura PWM nos pinos dos motores
    TCCR0A |= (1 << WGM00) | (1 << WGM01); // Modo Fast PWM
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // PWM não invertido
    TCCR0B |= (1 << CS01); // Prescaler de 8
    OCR0A = 255; // PWM máximo para motor direito
    OCR0B = 255; // PWM máximo para motor esquerdo

    // Configura ADC
    ADMUX = (1 << REFS0); 
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Habilita ADC com prescaler de 64

    initTimer1();
}

void initTimer1() {
    TCCR1B |= (1 << WGM12); // Modo CTC
    OCR1A = 15624; // Para 1 segundo com F_CPU = 16MHz e prescaler 1024
    TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler de 1024
    TIMSK1 |= (1 << OCIE1A);
    sei(); 
}

void moverMotores(uint8_t re, uint8_t moverEsq, uint8_t moverDir) {
    if (moverEsq) {
        if (re) {
            PORTD |= (1 << MOTOR_ESQ_TRAS);
            PORTD &= ~(1 << MOTOR_ESQ_FRENTE);
        } else {
            PORTD |= (1 << MOTOR_ESQ_FRENTE);
            PORTD &= ~(1 << MOTOR_ESQ_TRAS);
        }
    } else {
        PORTD &= ~((1 << MOTOR_ESQ_FRENTE) | (1 << MOTOR_ESQ_TRAS));
    }

    if (moverDir) {
        if (re) {
            PORTD |= (1 << MOTOR_DIR_TRAS);
            PORTD &= ~(1 << MOTOR_DIR_FRENTE);
        } else {
            PORTD |= (1 << MOTOR_DIR_FRENTE);
            PORTD &= ~(1 << MOTOR_DIR_TRAS);
        }
    } else {
        PORTD &= ~((1 << MOTOR_DIR_FRENTE) | (1 << MOTOR_DIR_TRAS));
    }
}

void pararMotores() {
    PORTD &= ~((1 << MOTOR_ESQ_FRENTE) | (1 << MOTOR_ESQ_TRAS) |
               (1 << MOTOR_DIR_FRENTE) | (1 << MOTOR_DIR_TRAS));
}

void controleLEDs() {
    static uint8_t aguardandoBaixo = 0;

    // Leitura do valor do LDR
    ADMUX = (ADMUX & 0xF0) | (LDR & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    uint16_t ldrValor = ADC;

    if (ldrValor > 600 && !aguardandoBaixo && ledsAtivos > 0) {
        // Desliga um LED por vez
        if (ledsAtivos == 3) {
            PORTC &= ~(1 << LED1);
            ledsAtivos--;
        } else if (ledsAtivos == 2) {
            PORTC &= ~(1 << LED2);
            ledsAtivos--;
        } else if (ledsAtivos == 1) {
            PORTC &= ~(1 << LED3);
            ledsAtivos--;
        }
        giro180();
        aguardandoBaixo = 1; // Aguarda o LDR cair abaixo de 500
    }

    // Se o LDR caiu para menos de 500, reinicia o ciclo
    if (ldrValor < 450 && aguardandoBaixo) {
        aguardandoBaixo = 0;
    }
}

void giro180() {
    // Gira 180 graus utilizando os motores em sentido contrário
    PORTD |= (1 << MOTOR_ESQ_FRENTE) | (1 << MOTOR_DIR_TRAS);
    _delay_ms(4000);
    pararMotores();
}
