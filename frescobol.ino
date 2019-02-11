//#define DEBUG
//#define TV_ON

#include <EEPROM.h>
#include "pitches.h"

typedef char  s8;
typedef short s16;
typedef unsigned long u32;

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

static const int MAP[2] = { PIN_ESQ, PIN_DIR };

#define HITS_MAX   700
#define HITS_BESTS 10

#define HIT_BACK_DT 200         // minimum time to hold for back
#define HIT_MIN_DT  235         // minimum time between two hits (125kmh)
#define HIT_KMH_MAX 125         // to fit in s8 (changed to u8, but lets keep 125)

#define HIT_MARK 0
#define HIT_NONE 1
#define HIT_SERV 2

#define STATE_IDLE       0
#define STATE_PLAYING    1
#define STATE_TIMEOUT    2

#define NAME_MAX 20

int  STATE;
bool IS_BACK;
char STR[32];

typedef struct {
    char names[2][NAME_MAX+1];  // = { "Atleta ESQ", "Atleta DIR" };
    u32  timeout;               //  = 300 * ((u32)1000);
    int  distance;              // = 800;

    u16  hit;
    s8   dts[HITS_MAX];         // cs (ms*10)
} Save;
Save S;

typedef struct {
    // needed on EEPROM_Load
    u8  kmhs[HITS_MAX];            // kmh (max 125km/h)

    // calculated when required
    s8  bests[2][2][HITS_BESTS];    // kmh (max 125kmh/h)
    u32 time;                       // ms (total time)
    u32 ps[2];                      // sum(kmh*kmh)
    u16 hits;
    u8  servs;
    s8  pace;                       // kmh
    u16 total;
} Game;
Game G;

int Falls (void) {
    return G.servs - (STATE==STATE_IDLE ? 0 : 1);
}

int  PT_Bests (s8* bests, int* min_, int* max_);
void PT_All   (void);
#include "pt.c.h"

void TV_All (const char* str, int p, int kmh, int is_back);
#include "tv.c.h"

void Serial_Hit   (char* name, u32 kmh, bool is_back);
void Serial_Score (void);
void Serial_Log   (void);
int  Serial_Check (void);
#include "serial.c.h"

void Sound (s8 kmh) {
    if (kmh < 40) {
        tone(PIN_TONE, NOTE_C5, 50);
    } else if (kmh < 50) {
        tone(PIN_TONE, NOTE_D5, 50);
    } else if (kmh < 60) {
        tone(PIN_TONE, NOTE_E5, 50);
    } else if (kmh < 70) {
        tone(PIN_TONE, NOTE_F5, 50);
    } else if (kmh < 80) {
        tone(PIN_TONE, NOTE_G5, 50);
    } else if (kmh < 90) {
        tone(PIN_TONE, NOTE_A5, 50);
    } else {
        tone(PIN_TONE, NOTE_B5, 50);
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

void EEPROM_Load (void) {
    for (int i=0; i<sizeof(Save); i++) {
        ((byte*)&S)[i] = EEPROM[i];
    }
    S.hit = min(S.hit, HITS_MAX);
    S.names[0][NAME_MAX] = '\0';
    S.names[1][NAME_MAX] = '\0';

    for (int i=0; i<S.hit; i++) {
        s8 dt = S.dts[i];
        dt = (dt > 0) ? dt : -dt;

        u32 kmh_ = ((u32)36) * S.distance / dt*10;
                   // prevents overflow
        G.kmhs[i] = min(kmh_, HIT_KMH_MAX);
    }
}

void EEPROM_Save (void) {
    for (int i=0; i<sizeof(Save); i++) {
        EEPROM[i] = ((byte*)&S)[i];
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

    EEPROM_Load();
}

void loop (void)
{
// RESTART
    Serial.println(F("= INICIO ="));
    STATE = STATE_IDLE;

    while (1)
    {
// SERVICE
        delay(2000);
        tone(PIN_TONE, NOTE_G7, 500);
        delay(1000);

        PT_All();
        TV_All("GO!", 0, 0, 0);
        Serial_Score();

        int got = Await_Press(true);
        if (got == -1) {
            goto _RESTART;
        }

        u32 t0 = millis();

        if (got != S.hit%2) {
            S.dts[S.hit++] = HIT_NONE;
        }
        S.dts[S.hit++] = HIT_SERV;
        STATE = STATE_PLAYING;

        tone(PIN_TONE, NOTE_C5, 50);

        IS_BACK = false;

#ifdef DEBUG
        Serial.println(F("> saque"));
#endif
        PT_All();
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
                    tone(PIN_TONE, NOTE_C8, 10);
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
            dt = min(dt/10, 127); // we don't have space for dt>1270ms,so we'll
                                  // just assume it since its already slow

            u32 kmh_ = ((u32)36) * S.distance / (dt*10);
                       // prevents overflow
            s8 kmh = min(kmh_, HIT_KMH_MAX);
            Sound(kmh);

#ifdef DEBUG
            if (nxt != got) {
                Serial_Hit(S.names[got],   kmh, IS_BACK);
                Serial_Hit(S.names[1-got], kmh, false);
            } else {
                Serial_Hit(S.names[1-got], kmh, IS_BACK);
            }
#endif

            if (IS_BACK) {
                S.dts[S.hit] = -dt;
            } else {
                S.dts[S.hit] = dt;
            }
            G.kmhs[S.hit] = kmh;
            S.hit++;
            if (nxt != got) {
                S.dts[S.hit]  = dt;
                G.kmhs[S.hit] = kmh;
                S.hit++;
            }
            nxt = 1 - got;

//u32 x1 = millis();
            PT_All();
//u32 x2 = millis();
//Serial.print("> ");
//Serial.println(x2-x1);
            TV_All(NULL, 1-got, kmh, IS_BACK);

// TIMEOUT
            if (G.time >= S.timeout) {
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
                    tone(PIN_TONE, NOTE_C4, 30);
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

        tone(PIN_TONE, NOTE_C4, 100);
        delay(110);
        tone(PIN_TONE, NOTE_C3, 100);
        delay(110);
        tone(PIN_TONE, NOTE_C2, 300);
        delay(310);

        EEPROM_Save();
        PT_All();
        TV_All("QUEDA", 0, 0, 0);
        Serial.println(F("QUEDA"));
    }

_TIMEOUT:
    STATE = STATE_TIMEOUT;
    tone(PIN_TONE, 200, NOTE_C7);
    PT_All();
    TV_All("FIM", 0, 0, 0);
    Serial_Score();
    Serial.println(F("= FIM ="));
    delay(5000);

    while (1) {
        int got = Await_Press(true);
        if (got == -1) {
            goto _RESTART;
        }
        if (got > 0) {
            tone(PIN_TONE, NOTE_C8, 10);
            delay(1000);
            if (digitalRead(PIN_ESQ)==LOW && digitalRead(PIN_DIR)==LOW) {
                break;
            }
        }
    }

_RESTART:
    S.hit = 0;
    EEPROM_Save();
}
