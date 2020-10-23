/*
   Nombre del archivo: FP-uC
   Autor: Chiesa Docampo Franco
   Descripción: codigo para controlar 2 LEDs y trigger de la cámara. Puede ser usado mediante el Monitor Serie que provee el Arduino IDE o mediante la interfaz de Python.
*/

double val;
double val1;
double val2;
double val3;
double val4;
double fps_cam;
double DC;
double delay_trigger;

void FPS()
{

  GTCCR = _BV(TSM) | _BV(PSRSYNC); // Detiene y sincroniza los timers.

  // Configuración del timer 1 para PWM 1 | L470 (dataOdd, primer valor en el vector de datos a procesar).
  TIMSK1 = 0;
  TCNT1 = 0; // Delay
  ICR1 = val1; // Frecuencia
  OCR1A = val2; // Duty-Cycle
  DDRB |= (1 << DDB5); // PB5 ( OC1A/PCINT5 ) | digital pin 11 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << WGM11) | (0 << WGM10); // Inverting PWM on OC1A, TOP = ICR1.
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10); // Prescaler = 64.

  // Configuración del timer 3 para PWM 2 | L410 (dataEven, segundo valor en el vector de datos a procesar).
  TIMSK3 = 0;
  TCNT3 = val3; // Delay -> Desfase de 180 grados respecto del PWM 1.
  ICR3 = val1; // Frecuencia
  OCR3A = val2; // Duty-Cycle
  DDRE |= (1 << DDE3); // PE3 (AIN1/OC3A) | digital pin 5 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR3A = (1 << COM3A1) | (1 << COM3A0) | (1 << WGM31) | (0 << WGM30); // Inverting PWM on OC3A, PWM Phase Correct, TOP = ICR3.
  TCCR3B = (1 << WGM33) | (1 << WGM32) | (0 << CS32) | (1 << CS31) | (1 << CS30); // Prescaler = 64.

  // Configuración del timer 5 para PWM 3 (control del trigger de la cámara).
  TIMSK5 = 0;
  TCNT5 = val4; // El experimento que figura en el informe fue hecho con TNT5 = 330 a 40 FPS. TCNT5 debe ser tal que que el ciclo ON del PWM 3 este dentro de los ciclos OFF de los PWM 1 y 2.
  ICR5 = val3; // Frecuencia
  OCR5A = 50; // Duty Cycle
  DDRL |= (1 << DDL3); // // output pin is PWM 46 (ATMEGA2560) | PL3 (OC5A) | digital pin 46 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR5A = (1 << COM5A1) | (0 << COM5A0) | (1 << WGM51) | (0 << WGM50); // Non-inverting PWM on OC5A, TOP = ICR5.
  TCCR5B = (1 << WGM53) | (1 << WGM52) | (0 << CS52) | (1 << CS51) | (1 << CS50); // Prescaler = 64.

  GTCCR = 0; // Permite a los timers correr simultáneamente.

}

void L470_check() { // Reviso el estado del L470.

  GTCCR = _BV(TSM) | _BV(PSRSYNC); // Detiene y sincroniza los timers.

  // Configuración del timer 1 para PWM 1 | L470 (dataOdd, primer valor en el vector de datos a procesar).
  TIMSK1 = 0;
  TCNT1 = 0;
  ICR1 = 5000;
  OCR1A = 5000; // Duty-Cycle L470 100% -> señal de 0V -> L470 encendido de forma constante.
  DDRB |= (1 << DDB5); // PB5 ( OC1A/PCINT5 ) | digital pin 11 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << WGM11) | (0 << WGM10); // Inverting PWM on OC1A, TOP = ICR1.
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10); // Prescaler = 64.

  // Configuración del timer 3 para PWM 2 | L410 (dataEven, segundo valor en el vector de datos a procesar).
  TIMSK3 = 0;
  TCNT3 = 0;
  ICR3 = 5000;
  OCR3A = 0; // Duty-Cycle L410 0%% -> señal de 5V -> L410 apagado.
  DDRE |= (1 << DDE3); // PE3 (AIN1/OC3A) | digital pin 5 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR3A = (1 << COM3A1) | (1 << COM3A0) | (1 << WGM31) | (0 << WGM30); // Inverting PWM on OC3A, PWM Phase Correct, TOP = ICR3.
  TCCR3B = (1 << WGM33) | (1 << WGM32) | (0 << CS32) | (1 << CS31) | (1 << CS30); // Prescaler = 64.

  GTCCR = 0; // Permite a los timers correr simultáneamente.

}

