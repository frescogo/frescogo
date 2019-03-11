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

#define PIN_LEFT  3
#define PIN_RIGHT 4
#define PIN_CFG   2

#ifdef ARDUINO_AVR_NANO
#define PIN_TONE 11
#endif

static const int MAP[2] = { PIN_LEFT, PIN_RIGHT };

#define HITS_MAX   700
#define HITS_BESTS 10

#define HIT_BACK_DT 200         // minimum time to hold for back
#define HIT_MIN_DT  235         // minimum time between two hits (125kmh)
//#define HIT_KMH_MAX 125         // to fit in s8 (changed to u8, but lets keep 125)
#define HIT_KMH_MAX  90         // safe value to avoid errors

#define HIT_MARK 0
#define HIT_NONE 1
#define HIT_SERV 2

#define STATE_IDLE       0
#define STATE_PLAYING    1
#define STATE_TIMEOUT    2

#define NAME_MAX 20

static int  STATE;
static bool IS_BACK;
static char STR[64];

typedef struct {
    char names[2][NAME_MAX+1];  // = { "Atleta ESQ", "Atleta DIR" };
    u32  timeout;               // = 300 * ((u32)1000);
    int  distance;              // = 800;

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
    s8  pace;                       // kmh
    u16 total;
} Game;
static Game G;

enum {
    IN_LEFT  = 0,   // must be 0 (bc of MAP and 1-X)
    IN_RIGHT = 1,   // must be 1 (bc of MAP and 1-X)
    IN_NONE,
    IN_GO,
    IN_FALL,
    IN_TIMEOUT,
    IN_ALL
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
        tone(PIN_TONE, NOTE_C6, 20);
        delay(30);
        tone(PIN_TONE, NOTE_C6, 20);
    } else if (kmh < 80) {
        tone(PIN_TONE, NOTE_E6, 20);
        delay(30);
        tone(PIN_TONE, NOTE_E6, 20);
    } else if (kmh < 90) {
        tone(PIN_TONE, NOTE_G6, 20);
        delay(30);
        tone(PIN_TONE, NOTE_G6, 20);
    } else {
        tone(PIN_TONE, NOTE_C7, 20);
        delay(30);
        tone(PIN_TONE, NOTE_C7, 20);
    }
}

enum {
    CFG_OFF,
    CFG_GO,         // 0s,  !LEFT, !RIGHT
    CFG_FALL,       // 2s,  !LEFT, !RIGHT
    CFG_TIMEOUT,    // 5s,   LEFT,  RIGHT
    CFG_ALL         // 30s, !LEFT, !RIGHT
};

