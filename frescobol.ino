#include <TVout.h>
#include <pollserial.h>
#include <fontALL.h>

typedef char  s8;
typedef short s16;
typedef unsigned long u32;

#define PIN_ESQ 21
#define PIN_DIR 20

#define DX  184
#define DY   64
#define FX    4
#define FY    6

#define BALL_MARK    0
#define BALL_NONE    1
#define BALL_SERVICE 2

#define BESTS 10

#define HIT_AWAIT 100       // desired await between two hits

#define KMH_MAX 125         // to fit in s8

TVout TV;
pollserial pserial;

typedef struct {
    u8 dt;                      // cs (ms*10)
    s8 kmh;                     // +/-kmh (max 125km/h)
} Hit;
Hit  HITS[1000];
int  HIT = 0;

char NAMES[2][20] = { "Atleta ESQ", "Atleta DIR" };

u32  TIMEOUT  = 300 * ((u32)1000);
int  DISTANCE = 800;

bool IS_BACK;

char STR[32];

typedef struct {
    s8  bests[2][2][BESTS];     // kmh (max 125kmh/h)
    u32 ps[2];                  // sum(kmh*kmh)
    u32 time;                   // ms (total time)
    u16 hits;
    u8  falls;
} Game;
Game GAME;

int Bests (s8* bests, int* min_, int* max_) {
    *min_ = bests[BESTS-1];
    *max_ = bests[0];
    for (int i=0; i<BESTS; i++) {
        if (bests[i] == 0) {
            return i;
        }
    }
    return BESTS;
}

void Bests_Apply (void) {
    for (int i=0; i<2; i++) {
        for (int j=0; j<2; j++) {
            for (int k=0; k<BESTS; k++) {
                s8 v = GAME.bests[i][j][k];
                GAME.ps[i] += v*v*4;
            }
        }
    }
}

u32 Get_Total (int falls) {
    Bests_Apply();
    u32 avg   = (GAME.ps[0] + GAME.ps[1]) / 2;
    u32 total = min(avg, min(GAME.ps[0],GAME.ps[1])*1.1);
    int pct   = 100 - min(100, (falls)*3);
    return total * pct/100;
}

void ALL (void) {
    GAME.ps[0] = 0;
    GAME.ps[1] = 0;
    GAME.time  = 0;
    GAME.hits  = 0;
    GAME.falls = 0;

    memset(GAME.bests, 0, 2*2*BESTS*sizeof(s8));

    for (int i=0 ; i<HIT ; i++) {
    //for (int i=0 ; i<600 ; i++) {
        Hit v = HITS[i];
        u16 kmh = (v.kmh >= 0 ? v.kmh : -v.kmh);
        u16 pt  = kmh*kmh;

        if (v.dt != BALL_NONE) {
            GAME.hits++;
        }

        if (v.dt == BALL_SERVICE) {
            GAME.falls++;
        }

        if (v.dt!=BALL_NONE && v.dt!=BALL_SERVICE) {
            Hit nxt = HITS[i+1];
            if (i==HIT-1 || nxt.dt==BALL_NONE || nxt.dt==BALL_SERVICE) {
                // ignore last hit
            }
            else
            {
                // ps
                GAME.ps[1-(i%2)] += pt;

                // bests
                s8* vec = GAME.bests[ 1-(i%2) ][ v.kmh>0 ];
                for (int j=0; j<BESTS; j++) {
                    if (kmh > vec[j]) {
                        for (int k=BESTS-1; k>j; k--) {
                            vec[k] = vec[k-1];
                        }
                        vec[j] = kmh;
                        break;
                    }
                }
            }

            GAME.time += v.dt*10;
        }
    }
}

void TV_All (const char* str, int p, int kmh, int is_back);
#include "tv.c.h"

void Serial_Hit (char* name, u32 kmh, bool is_back);
void Serial_All (void);
int  Serial_Check (void);
#include "serial.c.h"

void Sound (u32 kmh) {
    if (kmh < 40) {
        TV.tone( 500, 30);
    } else if (kmh < 50) {
        TV.tone(1500, 30);
    } else if (kmh < 60) {
        TV.tone(2500, 30);
    } else if (kmh < 70) {
        TV.tone(3500, 30);
    } else {
        TV.tone(4500, 30);
    }
}

