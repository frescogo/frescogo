#define MAJOR 1
#define MINOR 4

//#define DEBUG
//#define TV_ON

#ifdef DEBUG
#define assert(x) \
    if (!x) {                               \
        pinMode(LED_BUILTIN,OUTPUT);        \
        while (1) {                         \
            digitalWrite(LED_BUILTIN,HIGH); \
            delay(250);                     \
            digitalWrite(LED_BUILTIN,LOW);  \
            delay(250);                     \
        }                                   \
    }
#else
#define assert(x)
#endif

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
static TVout TV;
static pollserial pserial;

#else // !TV_ON

#endif

#if 1
#define PIN_LEFT  4
#define PIN_RIGHT 2
#define PIN_CFG   3
#else
#define PIN_LEFT  3
#define PIN_RIGHT 4
#define PIN_CFG   2
#endif

#ifdef ARDUINO_AVR_NANO
#define PIN_TONE 11
#endif

static const int MAP[2] = { PIN_LEFT, PIN_RIGHT };

#define HITS_MAX        600
#define HITS_BESTS      7

#define HIT_BACK_DT     180         // minimum time to hold for back
#define HIT_MIN_DT      235         // minimum time between two hits (125kmh)
//#define HIT_KMH_MAX   125         // to fit in s8 (changed to u8, but lets keep 125)
#define HIT_KMH_MAX     100         // safe value to avoid errors

#define HIT_MARK        0
#define HIT_NONE        1
#define HIT_SERV        2

#define STATE_IDLE      0
#define STATE_PLAYING   1
#define STATE_TIMEOUT   2

#define NAME_MAX        20

#define POT_VEL         50
#define MAX_VEL         85

static int  STATE;
static bool IS_BACK;
static char STR[100];

typedef struct {
    char juiz[NAME_MAX+1];      // = "Juiz"
    char names[2][NAME_MAX+1];  // = { "Atleta ESQ", "Atleta DIR" }
    u32  timeout;               // = 180 * ((u32)1000) ms
    u16  distancia;             // = 700 cm
    s8   potencia;              // = sim/nao
    s8   equilibrio;            // = sim/nao
    u8   continuidade;          // = 3%
    s8   velocidades;           // = sim/nao
    u8   maxima;                // = 85 kmh

    u16  hit;
    s8   dts[HITS_MAX];         // cs (ms*10)
} Save;
static Save S;

typedef struct {
    // needed on EEPROM_Load
    u8  kmhs[HITS_MAX];            // kmh (max 125km/h)

    // calculated when required
    s8  bests[2][2][HITS_BESTS];    // kmh (max 125kmh/h)
    u32 time;                       // ms (total time)
    u32 ps[2];                      // sum(kmh*kmh)
    u16 hits;
    u8  servs;
    s8  pace[2];                    // kmh/kmh2
    u16 total;
} Game;
static Game G;

enum {
    IN_LEFT  = 0,   // must be 0 (bc of MAP and 1-X)
    IN_RIGHT = 1,   // must be 1 (bc of MAP and 1-X)
    IN_NONE,
    IN_GO_FALL,
    IN_TIMEOUT,
    IN_RESTART,
    IN_UNDO,
    IN_RESET
};

int Falls (void) {
    return G.servs - (STATE==STATE_IDLE ? 0 : 1);
                        // after fall
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
        tone(PIN_TONE, NOTE_E5, 50);
    } else if (kmh < 60) {
        tone(PIN_TONE, NOTE_G5, 50);
    } else if (kmh < 70) {
        tone(PIN_TONE, NOTE_C5, 20);
        delay(35);
        tone(PIN_TONE, NOTE_C5, 20);
    } else if (kmh < 80) {
        tone(PIN_TONE, NOTE_E5, 20);
        delay(35);
        tone(PIN_TONE, NOTE_E5, 20);
    } else if (kmh < 90) {
        tone(PIN_TONE, NOTE_G5, 20);
        delay(35);
        tone(PIN_TONE, NOTE_G5, 20);
    } else {
        tone(PIN_TONE, NOTE_C6, 20);
        delay(35);
        tone(PIN_TONE, NOTE_C6, 20);
    }
}

