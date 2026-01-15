#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"

#define LED_PIN 15
#define BUTTON_PIN 14

int led_value = 0;
bool boton_presionado = false;

//Procedimiento para encendido y apagado de led
void onAndoffLed(char *mensaje) 
{
   led_value = 1 - led_value;
   gpio_put(LED_PIN, led_value);
   printf("%s\n", mensaje);
}

//Evento repetitivo ejecutado al cumplirse un tiempo predefinido
bool repeating_timer_callback(struct repeating_timer *t)
{

   if(!boton_presionado)
      onAndoffLed("LED Encendido/Apagado por Timer");
   else
      printf("Timer ejecutado sin conmutacion de LED\n");

   return true;
}

//Evento de "alarma" ejecutado tambin al cumplirse un tiempo predefinido
int64_t alarm_callback(alarm_id_t id, void *user_data)
{
   printf("Alarma Disparada con identificador %d\n", id);
   return 2000000;
}

//Accion ejecutada cuando se detecta un evento de Interrupcion 
// GPIO_IRQ_EDGE_FALL (0x04)	IRQ cuando el GPIO ha pasado de un 1 logico a un 0 logico.
// GPIO_IRQ_EDGE_RISE	(0x08) IRQ cuando el GPIO ha pasado de un 0 logico a un 1 logico.
void button_callback(uint gpio, uint32_t events)
{
   //printf("Interrupcion ocurrio en el PIN %d con Evento %d \n", gpio, events);
 
   //printf("Bandera: %d",boton_presionado);

   if(!boton_presionado && events == 8) {
      boton_presionado = true;
      printf("LED Apagado por Boton y evento %d \n", boton_presionado, events);
   }
   else {
      if(boton_presionado && events == 8) {
         boton_presionado = false;
         printf("LED Encendido por Boton valor y evento %d \n", boton_presionado, events);
      }
   }
}

int main()
{
   stdio_init_all();

   //Inicializacion de Pines en microcontrolador
   gpio_init(LED_PIN);
   gpio_set_dir(LED_PIN, GPIO_OUT);

   gpio_init(BUTTON_PIN);
   gpio_set_dir(BUTTON_PIN, GPIO_IN);
   gpio_pull_up(BUTTON_PIN);
   //Fin inicializacion de Pines en microcontrolador

   //Asignar funcin a ser llamada cuando se reciba un evento desde el PIN definido
   gpio_set_irq_enabled_with_callback(BUTTON_PIN, 0x04 | 0x08, 1, button_callback);

   //Definir un evento repetitivo por tiempo
   struct repeating_timer timer;
   add_repeating_timer_ms(800, repeating_timer_callback, NULL, &timer);
    
   //Definir una alarma que se llamara alarma generada al ser alcanzado el tiempo
   add_alarm_in_ms(4000, alarm_callback, NULL, false);

   while(1)
   {
      tight_loop_contents();
   }

}

