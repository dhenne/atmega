/*
 * main.c
 *
 *  Created on: 12.06.2015
 *      Author: henne
 */





// #define F_CPU 8000000UL
#define F_CPU 14745800UL
#define __AVR_ATmega16__

#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1

/* _BV(a) is a macro which returns the value corresponding to 2 to the power 'a'. Thus _BV(PX3) would be 0x08 or 0b00001000 */


#include "avr/io.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

#define F_PWM         500L               // PWM-Frequenz in Hz
#define PWM_PRESCALER 8                  // Vorteiler für den Timer
#define PWM_STEPS     256                // PWM-Schritte pro Zyklus(1..256)
#define PWM_PORT      PORTB              // Port für PWM
#define PWM_DDR       DDRB               // Datenrichtungsregister für PWM
#define PWM_CHANNELS  8                  // Anzahl der PWM-Kanäle

#define T_PWM (F_CPU/(F_PWM*PWM_STEPS)) // Systemtakte pro PWM-Takt

#if (T_PWM<(93+5))
    #error T_PWM zu klein, F_CPU muss vergrössert werden oder F_PWM oder PWM_STEPS verkleinert werden
#endif

// ab hier nichts ändern, wird alles berechnet
uint16_t pwm_timing[PWM_CHANNELS+1];          // Zeitdifferenzen der PWM Werte
uint16_t pwm_timing_tmp[PWM_CHANNELS+1];

uint8_t  pwm_mask[PWM_CHANNELS+1];            // Bitmaske für PWM Bits, welche gelöscht werden sollen
uint8_t  pwm_mask_tmp[PWM_CHANNELS+1];        // ändern uint16_t oder uint32_t für mehr Kanäle

uint8_t  pwm_setting[PWM_CHANNELS];           // Einstellungen für die einzelnen PWM-Kanäle
uint8_t  pwm_setting_tmp[PWM_CHANNELS+1];     // Einstellungen der PWM Werte, sortiert

volatile uint8_t pwm_cnt_max=1;               // Zählergrenze, Initialisierung mit 1 ist wichtig!
volatile uint8_t pwm_sync;                    // Update jetzt möglich

uint16_t *isr_ptr_time  = pwm_timing;
uint16_t *main_ptr_time = pwm_timing_tmp;

uint8_t *isr_ptr_mask  = pwm_mask;              // Bitmasken fuer PWM-Kanäle
uint8_t *main_ptr_mask = pwm_mask_tmp;          // ändern uint16_t oder uint32_t für mehr Kanäle
// ändern auf uint16_t für mehr als 8 Bit Auflösung

// Zeiger austauschen
// das muss in einem Unterprogramm erfolgen,
// um eine Zwischenspeicherung durch den Compiler zu verhindern

void tausche_zeiger(void) {
    uint16_t *tmp_ptr16;
    uint8_t *tmp_ptr8;                          // ändern uint16_t oder uint32_t für mehr Kanäle

    tmp_ptr16 = isr_ptr_time;
    isr_ptr_time = main_ptr_time;
    main_ptr_time = tmp_ptr16;
    tmp_ptr8 = isr_ptr_mask;
    isr_ptr_mask = main_ptr_mask;
    main_ptr_mask = tmp_ptr8;
}

// PWM Update, berechnet aus den PWM Einstellungen
// die neuen Werte für die Interruptroutine

void pwm_update(void) {

    uint8_t i, j, k;
    uint8_t m1, m2, tmp_mask;                   // ändern uint16_t oder uint32_t für mehr Kanäle
    uint8_t min, tmp_set;                       // ändern auf uint16_t für mehr als 8 Bit Auflösung

    // PWM Maske für Start berechnen
    // gleichzeitig die Bitmasken generieren und PWM Werte kopieren

    m1 = 1;
    m2 = 0;
    for(i=1; i<=(PWM_CHANNELS); i++) {
        main_ptr_mask[i]=~m1;                       // Maske zum Löschen der PWM Ausgänge
        pwm_setting_tmp[i] = pwm_setting[i-1];
        if (pwm_setting_tmp[i]!=0) m2 |= m1;        // Maske zum setzen der IOs am PWM Start
        m1 <<= 1;
    }
    main_ptr_mask[0]=m2;                            // PWM Start Daten

    // PWM settings sortieren; Einfügesortieren

    for(i=1; i<=PWM_CHANNELS; i++) {
        min=PWM_STEPS-1;
        k=i;
        for(j=i; j<=PWM_CHANNELS; j++) {
            if (pwm_setting_tmp[j]<min) {
                k=j;                                // Index und PWM-setting merken
                min = pwm_setting_tmp[j];
            }
        }
        if (k!=i) {
            // ermitteltes Minimum mit aktueller Sortiertstelle tauschen
            tmp_set = pwm_setting_tmp[k];
            pwm_setting_tmp[k] = pwm_setting_tmp[i];
            pwm_setting_tmp[i] = tmp_set;
            tmp_mask = main_ptr_mask[k];
            main_ptr_mask[k] = main_ptr_mask[i];
            main_ptr_mask[i] = tmp_mask;
        }
    }

    // Gleiche PWM-Werte vereinigen, ebenso den PWM-Wert 0 löschen falls vorhanden

    k=PWM_CHANNELS;             // PWM_CHANNELS Datensätze
    i=1;                        // Startindex

    while(k>i) {
        while ( ((pwm_setting_tmp[i]==pwm_setting_tmp[i+1]) || (pwm_setting_tmp[i]==0))  && (k>i) ) {

            // aufeinanderfolgende Werte sind gleich und können vereinigt werden
            // oder PWM Wert ist Null
            if (pwm_setting_tmp[i]!=0)
                main_ptr_mask[i+1] &= main_ptr_mask[i];        // Masken vereinigen

            // Datensatz entfernen,
            // Nachfolger alle eine Stufe hochschieben
            for(j=i; j<k; j++) {
                pwm_setting_tmp[j] = pwm_setting_tmp[j+1];
                main_ptr_mask[j] = main_ptr_mask[j+1];
            }
            k--;
        }
        i++;
    }

    // letzten Datensatz extra behandeln
    // Vergleich mit dem Nachfolger nicht möglich, nur löschen
    // gilt nur im Sonderfall, wenn alle Kanäle 0 sind
    if (pwm_setting_tmp[i]==0) k--;

    // Zeitdifferenzen berechnen

    if (k==0) { // Sonderfall, wenn alle Kanäle 0 sind
        main_ptr_time[0]=(uint16_t)T_PWM*PWM_STEPS/2;
        main_ptr_time[1]=(uint16_t)T_PWM*PWM_STEPS/2;
        k=1;
    }
    else {
        i=k;
        main_ptr_time[i]=(uint16_t)T_PWM*(PWM_STEPS-pwm_setting_tmp[i]);
        tmp_set=pwm_setting_tmp[i];
        i--;
        for (; i>0; i--) {
            main_ptr_time[i]=(uint16_t)T_PWM*(tmp_set-pwm_setting_tmp[i]);
            tmp_set=pwm_setting_tmp[i];
        }
        main_ptr_time[0]=(uint16_t)T_PWM*tmp_set;
    }

    // auf Sync warten

    pwm_sync=0;             // Sync wird im Interrupt gesetzt
    while(pwm_sync==0);

    // Zeiger tauschen
    cli();
    tausche_zeiger();
    pwm_cnt_max = k;
    sei();
}