int Await_Input (bool serial) {
    static u32 old;
    static int cfg = CFG_OFF;
    while (1) {
        if (serial) {
            int ret = Serial_Check();
            if (ret != IN_NONE) {
                return ret;
            }
        }

        // CFG UNPRESSED
        if (digitalRead(PIN_CFG) == HIGH)
        {
            int pin_left  = digitalRead(PIN_LEFT);
            int pin_right = digitalRead(PIN_RIGHT);

            // CFG was OFF
            if (cfg == CFG_OFF)
            {
                if (pin_left == LOW) {
                    return IN_LEFT;
                }
                if (pin_right == LOW) {
                    return IN_RIGHT;
                }
            }

            // CFG was ON
            else
            {
                int old = cfg;
                cfg = CFG_OFF;             // leave CFG mode
                //delay(500);
                if        (old==CFG_GO      && pin_left==HIGH && pin_right==HIGH) {
                    return IN_GO;
                } else if (old==CFG_FALL    && pin_left==HIGH && pin_right==HIGH) {
                    return IN_FALL;
                } else if (old==CFG_TIMEOUT && pin_left==LOW  && pin_right==LOW) {
                    return IN_TIMEOUT;
                } else if (old==CFG_ALL     && pin_left==HIGH && pin_right==HIGH) {
                    return IN_ALL;
                }
            }
        }

        // CFG PRESSED
        else
        {
            u32 now = millis();
            if (cfg == CFG_OFF) {          // enter CFG mode
                cfg = CFG_GO;
                old = millis();
                tone(PIN_TONE, NOTE_C2, 50);
            } else if (cfg==CFG_GO && now-old>=2000) {
                cfg = CFG_FALL;
                tone(PIN_TONE, NOTE_C3, 50);
            } else if (cfg==CFG_FALL && now-old>=5000) {
                cfg = CFG_TIMEOUT;
                tone(PIN_TONE, NOTE_C4, 50);
            } else if (cfg==CFG_TIMEOUT && now-old>=15000) {
                cfg = CFG_ALL;
                tone(PIN_TONE, NOTE_C5, 50);
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

        u32 kmh_ = ((u32)36) * S.distance / (dt*10);
                   // prevents overflow
        G.kmhs[i] = min(kmh_, HIT_KMH_MAX);
    }
}

void EEPROM_Save (void) {
    for (int i=0; i<sizeof(Save); i++) {
        EEPROM[i] = ((byte*)&S)[i];
    }
}

void EEPROM_Default (void) {
    strcpy(S.names[0], "Atleta ESQ");
    strcpy(S.names[1], "Atleta DIR");
    S.distance = 800;
    S.timeout  = 180 * ((u32)1000);
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

void loop (void)
{
// RESTART
    Serial.println(F("= INICIO ="));
    STATE = STATE_IDLE;

    while (1)
    {
// GO
        PT_All();
        int got;
        while (1) {
            got = Await_Input(true);
            if (got == IN_ALL) {
                EEPROM_Default();
                goto _RESTART;
            } else if (got == IN_TIMEOUT) {
                goto _TIMEOUT;
            } else if (got == IN_GO) {
                break;
            }
        }
        tone(PIN_TONE, NOTE_G7, 500);
        TV_All("GO!", 0, 0, 0);
        Serial_Score();

// SERVICE
        while (1) {
            got = Await_Input(true);
            if (got == IN_TIMEOUT) {
                goto _TIMEOUT;
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

// HIT
            u32 t1;
            int dt;
            while (1) {
                got = Await_Input(false);
                if (got == IN_TIMEOUT) {
                    goto _TIMEOUT;
                } else if (got == IN_FALL) {
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

            u32 kmh_ = ((u32)36) * S.distance / (dt*10);
                       // prevents overflow
            s8 kmh = min(kmh_, HIT_KMH_MAX);

            static u8 al_cnt = 0;
            u8 al_set = 0;
            u32 left = S.timeout - G.time;
            if (al_cnt==0 && left<S.timeout/2 || al_cnt==1 && left<30000 ||
                al_cnt==2 && left<10000       || al_cnt==3 && left< 5000) {
                al_cnt += 1;
                al_set = 1;
                tone(PIN_TONE, NOTE_C6, 150);
            } else {
                Sound(kmh);
            }

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
            if (!al_set) {
                u32 dt_ = millis() - t1;
                if (HIT_BACK_DT > dt_) {
                    delay(HIT_BACK_DT-dt_);
                }
                if (got == 0) {
                    IS_BACK = (digitalRead(PIN_LEFT)  == LOW);
                } else {
                    IS_BACK = (digitalRead(PIN_RIGHT) == LOW);
                }
                if (IS_BACK) {
                    tone(PIN_TONE, NOTE_C4, 30);
                } else {
                    // desequilibrio
                    u32 p0  = G.ps[0];
                    u32 p1  = G.ps[1];
                    u32 avg = (p0 + p1) / 2;
                    u32 m   = min(p0,p1);
                    if (G.time >= S.timeout/2) {
                        if (kmh>60 && m*11/10<avg) {
                            if (p0>p1 && nxt==0 || p1>p0 && nxt==1) {
                                tone(PIN_TONE, NOTE_C3, 30);
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
    }

_TIMEOUT:
    STATE = STATE_TIMEOUT;
    tone(PIN_TONE, 200, NOTE_C5);
    PT_All();
    TV_All("FIM", 0, 0, 0);
    Serial.println(F("= FIM ="));
    Serial_Score();
    EEPROM_Save();

    while (1) {
        int got = Await_Input(true);
        if (got == IN_FALL) {
            break;
        }
    }

_RESTART:
    tone(PIN_TONE, 200, NOTE_C6);
    S.hit = 0;
    EEPROM_Save();
}
