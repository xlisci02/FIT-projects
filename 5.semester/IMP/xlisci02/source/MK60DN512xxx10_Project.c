/**
 * File		MK60DN512xxx10_Project.c
 * Autor	Matus Liscinsky
 * Login	xlisci02
 * Popis	IMP Projekt - Zdrojovy kod pre meranie srdcoveho tepu na platforme Fitkit3
 * Datum	29.12.2018
 * Original
 */

#include "fsl_pit.h"
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK60D10.h"
#include "fsl_debug_console.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define PIEZZO_MASK 0x10        // PORT A, bit 4, - reproduktor

/* Aktivacne konstanty jednotlivych pozicii LED displeja (aktivacia pomocou log. 0 -- KATODA) */
#define D1 0x08C0 //PTA9
#define D2 0x0A80 //PTA6
#define D3 0x02C0 //PTA11
#define D4 0x0A40 //PTB7

/* Aktivacne konstanty hodnot cislic (7 segmentu a-g na PTA/PTD, svieti v log. 1 -- ANODA) */

#define DASH 0x0400
#define LED_OFF 0x0AC0

#define N0D 0xB300 // "0"
#define N0A 0x0100 // "0"

#define N1A 0x0100 // "1"
#define N1D 0x0100 // "1"

#define N2D 0xB100 // "2"
#define N2A 0x0400 // "2"

#define N3D 0x9500 // "3"
#define N3A 0x0500 // "3"

#define N4D 0x0300 // "4"
#define N4A 0x0500 // "4"

#define N5D 0x9200 // "5"
#define N5A 0x0500 // "5"

#define N6D 0xB200 // "6"
#define N6A 0x0500 // "6"

#define N7D 0x1100 // "7"
#define N7A 0x0100 // "7"

#define N8D 0xB300 // "8"
#define N8A 0x0500 // "8"

#define N9D 0x9300 // "9"
#define N9A 0x0500 // "9"

#define DELAY_LEDD 2500

const unsigned int digitA[] = { N0A, N1A, N2A, N3A, N4A, N5A, N6A, N7A, N8A, N9A };
const unsigned int digitD[] = { N0D, N1D, N2D, N3D, N4D, N5D, N6D, N7D, N8D, N9D };

char result[5] = "0000";

unsigned int index;