ISR(TIMER1_COMPA_vect) {
    static uint8_t pwm_cnt;                     // ändern auf uint16_t für mehr als 8 Bit Auflösung
    uint8_t tmp;                                // ändern uint16_t oder uint32_t für mehr Kanäle

    OCR1A += isr_ptr_time[pwm_cnt];
    tmp    = isr_ptr_mask[pwm_cnt];

    if (pwm_cnt == 0) {
        PWM_PORT = tmp;                         // Ports setzen zu Begin der PWM
                                                // zusätzliche PWM-Ports hier setzen
        pwm_cnt++;
    }
    else {
        PWM_PORT &= tmp;                        // Ports löschen
                                                // zusätzliche PWM-Ports hier setzen
        if (pwm_cnt == pwm_cnt_max) {
            pwm_sync = 1;                       // Update jetzt möglich
            pwm_cnt  = 0;
        }
        else pwm_cnt++;
    }
}
enum colors {
	rot,
	gelb,
	blau,
	orange,
	weiss,
	lila,
	gruen,
	anzahl
};

void initialize(uint8_t values[anzahl][3])  {
	values[rot][0] = 0; values[rot][1] = 255; values[rot][2] = 0;
	values[gelb][0] = 0; values[gelb][1] = 255; values[gelb][2] = 140;
	values[blau][0] = 255; values[blau][1] = 0; values[blau][2] = 0;
	values[orange][0] = 0; values[orange][1] = 255; values[orange][2] = 60;
	values[lila][0] = 40; values[lila][1] = 255; values[lila][2] = 0;
	values[gruen][0] = 0; values[gruen][1] = 255; values[gruen][2] = 255;
	values[weiss][0] = 0; values[weiss][1] = 255; values[weiss][2] = 255;
}

int main(void) {
    // PWM Port einstellen

    PWM_DDR = 0xFF;         // Port als Ausgang
    // zusätzliche PWM-Ports hier setzen

    // Timer 1 OCRA1, als variablen Timer nutzen

    TCCR1B = 2;             // Timer läuft mit Prescaler 8
    TIMSK |= (1<<OCIE1A);   // Interrupt freischalten

    sei();                  // Interrupts global einschalten

    uint8_t t1[8]={10, 10, 10, 0, 0, 0, 0, 0};
    uint8_t values[anzahl][3] = { 0 };
    initialize(values);


    memcpy(pwm_setting, t1, 8);
    pwm_update();
    		uint8_t i = rand() % anzahl;
    		int direction = 0;
    	    while(1) {
    	    	if (values[i][0] == t1[0] && values[i][1] == t1[1]  && values[i][2] == t1[2] ) {
    	    		i = rand() % anzahl;
    	    		_delay_ms(1000);
    	    	} else {
    	    		for (int cx = 0 ; cx < 3 ; cx++) {
    	    			if (t1[cx] < values[i][cx] ) {
    	    				t1[cx]++;
    	    			} else if (t1[cx] > values[i][cx] ) {
    	    				t1[cx]--;
    	    			}
    	    		}
    	    	}
    	    	_delay_ms(1);
    	    	memcpy(pwm_setting, t1, 8);
    	    	pwm_update();
/*
    	    	if ( t1[i] > 250 || t1[i] < 10 ) {
    	    		i = rand() % 3;
    	    		if ( t1[i] > 250 ) {
    	    			direction = 1;
    	    		} else {
    	    			direction = 0;
    	    		}
    	    	}

    	    	if (direction == 0) {
    	    		t1[i] += 1;
    	    	} else {
    	    		t1[i] -= 1;
    	    	}


    	    	_delay_ms(1);
    	    	memcpy(pwm_setting, t1, 8);
    	    	pwm_update();
    	    	if (i == 3 && direction == 0) {
    	    		direction = 1;
    	    		i = 0;
    	    	}
    	    	if (i == 3 && direction == 1) {
    	    		direction = 0;
    	    		i = 0;
    	    	}*/

    	    }
	// randomColors(1000);

    return 0;
}

