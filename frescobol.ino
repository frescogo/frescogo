typedef char  s8;
typedef short s16;
typedef unsigned long u32;

//#define DEBUG
//#define TV_ON

#ifdef TV_ON

#include <TVout.h>
#include <pollserial.h>
#include <fontALL.h>
#define Serial      pserial
#define delay       TV.delay
#define millis      TV.millis
#define tone(x,y,z) TV.tone(y,z)
#define DX  184
#define DY   64
#define FX    4
#define FY    6
TVout TV;
pollserial pserial;

#else // !TV_ON

#define PIN_TONE 11

#endif

#ifdef ARDUINO_AVR_NANO
#define PIN_ESQ 2
#define PIN_DIR 3
#endif
#ifdef ARDUINO_AVR_MEGA2560
#define PIN_ESQ 21
#define PIN_DIR 20
#endif

#define HITS_MAX    700
#define HITS_BESTS 10

#define HIT_BACK_DT 200         // minimum time to hold for back
#define HIT_MIN_DT  235         // minimum time between two hits (125kmh)
#define HIT_KMH_MAX 125         // to fit in s8

#define HIT_MARK 0
#define HIT_NONE 1
#define HIT_SERV 2

#define STATE_IDLE       0
#define STATE_PLAYING    1
#define STATE_TIMEOUT    2

int STATE;

typedef struct {
    u8 dt;                      // cs (ms*10)
    s8 kmh;                     // +/-kmh (max 125km/h)
} Hit;
Hit  HITS[HITS_MAX];
int  HIT = 0;

char NAMES[2][20] = { "Atleta ESQ", "Atleta DIR" };

u32  TIMEOUT  = 300 * ((u32)1000);
int  DISTANCE = 800;

bool IS_BACK;

char STR[32];

int MAP[2] = { PIN_ESQ, PIN_DIR };

typedef struct {
    s8  bests[2][2][HITS_BESTS];    // kmh (max 125kmh/h)
    u32 ps[2];                      // sum(kmh*kmh)
    u32 time;                       // ms (total time)
    u16 hits;
    u8  servs;
} Game;
Game GAME;

int Falls (void) {
    return GAME.servs - (STATE==STATE_IDLE ? 0 : 1);
}

int  PT_Bests       (s8* bests, int* min_, int* max_);
void PT_Bests_Apply (void);
u32  PT_Total       (void);
void PT_All         (void);
#include "pt.c.h"

void TV_All (const char* str, int p, int kmh, int is_back);
#include "tv.c.h"

void Serial_Hit   (char* name, u32 kmh, bool is_back);
void Serial_Score (void);
void Serial_Log   (void);
int  Serial_Check (void);
#include "serial.c.h"

void Sound (s8 kmh) {
    if (kmh < 45) {
        tone(PIN_TONE,  500, 30);
    } else if (kmh < 55) {
        tone(PIN_TONE, 1500, 30);
    } else if (kmh < 65) {
        tone(PIN_TONE, 2500, 30);
    } else if (kmh < 75) {
        tone(PIN_TONE, 3500, 30);
    } else if (kmh < 85) {
        tone(PIN_TONE, 4500, 30);
    } else {
        tone(PIN_TONE, 5000, 30);
    }
}

int Await_Press (bool serial) {
    while (1) {
        if (serial) {
            int ret = Serial_Check();
            if (ret < 0) {
                return ret;
            }
        }
        if (digitalRead(PIN_ESQ) == LOW) {
            return 0;
        }
        if (digitalRead(PIN_DIR) == LOW) {
            return 1;
        }
    }
}

void setup (void) {
    pinMode(PIN_ESQ, INPUT_PULLUP);
    pinMode(PIN_DIR, INPUT_PULLUP);

#ifdef TV_ON
    TV.begin(PAL,DX,DY);
    TV.select_font(font4x6);
    TV.set_hbi_hook(Serial.begin(9600));
#else
    Serial.begin(9600);
#endif
}