int AWAIT_PRESS (bool serial) {
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
    EICRA = 0b1010;     // FALLING for both INT0/INT1

    TV.begin(PAL,DX,DY);
    TV.select_font(font4x6);
    TV.set_hbi_hook(pserial.begin(9600));
}

void loop (void)
{
    pserial.println(F("= INICIO ="));
    HIT = 0;
    ALL();
    TV_All("INICIO", 0, 0, 0);

    while (1)
    {
        TV.delay(2000);
        TV.tone(3000, 500);
        TV.delay(1000);

        ALL();
        TV_All("GO!", 0, 0, 0);

// SERVICE
        int got = AWAIT_PRESS(true);
        if (got == -1) {
            return;         // restart
        }

        u32 t0 = TV.millis();

        if (got != HIT%2) {
            HITS[HIT++].dt = BALL_NONE;
        }
        HITS[HIT++].dt = BALL_SERVICE;

        TV.tone(500, 30);

        IS_BACK = false;

        pserial.println(F("> saque"));
        TV_All("---", 0, 0, 0);
        TV.delay(HIT_AWAIT);

        int nxt = 1 - got;
        while (1) {
            // both are unpresseed?
            bool both = digitalRead(PIN_ESQ) == HIGH &&
                        digitalRead(PIN_DIR) == HIGH;

// HIT
            // debounce
            u32 t1;
            int dt;
            while (1) {
                got = AWAIT_PRESS(false);

                t1 = TV.millis();
                dt = (t1 - t0);
                if (got!=nxt && dt<500) {
                    continue;
                }

                TV.delay(50);
                if (got==0 && digitalRead(PIN_ESQ)==LOW) {
                    break;
                } else if (got==1 && digitalRead(PIN_DIR)==LOW) {
                    break;
                }
            }
            //ceu_arduino_assert(dt>50, 2);

            t0 = t1;

// FALL
            // if both were unpressed and now both are pressed,
            // and its long since the previous hit, then this is a fall
            if (dt > 1000 ) {
                TV.delay(100);
                if (both && digitalRead(PIN_ESQ)==LOW &&
                            digitalRead(PIN_DIR)==LOW)
                {
                    break;
                }
            }

            if (nxt != got) {
                dt = dt / 2;
            }

            u32 kmh_ = ((u32)36) * DISTANCE / dt;
                       // prevents overflow
            s16 kmh = min(kmh_, KMH_MAX);
            Sound(kmh);

            if (nxt != got) {
                Serial_Hit(NAMES[got],   kmh, IS_BACK);
                Serial_Hit(NAMES[1-got], kmh, false);
            } else {
                Serial_Hit(NAMES[1-got], kmh, IS_BACK);
            }

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

//var u32 x1 = _TV.millis();
            ALL();
//var u32 x2 = _TV.millis();
//_pserial.print("> ");
//_pserial.println(x2-x1);
            TV_All(NULL, 1-got, kmh, IS_BACK);
            if (GAME.time >= TIMEOUT) {
                goto END;
            }

            // sleep inside hit to reach HIT_AWAIT and check BACK below
            u32 dt_ = TV.millis() - t1;
            if (HIT_AWAIT > dt_) {
                TV.delay(HIT_AWAIT-dt_);
            }

            if (got == 0) {
                IS_BACK = (digitalRead(PIN_ESQ) == LOW);
            } else {
                IS_BACK = (digitalRead(PIN_DIR) == LOW);
            }
            if (IS_BACK) {
                TV.tone(200, 30);
            }
        }

        TV.tone(300, 100);
        TV.delay(150);
        TV.tone(200, 100);
        TV.delay(150);
        TV.tone(100, 200);
        TV.delay(200);

        ALL();
        TV_All("QUEDA", 0, 0, 0);
        pserial.println(F("QUEDA"));
        Serial_All();
    }

END:
    TV.tone(200, 2000);
    ALL();
    TV_All("FIM", 0, 0, 0);
    Serial_All();
    pserial.println(F("= FIM ="));
    TV.delay(5000);

    while (1) {
        int got = AWAIT_PRESS(true);
        if (got == -1) {
            return;         // restart
        }
        TV.delay(1000);
        if (digitalRead(PIN_ESQ)==LOW && digitalRead(PIN_DIR)==LOW) {
            break;
        }
    }
}