/* Inicializacia MCU - zakladne nastevenie hodin, vypnutie watchdogu */
void MCUInit(void)  {
    MCG->C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/* Fukcnia delay */
void delay(long long bound) {
  long long i;
   for(i = 0; i < bound; i++);
}

/* Funkcia pre pipnutie bzuciaka, vola sa v pripade neuspesneho merania tepu */
void beep(void) {
	for (uint32_t q = 0; q < 1000; q++) {
			GPIOA->PSOR = PIEZZO_MASK; delay(500);
			GPIOA->PCOR = PIEZZO_MASK; delay(500);
	}
}

void segmentInit(){

    // Aktivacia hodin riadiacich modulov portov A a D
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTD_MASK;

    // Nastaveni vyvodu LED displeje jako univerzalni GPIO pro SW rizeni

    // Nastavenie pinov ako GPIO piny
    uint32_t GPIO_output =  PORT_PCR_MUX(0x01) | PORT_PCR_DSE(0x01);
    PORTA->PCR[6] = GPIO_output ; //
    PORTA->PCR[7] = GPIO_output ; //
    PORTA->PCR[8] = GPIO_output ; //
    PORTA->PCR[9] = GPIO_output ; //
    PORTA->PCR[10] = GPIO_output ; //
    PORTA->PCR[11] = GPIO_output ; //

    PORTD->PCR[8] = GPIO_output ; //
    PORTD->PCR[9] = GPIO_output ; //
    PORTD->PCR[12] = GPIO_output ; //
    PORTD->PCR[13] = GPIO_output ; //
    PORTD->PCR[14] = GPIO_output ; //
    PORTD->PCR[15] = GPIO_output ; //

    PORTA->PCR[4]  = GPIO_output; // bzuciak

    // Konfiguracia ako vystupne piny
    PTA->PDDR = GPIO_PDDR_PDD(0x0FD0);  //
    PTD->PDDR = GPIO_PDDR_PDD(0xF300);  //

	return;
}



void display_val(char *val_str) {

    // Ak sme ukazatelom mimo retazec alebo mimo displej
    // nic nezobrazujeme a skoncime
    if (index >= strlen(val_str) || index > 4)
    {
        PTA->PDOR = GPIO_PDOR_PDO(LED_OFF);
        PTD->PDOR = GPIO_PDOR_PDO(0x0000);
        return;
    }
    if (isdigit(val_str[index])) // zobrazovanie cislic
    {
        PTA->PDOR |= GPIO_PDOR_PDO(digitA[val_str[index]-'0']);
		PTD->PDOR = GPIO_PDOR_PDO(digitD[val_str[index]-'0']);

        index++;
    }
    else // v pripade neciselneho symbolu zobrazujeme pomlcku '-'
    {
        PTA->PDOR |=  GPIO_PDOR_PDO(DASH);
        PTD->PDOR = GPIO_PDOR_PDO(0x0000);
        index++;
    }
}

/* Funkcia pre vypis pola result na displej */
void print_on_display(void){
	index = 0;

	/* Prva cislica */
	PTA->PDOR = GPIO_PDOR_PDO(D1);
	display_val(result);
	delay(DELAY_LEDD);
	PTA->PDOR = GPIO_PDOR_PDO(LED_OFF);

	/* Druha cislica */
	PTA->PDOR = GPIO_PDOR_PDO(D2);
	display_val(result);
	delay(DELAY_LEDD);
	PTA->PDOR = GPIO_PDOR_PDO(LED_OFF);

	/* Tretia cislica */
	PTA->PDOR = GPIO_PDOR_PDO(D3);
	display_val(result);
	delay(DELAY_LEDD);
	PTA->PDOR = GPIO_PDOR_PDO(LED_OFF);

	/* Stvrta cislica */
	PTA->PDOR = GPIO_PDOR_PDO(D4);
	display_val(result);
	delay(DELAY_LEDD);
	PTA->PDOR = GPIO_PDOR_PDO(LED_OFF);
}

/* Inicializacia modulu ADC0 */
void ADC0_Init(void)
{
	/* Aktivacia hodin pre ADC0 */
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK; // Aktivace hodin pro ADC0

    /* Povolenie preruseni */
	NVIC_EnableIRQ(ADC0_IRQn);

	/* Nastavenie stavoveho a riadiaceho registra SC3 */
	ADC0->SC3 = 0xf; // Continuous Conversion Enable, Hardware Average Enable,
					// Hardware Average Select = 32 samples averaged

	/* Nastavenie konfiguracneho registra */
	ADC0->CFG1 = 0x6D; // The divide ratio is 8, (Bus clock)/2,  16-bit conversion

	/* Nastavenie stavoveho a riadiaceho registra SC1A */
	ADC0->SC1[0] = 0x50; // Conversion complete interrupt is enabled, AD16 is selected as input.

}

/* Globalne premenne pre meranie tepu */
#define ARR_SIZE 4
int readings[ARR_SIZE];      // pole nameranych hodnot
int total;                  // namerane hodnoty
int average;                // namerana spriemerovana hodnota
int before;					// predchadzajuca spremerovana hodnota
int beats;					// pocitac uderov srdca
int rise_threshold = 5;		// prah narastu signalu
int rise_count;				// pocitadlo sekvencii narastu signalu
int rising;					// flag, urcuje ci sa hodnota signalu zvysuje
int samples;				// pocet nameranych (spriemerovanych) vzorkov
int mutex = 1;				// pouziva sa ako zamok, pri spracovani vzorku
int ptr;					// index do pola nameranych hodnot
int sum;					// premenna potrebna pre smoothing (vyhladenie) signalu
int n;						// pocet nameranych hodnot pre priemerovanie
int interrupts;				// pocet preruseni od zaciatku merania
int measured;				// flag, urcuje ci uz bola nejaka hodnota namerana
int measuring = 1;			// flag, urcuje ci sa prave prebieha meranie

/* Obsluha prerusenia pre ADC0 */
void ADC0_IRQHandler(void) {
	print_on_display(); // vypis na displej s kazdym prerusenim

	/* prah necinnosti */
	if(ADC0->R[0] < 3000){
		samples = 0;
		beats = 0;
		if(measured == 0){
			result[0] = '0';

			result[1] = '0';

			result[2] = '0';

			result[3] = '0';
		}

		measuring = 1;
	}

	/* zbieranie hodnot zo vstupu */
	else{

		if(mutex && measuring){
			samples ++;
			total += ADC0->R[0];
			n++;
		}
	}
}


/* Obsluha generovaneho prerusenia */
void PIT0_IRQHandler(void){

	/* zamknutie zamku */
	mutex = 0;
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);

	/* Ak sa nieco nameralo */
	if(samples){
		interrupts++;

		/* Smoothing signalu */
		total /= n;
		sum -= readings[ptr];
		sum += total;
		readings[ptr] = total;
		average = sum / ARR_SIZE;

		/* Narast hodnoty signalu */
		if (average > before) {
		   rise_count++;
		   if (!rising && rise_count > rise_threshold){
			   rising = 1;
			   beats ++;
		   }
		}
		/* Signal pada/klesa */
		else {
			rising = 0;
			rise_count = 0;
		}

		before = average;
		n = 0;
		total = 0;
		ptr++;
		ptr %= ARR_SIZE;

		/* displej indikuje stav merania, postupne sa doplnuju '-' */
		if(interrupts %25 < 20){
			result[0] = '\0';
			for(unsigned i = 0 ; i < ((unsigned int)interrupts/375)+1; i++)
				strcat(result, "-");
		}
		/* pre blikanie */
		else
			result[0] = '\0';

		/* Vzorkuje sa kazdych 20ms (f = 50Hz),
		 * pocitadlo preruseni == 1500 indikuje uplynutie doby 30sek
		 * (50*30 = 1500) */
		if(interrupts == 1500) {
			if(beats*2 > 60 && beats*2 < 100){
				result[0] = (int)((beats*2)/1000) + '0';

				result[1] = ((int)((beats*2)/100))%10 + '0';

				result[2] = ((int)((beats*2)/10))%10 + '0';

				result[3] = (int)((beats*2))%10  + '0';
			}
			/* Tep mimo rozsah */
			else{
				beep();
			}
			measured = 1;
			measuring = 0;
			samples = 0;
	   }

	}
	else
		interrupts = 0;
	/* Uvolnenie zamku */
	mutex = 1;
	PIT_StartTimer(PIT, kPIT_Chnl_0);
}


int main(void)
{
 	/* Inicializacia */
	MCUInit();
    ADC0_Init();
    segmentInit();

    /* Periodic Interrupt Timer  */
    pit_config_t pitConfig;

	PIT_GetDefaultConfig(&pitConfig);
	PIT_Init(PIT, &pitConfig);
	/* Nastavenie periody generovania prerusenia na 20 ms */
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, MSEC_TO_COUNT(20U, CLOCK_GetFreq(kCLOCK_BusClk)));
	/* Povolenie preruseni */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
	NVIC_EnableIRQ(PIT0_IRQn);
	/* Spustenie casovaca prerusenia */
	PIT_StartTimer(PIT, kPIT_Chnl_0);

    while (1){}
    return 0;
}