void loop (void)
{
    Serial.println(F("= INICIO ="));
    STATE = STATE_IDLE;
    HIT = 0;
    PT_All();
    TV_All("INICIO", 0, 0, 0);

    while (1)
    {
        delay(2000);
        tone(PIN_TONE, 3000, 500);
        delay(1000);

        PT_All();
        TV_All("GO!", 0, 0, 0);

// SERVICE
        int got = Await_Press(true);
        if (got == -1) {
            return;         // restart
        }
        STATE = STATE_IDLE;

        u32 t0 = millis();

        if (got != HIT%2) {
            HITS[HIT++].dt = HIT_NONE;
        }
        HITS[HIT++].dt = HIT_SERV;

        tone(PIN_TONE, 500, 30);

        IS_BACK = false;

        Serial.println(F("> saque"));
        TV_All("---", 0, 0, 0);
        delay(HIT_MIN_DT);

        int nxt = 1 - got;
        while (1)
        {
            // wait "got" pin to unpress
            while (digitalRead(MAP[got]) == LOW)
                ;

            // both are unpresseed?
            bool both = digitalRead(MAP[nxt]) == HIGH;

// HIT
            u32 t1;
            int dt;
            while (1) {
                got = Await_Press(false);
                t1 = millis();
                dt = (t1 - t0);
                if (got==nxt || dt>=3*HIT_MIN_DT) {
                    break;
                } else {
                    // ball cannot go back and forth so fast
                }
            }
            //ceu_arduino_assert(dt>50, 2);

            t0 = t1;

// FALL
            // if both were unpressed and now both are pressed,
            // and its long since the previous hit, then this is a fall
            if (dt > 1000 ) {
                delay(HIT_MIN_DT/2);
                if (both && digitalRead(PIN_ESQ)==LOW &&
                            digitalRead(PIN_DIR)==LOW)
                {
                    tone(PIN_TONE, 4000, 10);
                    for (int i=0; i<50; i++) {
                        delay(100);
                        if (digitalRead(PIN_ESQ)==HIGH || digitalRead(PIN_DIR)==HIGH) {
                            goto _FALL;
                        }
                    }
                    goto _TIMEOUT;
                }
            }

            if (nxt != got) {
                dt = dt / 2;
            }

            u32 kmh_ = ((u32)36) * DISTANCE / dt;
                       // prevents overflow
            s8 kmh = min(kmh_, HIT_KMH_MAX);
            Sound(kmh);

#ifndef TV_ON
            if (nxt != got) {
                Serial_Hit(NAMES[got],   kmh, IS_BACK);
                Serial_Hit(NAMES[1-got], kmh, false);
            } else {
                Serial_Hit(NAMES[1-got], kmh, IS_BACK);
            }
#endif

            HITS[HIT].dt = min(dt/10, 255);
            if (IS_BACK) {
                HITS[HIT].kmh = -kmh;
            } else {
                HITS[HIT].kmh = kmh;
            }
            HIT++;
            if (nxt != got) {
                HITS[HIT].dt  = min(dt/10, 255);
                HITS[HIT].kmh = kmh;
                HIT++;
            }
            nxt = 1 - got;

//u32 x1 = millis();
            PT_All();
//u32 x2 = millis();
//Serial.print("> ");
//Serial.println(x2-x1);
            TV_All(NULL, 1-got, kmh, IS_BACK);

// TIMEOUT
            if (GAME.time >= TIMEOUT) {
                goto _TIMEOUT;
            }

            // sleep inside hit to reach HIT_BACK_DT
            {
                u32 dt_ = millis() - t1;
                if (HIT_BACK_DT > dt_) {
                    delay(HIT_BACK_DT-dt_);
                }
                if (got == 0) {
                    IS_BACK = (digitalRead(PIN_ESQ) == LOW);
                } else {
                    IS_BACK = (digitalRead(PIN_DIR) == LOW);
                }
                if (IS_BACK) {
                    tone(PIN_TONE, 200, 30);
                }
            }

            // sleep inside hit to reach HIT_MIN_DT
            {
                u32 dt_ = millis() - t1;
                if (HIT_MIN_DT > dt_) {
                    delay(HIT_MIN_DT-dt_);
                }
            }
        }
_FALL:
        STATE = STATE_IDLE;

        tone(PIN_TONE, 300, 100);
        delay(150);
        tone(PIN_TONE, 200, 100);
        delay(150);
        tone(PIN_TONE, 100, 200);
        delay(200);

        PT_All();
        TV_All("QUEDA", 0, 0, 0);
        Serial.println(F("QUEDA"));
        Serial_Score();
    }

_TIMEOUT:
    STATE = STATE_TIMEOUT;
    tone(PIN_TONE, 200, 2000);
    PT_All();
    TV_All("FIM", 0, 0, 0);
    Serial_Score();
    Serial.println(F("= FIM ="));
    delay(5000);

    while (1) {
        int got = Await_Press(true);
        if (got == -1) {
            return;         // restart
        }
        delay(1000);
        if (digitalRead(PIN_ESQ)==LOW && digitalRead(PIN_DIR)==LOW) {
            break;
        }
    }
}