int Await_Input (bool serial) {
    static u32 old;
    static int pressed = 0;
    while (1) {
        if (serial) {
            int ret = Serial_Check();
            if (ret != IN_NONE) {
                return ret;
            }
        }

        u32 now = millis();

        int pin_left  = digitalRead(PIN_LEFT);
        int pin_right = digitalRead(PIN_RIGHT);

        // CFG UNPRESSED
        if (digitalRead(PIN_CFG) == HIGH)
        {
            pressed = 0;
            old = now;
            if (pin_left == LOW) {
                return IN_LEFT;
            }
            if (pin_right == LOW) {
                return IN_RIGHT;
            }
        }

        // CFG PRESSED
        else
        {
            if (!pressed) {
                tone(PIN_TONE, NOTE_C2, 50);
                pressed = 1;
            }

            // fall
            if        (now-old>= 750 && pin_left==HIGH && pin_right==HIGH) {
                old = now;
                return IN_GO_FALL;
            } else if (now-old>=5000 && pin_left==LOW  && pin_right==HIGH) {
                old = now;
                return IN_UNDO;
            } else if (now-old>=5000 && pin_left==HIGH && pin_right==LOW) {
                old = now;
                return IN_RESTART;
            } else if (now-old>=5000 && pin_left==LOW  && pin_right==LOW) {
                old = now;
                return IN_RESET;
            }
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

        u32 kmh_ = ((u32)36) * S.distancia / (dt*10);
                   // prevents overflow
        G.kmhs[i] = min(kmh_, HIT_KMH_MAX);
        G.kmhs[i] = min(G.kmhs[i], S.maxima);
    }
}

void EEPROM_Save (void) {
    for (int i=0; i<sizeof(Save); i++) {
        EEPROM[i] = ((byte*)&S)[i];
    }
}

void EEPROM_Default (void) {
    strcpy(S.juiz,     "?");
    strcpy(S.names[0], "Atleta ESQ");
    strcpy(S.names[1], "Atleta DIR");
    S.distancia    = 750;
    S.timeout      = 180 * ((u32)1000);
    S.potencia     = 0;
    S.equilibrio   = 1;
    S.continuidade = 3;
    S.velocidades  = 1;
    S.maxima       = MAX_VEL;
}

void setup (void) {
    pinMode(PIN_CFG,   INPUT_PULLUP);
    pinMode(PIN_LEFT,  INPUT_PULLUP);
    pinMode(PIN_RIGHT, INPUT_PULLUP);

#ifdef TV_ON
    TV.begin(PAL,DX,DY);
    TV.select_font(font4x6);
    TV.set_hbi_hook(Serial.begin(9600));
#else
    Serial.begin(9600);
#endif

    EEPROM_Load();
}

u32 alarm (void) {
    u32 left = S.timeout - G.time;
    if (left < 5000) {
        return S.timeout - 0;
    } else if (left < 10000) {
        return S.timeout - 5000;
    } else if (left < 30000) {
        return S.timeout - 10000;
    } else if (left < 60000) {
        return S.timeout - 30000;
    } else {
        return (G.time/60000 + 1) * 60000;
    }
}

void loop (void)
{
// RESTART
    Serial.print(F("= FrescoGO! (versao "));
    Serial.print(MAJOR);
    Serial.print(".");
    Serial.print(MINOR);
    Serial.println(F(") ="));
    STATE = STATE_IDLE;
    PT_All();
    TV_All("GO!", 0, 0, 0);
    Serial_Score();
    u32 al_nxt = alarm();

    while (1)
    {
// GO
        PT_All();
        if (G.time >= S.timeout) {
            goto _TIMEOUT;          // if reset on ended game
        }

        int got;
        while (1) {
            got = Await_Input(true);
            if (got == IN_RESET) {
                EEPROM_Default();
                goto _RESTART;
            } else if (got == IN_RESTART) {
                goto _RESTART;
            } else if (got==IN_UNDO && S.hit>0) {
                while (1) {
                    S.hit -= 1;
                    if (S.hit == 0) {
                        break;
                    } else if (S.dts[S.hit] == HIT_SERV) {
                        if (S.dts[S.hit-1] == HIT_NONE) {
                            S.hit -= 1;
                        }
                        break;
                    }
                }
                tone(PIN_TONE, NOTE_C2, 100);
                delay(110);
                tone(PIN_TONE, NOTE_C3, 100);
                delay(110);
                tone(PIN_TONE, NOTE_C4, 300);
                delay(310);
                EEPROM_Save();
                PT_All();
                TV_All("GO!", 0, 0, 0);
                Serial_Score();

/* No TIMEOUT outside playing: prevents falls miscount.
            } else if (got == IN_TIMEOUT) {
                goto _TIMEOUT;
*/
            } else if (got == IN_GO_FALL) {
                break;
            }
        }
        tone(PIN_TONE, NOTE_C7, 500);
        TV_All("GO!", 0, 0, 0);
        Serial_Score();

// SERVICE
        while (1) {
            got = Await_Input(true);
            if (got == IN_RESET) {
                EEPROM_Default();
                goto _RESTART;
            } else if (got == IN_RESTART) {
                goto _RESTART;
/* No TIMEOUT outside playing: prevents falls miscount.
            } else if (got == IN_TIMEOUT) {
                goto _TIMEOUT;
*/
/* No FALL just after service: prevents double falls.
            } else if (got == IN_GO_FALL) {
                goto _FALL;
*/
            } else if (got==IN_LEFT || got==IN_RIGHT) {
                break;
            }
        }

        u32 t0 = millis();

        if (got != S.hit%2) {
            S.dts[S.hit++] = HIT_NONE;
        }
        S.dts[S.hit++] = HIT_SERV;
        STATE = STATE_PLAYING;

        tone(PIN_TONE, NOTE_C5, 50);

        IS_BACK = false;

        if (S.velocidades) {
            Serial.println(F("> saque"));
        }

        PT_All();
        TV_All("---", 0, 0, 0);
        delay(HIT_MIN_DT);

        int nxt = 1 - got;
        while (1)
        {
            // wait "got" pin to unpress
            while (digitalRead(MAP[got]) == LOW)
                ;

// HIT
            u32 t1;
            int dt;
            while (1) {
                got = Await_Input(true);
                if (got == IN_RESET) {
                    EEPROM_Default();
                    goto _RESTART;
                } else if (got == IN_RESTART) {
                    goto _RESTART;
                } else if (got == IN_TIMEOUT) {
                    goto _TIMEOUT;
                } else if (got == IN_GO_FALL) {
                    goto _FALL;
                } else if (got==IN_LEFT || got==IN_RIGHT) {
                    t1 = millis();
                    dt = (t1 - t0);
                    if (got==nxt || dt>=3*HIT_MIN_DT) {
                        break;
                    } else {
                        // ball cannot go back and forth so fast
                    }
                }
            }
            //ceu_arduino_assert(dt>50, 2);

            t0 = t1;

            if (nxt != got) {
                dt = dt / 2;
            }
            dt = min(dt/10, 127); // we don't have space for dt>1270ms,so we'll
                                  // just assume it since its already slow

            u32 kmh_ = ((u32)36) * S.distancia / (dt*10);
                       // prevents overflow
            s8 kmh = min(kmh_, HIT_KMH_MAX);
            kmh = min(kmh_, S.maxima);

            u8 al_now = 0;
            if (G.time > al_nxt) {
                tone(PIN_TONE, NOTE_C7, 250);
                al_now = 1;
                al_nxt = alarm();
            } else {
                Sound(kmh);
            }

            if (S.velocidades) {
                if (nxt != got) {
                    Serial_Hit(kmh, IS_BACK);
                    Serial_Hit(kmh, false);
                } else {
                    Serial_Hit(kmh, IS_BACK);
                }
            }

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

            PT_All();
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
                    IS_BACK = (digitalRead(PIN_LEFT)  == LOW);
                } else {
                    IS_BACK = (digitalRead(PIN_RIGHT) == LOW);
                }
                if (!al_now)
                {
                    if (IS_BACK) {
                        tone(PIN_TONE, NOTE_C4, 30);
                    } else {
                        // desequilibrio
                        u32 p0  = G.ps[0];
                        u32 p1  = G.ps[1];
                        u32 avg = (p0 + p1) / 2;
                        u32 m   = min(p0,p1);
                        if (G.time >= 30000) {
                            if (kmh>60 && m*11/10<avg) {
                                if (p0>p1 && nxt==0 || p1>p0 && nxt==1) {
                                    tone(PIN_TONE, NOTE_C3, 30);
                                }
                            }
                        }
                    }
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

        PT_All();
        TV_All("QUEDA", 0, 0, 0);
        Serial.println(F("QUEDA"));
        Serial_Score();
        EEPROM_Save();

        if (Falls() >= 25) {
            S.dts[S.hit++] = HIT_SERV;  // simulate timeout after service
            goto _TIMEOUT;
        }
    }

_TIMEOUT:
    STATE = STATE_TIMEOUT;
    tone(PIN_TONE, NOTE_C2, 2000);
    PT_All();
    TV_All("FIM", 0, 0, 0);
    Serial.println(F("= FIM ="));
    Serial_Score();
    EEPROM_Save();

    while (1) {
        int got = Await_Input(true);
        if (got == IN_RESET) {
            EEPROM_Default();
            goto _RESTART;
        } else if (got == IN_RESTART) {
            goto _RESTART;
        }
    }

_RESTART:
    tone(PIN_TONE, NOTE_C5, 2000);
    S.hit = 0;
    EEPROM_Save();
}