void L410_check() { // Reviso el estado del L410

  GTCCR = _BV(TSM) | _BV(PSRSYNC); // Detiene y sincroniza los timers.

  // Configuración del timer 1 para PWM 1 | L470 (dataOdd, primer valor en el vector de datos a procesar).
  TIMSK1 = 0;
  TCNT1 = 0;
  ICR1 = 5000;
  OCR1A = 0; // Duty-Cycle L470 0% -> señal de 5V -> L470 apagado.
  DDRB |= (1 << DDB5); // PB5 ( OC1A/PCINT5 ) | digital pin 11 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << WGM11) | (0 << WGM10); // Inverting PWM on OC1A, TOP = ICR1.
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10); // Prescaler = 64.

  // Configuración del timer 3 para PWM 2 | L410 (dataEven, segundo valor en el vector de datos a procesar).
  TIMSK3 = 0;
  TCNT3 = 0;
  ICR3 = 5000;
  OCR3A = 5000; // Duty-Cycle L470 100%% -> señal de 0V -> L410 encendido de forma constante.
  DDRE |= (1 << DDE3); // PE3 (AIN1/OC3A) | digital pin 5 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR3A = (1 << COM3A1) | (1 << COM3A0) | (1 << WGM31) | (0 << WGM30); // Inverting PWM on OC3A, PWM Phase Correct, TOP = ICR3.
  TCCR3B = (1 << WGM33) | (1 << WGM32) | (0 << CS32) | (1 << CS31) | (1 << CS30); // Prescaler = 64.

  GTCCR = 0; // Permite a los timers correr simultáneamente.

}

void LEDs_OFF() {

  GTCCR = _BV(TSM) | _BV(PSRSYNC); // Detiene y sincroniza los timers.

  // Configuración del timer 1 para PWM 1 | L470 (dataOdd, primer valor en el vector de datos a procesar).
  TIMSK1 = 0;
  TCNT1 = 0;
  ICR1 = 5000;
  OCR1A = 0; // Duty-Cycle L470 0% -> señal de 5V -> L470 apagado.
  DDRB |= (1 << DDB5); // PB5 ( OC1A/PCINT5 ) | digital pin 11 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << WGM11) | (0 << WGM10); // Inverting PWM on OC1A, TOP = ICR1.
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10); // Prescaler = 64.

  // Configuración del timer 3 para PWM 2 | L410 (dataEven, segundo valor en el vector de datos a procesar).
  TIMSK3 = 0;
  TCNT3 = 0;
  ICR3 = 5000;
  OCR3A = 0; // Duty-Cycle L470 0%% -> señal de 5V -> L410 apagado.
  DDRE |= (1 << DDE3); // PE3 (AIN1/OC3A) | digital pin 5 (PWM) (https://www.arduino.cc/en/Hacking/PinMapping2560)
  TCCR3A = (1 << COM3A1) | (1 << COM3A0) | (1 << WGM31) | (0 << WGM30); // Inverting PWM on OC3A, PWM Phase Correct, TOP = ICR3.
  TCCR3B = (1 << WGM33) | (1 << WGM32) | (0 << CS32) | (1 << CS31) | (1 << CS30); // Prescaler = 64.

  GTCCR = 0; // Permite a los timers correr simultáneamente.

}

void DoSerial()
{
  char ch = toupper(Serial.read());   // Lee el caracter recibido y lo convierte a mayúscula.

  switch (ch) {
    case 'F': // Frames per second de la cámara y frecuencia de los 2 LEDs.

      Serial.print("Inserte FPS (20 a 100) seguida de un espacio: ");

      fps_cam = Serial.parseInt();   // Lee el número recibido.
      Serial.println(fps_cam);

      val = fps_cam / 2; // val = fps_cam / 2 -> TDM. | val = fps_cam si se desea usar 1 LED a la misma velocidad de la camara. Para hacer esto desconectar el LED que no se usa.

      Serial.print("Frecuencia de estimulación PWM 470 nm | 410 nm = ");
      Serial.println(val);

      val1 = 16 * pow(10, 6) / (val * 64) - 1;

      Serial.print("ICR1 and ICR3 = ");
      Serial.println(val1);

      val3 = 16 * pow(10, 6) / (fps_cam * 64) - 1;

      Serial.print("ICR5 = ");
      Serial.println(val3);

      Serial.print("TCNT3 = ");
      Serial.println(val3);

      break;

    case 'D': // Duty-Cycle de los PWMs que controlan los LEDs.

      Serial.print("Inserte Duty-Cycle (% de 1 a 49) seguida de un espacio: ");

      DC = Serial.parseInt();
      Serial.println(DC);

      val2 = (DC / 100) * val1;

      Serial.print("OCR1A = ");
      Serial.println(val2);

      break;

    case 'T': // Delay del trigger de la cámara. Determina el instante dentro del ciclo ON de cada PWM en el que la cámara va a sacar la foto.

      Serial.print("Inserte delay para el trigger seguido de un espacio: ");

      val4 = Serial.parseInt(); // [0 < val4 = TCNT5 < ICR1]
      Serial.println(val4);

      break;

    case 'A':

      L470_check();

      break;

    case 'B':

      L410_check();

      break;

    case 'C':

      LEDs_OFF();

      break;

    case 'S':

      FPS();

      break;

    default:
      break;
  }
}

int main() {

  Serial.begin(115200);
  Serial.setTimeout(10);

  cli();

  sei();

  LEDs_OFF();

  while (1)
  {
    if (Serial.available() > 0) {
      DoSerial();
    }
  }
}
