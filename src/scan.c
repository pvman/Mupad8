#include <stdbool.h>
#include <stdint.h>
#include <xc.h>
#include "../bsp/pic16f1459/adc.h"

//
#include "../bsp/pic16f1459/leds.h"
#include "../src/pic16f1459/system.h"

//enum { N_KEYS = 38 };
#define N_KEYS          39  

static uint8_t s_idx_key = 0;
static uint8_t s_level = 0;

static bool wait_timer = false;
uint8_t adc_levels[N_KEYS] = {0};

static bool is_initialized = false;

typedef struct {
    uint8_t pitch;
    uint8_t velocity;
} note_event_t;

static note_event_t note_event = {0, 0};

enum {
    PIN_OUTPUT = 0,
    PIN_INPUT = 1,
    PIN_ANALOG = 1,
    PIN_DIGITAL = 0
};

void scan_init() {   
    is_initialized = true;
    TRISCbits.TRISC3 = PIN_INPUT;
    TRISCbits.TRISC4 = PIN_OUTPUT;
    TRISCbits.TRISC5 = PIN_OUTPUT;
    
    ANSELA = 0;

    ANSELCbits.ANSC3 = PIN_ANALOG;  // An7=C3: ADC in
    LATCbits.LATC4 = 0;
    LATCbits.LATC5 = 0; 

    ANSELAbits.ANSA4 = PIN_DIGITAL; // End Of Scan
    TRISAbits.TRISA4 = PIN_INPUT;
    
    PIE1bits.ADIE = 0; // no ADC interrupt    
    ADCON0bits.ADON = 1; // ADC on
    ADCON1bits.ADFM = 0; // ADC result is left-justified; most of it in ADCRESH
    ADCON1bits.ADCS = 0b110; // Fosc/64; Tad = 4us @ Fosc=16MHz; 50us before ADC data is ready
    ADCON1bits.ADPREF = 0; // ADC ref is Vcc
    ADCON2bits.TRIGSEL = 0; // no autoconversion
}

inline void set_timer(void) {
    wait_timer = true;
    OPTION_REGbits.TMR0CS = 0; // use Fosc/4 as a clock source
    OPTION_REGbits.PSA = 1; // disable the timer prescaler
    INTCONbits.TMR0IE = 0; // disable the timer0 interrupt
    INTCONbits.TMR0IF = 0; // clear timer0 overflow flag
    TMR0 = 0;              // restart the timer
}

inline bool waiting_for_timer(void) {
    return (wait_timer = wait_timer && !INTCONbits.TMR0IF);
}

inline void set_clock_and_data(const uint8_t c, const uint8_t d) {
    LATCbits.LATC4 = !!c;
    LATCbits.LATC5 = !!d; 
}

inline uint8_t get_data(void) {
    return LATCbits.LATC5;
}

inline bool is_end_of_scan(void) {
    //return !! PORTAbits.RA4;
    static uint8_t scan_counter = 0;
    bool res = (++scan_counter) >= N_KEYS;
    if (scan_counter >= N_KEYS) scan_counter = 0;
    return res;       
}

inline void run_adc(void) {
    ADCON0bits.CHS = 7;
    ADCON0bits.GO = 1;
}

inline bool is_adc_ready(void) {
    return !ADCON0bits.GO_nDONE;
}

inline uint8_t get_adc(void) {
    return ADRESH;
}

inline void process_level(uint8_t idx_key, const uint8_t old_level, const uint8_t new_level) {
    s_idx_key = idx_key; 
    s_level = new_level;   
    
}


inline bool store_key_level(uint8_t level, bool end_of_scan) {
    static uint8_t idx_key = 0;
    process_level(idx_key, adc_levels[idx_key], level);
    adc_levels[idx_key] = level;
    idx_key ++;
    if (idx_key >= N_KEYS || end_of_scan) idx_key = 0;
    return idx_key == 0;
}

enum {
    S_INIT,
    S_GO,
    S_CLOCK_RAISE,
    S_CLOCK_FALL,
    S_ADC_WAIT,
    S_ADC_READ    
};

bool scan_task(){    
    static uint8_t state = S_INIT;    
    
    
    if (!is_initialized)
        scan_init();
    
    if (waiting_for_timer()) return false;
    
    switch(state) {
        case(S_INIT):
            set_clock_and_data(0,1);
            state = S_GO;
            set_timer();
        break;
        
        case(S_GO):
            set_clock_and_data(0,1);
            state = S_CLOCK_RAISE;
            set_timer();
         break;
         
        case(S_CLOCK_RAISE):
            set_clock_and_data(1, get_data());
            state = S_CLOCK_FALL;
            set_timer();
        break;
        
        case(S_CLOCK_FALL):
            set_clock_and_data(0,0);
            run_adc();
            state = S_ADC_WAIT;
            set_timer();
        break;
        
        case(S_ADC_WAIT):
            state = (is_adc_ready()? S_ADC_READ : S_ADC_WAIT);
        break;   
        
        case(S_ADC_READ):
            state = (store_key_level(get_adc(), is_end_of_scan())? S_GO : S_CLOCK_RAISE);
        break;
    }    
    
   
    return true;
}
