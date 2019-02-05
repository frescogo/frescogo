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

void Screen (const char* str, int p, int kmh, int is_back) {
    TV.clear_screen();
    //TV.draw_rect(0,0,DX-1,DY-1,WHITE,-1);

    int falls = GAME.falls - (str==NULL ? 1 : 0);

    // KMH
    if (str == NULL) {
        char c = (is_back ? '*' : ' ');
        if (p == 0) {
            sprintf(STR, "--> %c %d %c    ", c, kmh, c);
        } else {
            sprintf(STR, "    %c %d %c <--", c, kmh, c);
        }
        str = STR;
    }
    TV.select_font(font8x8);
    #define K 8
    TV.print(DX/2-K*strlen(str)/2, DY/2-K/2, str);
    TV.select_font(font4x6);

    // TIME/FALLS/PACE
    int time = (GAME.time > TIMEOUT) ? 0 : (TIMEOUT-GAME.time)/1000;
    sprintf(STR, "Tempo:  %3ds", time);
    TV.print(DX-FX*strlen(STR)-1,    0, STR);
    sprintf(STR, "Quedas:  %3d", falls);
    TV.print(DX-FX*strlen(STR)-1,   FY, STR);
    if (GAME.time > 5000) {
        u32 avg   = (GAME.ps[0] + GAME.ps[1]) / 2;
        u32 pace  = avg * 10 / GAME.time;
        sprintf(STR, "Ritmo:   %3d", pace);
    } else {
        sprintf(STR, "Ritmo:  ---");
    }
    TV.print(DX-FX*strlen(STR)-1, 2*FY, STR);

    // BEFORE GET_TOTAL: pace

    // TOTAL
    TV.print(0, DY-2*FY, "TOTAL");
    sprintf(STR, "%5d", Get_Total(falls) / 100);
    TV.print(0, DY-1*FY, STR);

    // AFTER GET_TOTAL: p0/p1

    // ESQ
    TV.print(0, 0, NAMES[0]);
    {
        int n, min_, max_;
        n = Bests(GAME.bests[0][1], &min_, &max_);
        sprintf(STR, "F: %2d (%3d/%3d)", n, max_, min_);
        TV.print(0, 1*FY, STR);
    }
    {
        int n, min_, max_;
        n = Bests(GAME.bests[0][0], &min_, &max_);
        sprintf(STR, "B: %2d (%3d/%3d)", n, max_, min_);
        TV.print(0, 2*FY, STR);
    }
    TV.print(0, 3*FY, GAME.ps[0]/100);

    // DIR
    TV.print(DX-FX*strlen(NAMES[1])-1, DY-1*FY, NAMES[1]);
    {
        int n, min_, max_;
        n = Bests(GAME.bests[1][1], &min_, &max_);
        sprintf(STR, "F: %2d (%3d/%3d)", n, max_, min_);
        TV.print(DX-FX*strlen(STR)-1, DY-2*FY, STR);
    }
    {
        int n, min_, max_;
        n = Bests(GAME.bests[1][0], &min_, &max_);
        sprintf(STR, "B: %2d (%3d/%3d)", n, max_, min_);
        TV.print(DX-FX*strlen(STR)-1, DY-3*FY, STR);
    }
    sprintf(STR, "%ld", GAME.ps[1]/100);
    TV.print(DX-FX*strlen(STR)-1, DY-4*FY, STR);
}

void Dump_Hit (char* name, u32 kmh, bool is_back) {
    pserial.print(F("> "));
    pserial.print(name);
    pserial.print(F(": "));
    pserial.print(kmh);
    if (is_back) {
        pserial.print(F(" !"));
    }
    pserial.println();
}

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

#if 0
code/call Dump (none) -> none do
    {{
        u32 avg  = (GAME.ps[0] + GAME.ps[1]) / 2;
        u32 pace = avg * 10 / GAME.time;

        // BEFORE GET_TOTAL: pace

        u32 total = Get_Total(GAME.falls) / 100;

        // AFTER GET_TOTAL: p0/p1

        pserial.println();
        pserial.println(F("--------------------------------"));
        sprintf_P(STR, PSTR("%15s"), NAMES[0]);
        pserial.print(STR);
        pserial.print(F(" / "));
        sprintf_P(STR, PSTR("%s"), NAMES[1]);
        pserial.print(STR);
        pserial.println();

        pserial.print(F("         ("));
        pserial.print(DISTANCE);
        pserial.print(F("cm - "));
        pserial.print(TIMEOUT/1000);
        pserial.println(F("s)"));

        pserial.println(F("--------------------------------"));
        pserial.println();

        sprintf_P(STR, PSTR("%15S: "), F("TOTAL"));
        pserial.print(STR);
        pserial.print(total);
        if (GAME.time > 5000) {
            pserial.print(F(" ("));
            pserial.print(pace);
            pserial.print(F(")"));
        }
        pserial.println();

        sprintf_P(STR, PSTR("%15S: "), F("Tempo"));
        pserial.print(STR);
        pserial.print(GAME.time);
        pserial.println(F("ms"));

        sprintf_P(STR, PSTR("%15S: "), F("Quedas"));
        pserial.print(STR);
        pserial.println(GAME.falls);

        sprintf_P(STR, PSTR("%15S: "), F("Golpes"));
        pserial.print(STR);
        pserial.println(GAME.hits);

        for (int i=0; i<2; i++) {
            sprintf_P(STR, PSTR("%15s: "), NAMES[i]);
            pserial.print(STR);
            pserial.println(GAME.ps[i]/100);
            for (int j=0; j<2; j++) {
                pserial.print(F(" [ "));
                for (int k=0; k<BESTS; k++) {
                    pserial.print(GAME.bests[i][j][k]);
                    pserial.print(" ");
                }
                pserial.println(F("]"));
            }
            pserial.println();
        }
    }}

    var int i = -1;
    var int bola = 0;
    loop do
        i = i + 1;
        if i == _HIT then
            break;
        end
        var _Hit v = _HITS[i];

        if v.dt == BALL_SERVICE then
            bola = bola + 1;
            _pserial.print(_F("-- Sequencia "));
            {{
                sprintf_P(STR, PSTR("%2d"), @bola);
                pserial.print(STR);
            }}
            _pserial.println(_F(" ----------------"));
        end

        if v.dt == BALL_NONE then
            continue;
        end

        if i % 2 == 0 then
            _pserial.print(_F("         "));
            if v.kmh < 0 then
                _pserial.print(_F("! "));
            else
                _pserial.print(_F("  "));
            end
        else
            _pserial.print(_F("                 "));
        end

        if v.dt == BALL_SERVICE then
            _pserial.println(_F("****"));
        else
            {{
                sprintf_P(STR, PSTR("%4d"), @v.dt);
                pserial.print(STR);
            }}
            if i%2==1 and v.kmh<0 then
                _pserial.print(_F(" !"));
            end
            _pserial.println();
        end
    end
    _pserial.println();
end

code/await Serial (none) -> NEVER do
    loop do
        loop do
            await 1s;
            if _pserial.available() as bool then
                await 1s;
                break;
            end
            //await async do end;
        end

        {{
            int i = 0;
            while (pserial.available()) {
                STR[i++] = pserial.read();
            }
            while (STR[i-1]=='\n' || STR[i-1]=='\r') {
                i--;
            }
            STR[i++] = '\0';
        }}

        var bool ok = do
            if {strncmp_P(STR, PSTR("reinicio"), 8) == 0} as bool then
                await async do
                    emit RESTART;
                end
            else/if {strncmp_P(STR, PSTR("placar"), 6) == 0} as bool then
                call Dump();
            else/if {strncmp_P(STR, PSTR("tempo "), 6) == 0} as bool then
                {{
                    String str = &STR[6];
                    TIMEOUT = str.toInt() * 1000;
                }}
            else/if {strncmp_P(STR, PSTR("dist "), 5) == 0} as bool then
                {{
                    String str = &STR[5];
                    DISTANCE = str.toInt();
                }}
            else/if {strncmp_P(STR, PSTR("esq"), 3) == 0} as bool then
                if {strlen(&STR[3]) < 15} as bool then
                    {strcpy(NAMES[0], &STR[3]);}
                else
                    escape false;
                end
            else/if {strncmp_P(STR, PSTR("dir"), 3) == 0} as bool then
                if {strlen(&STR[3]) < 15} as bool then
                    {strcpy(NAMES[1], &STR[3]);}
                else
                    escape false;
                end
            else/if {strncmp_P(STR, PSTR("-seq"), 4) == 0} as bool then
                if _HIT == 0 then
                    escape false;
                end
                loop do
                    _HIT = _HIT - 1;
                    if _HIT == 0 then
                        break;
                    else/if _HITS[_HIT].dt == BALL_SERVICE then
                        if _HITS[_HIT-1].dt == BALL_NONE then
                            _HIT = _HIT - 1;
                        end
                        break;
                    end
                end
            else/if {strncmp_P(STR, PSTR("+seq"), 4) == 0} as bool then
                if _HITS[_HIT].dt == BALL_MARK then
                    escape false;
                end
                loop do
                    _HIT = _HIT + 1;
                    if _HITS[_HIT].dt==BALL_MARK or _HITS[_HIT].dt==BALL_SERVICE then
                        break;
                    end
                end
            else/if {strncmp_P(STR, PSTR("-1"), 2) == 0} as bool then
                if _HIT > 0 then
                    _HIT = _HIT - 1;
                    if _HITS[_HIT].dt == BALL_NONE then
                        _HIT = _HIT - 1;
                    end
                else
                    escape false;
                end
            else/if {strncmp_P(STR, PSTR("+1"), 2) == 0} as bool then
                if _HITS[_HIT].dt != BALL_MARK then
                    _HIT = _HIT + 1;
                    if _HITS[_HIT].dt == BALL_SERVICE then
                        _HIT = _HIT + 1;    // skip BALL_NONE
                    end
                else
                    escape false;
                end
            else
                escape false;
            end
            escape true;
        end;

        if ok then
            _pserial.println(_F("ok"));
        else
            _pserial.println(_F("err"));
        end
        _ALL();
        _Screen("GO!", 0, 0, 0);
    end
end
#endif

void setup (void) {
    EICRA = 0b1010;     // FALLING for both INT0/INT1

    TV.begin(PAL,DX,DY);
    TV.select_font(font4x6);
    TV.set_hbi_hook(pserial.begin(9600));

#if 0
loop do
    watching RESTART do
#endif

        pserial.println(F("= INICIO ="));
        HIT = 0;
        ALL();
        Screen("INICIO", 0, 0, 0);

            while (1)
            {
                TV.delay(2000);
                TV.tone(3000, 500);
                TV.delay(1000);

                ALL();
                Screen("GO!", 0, 0, 0);

    // SERVICE
                int got;
                    //spawn Serial();
                    while (1) {
                        if (digitalRead(PIN_ESQ) == LOW) {
                            got = 0;
                            break;
                        }
                        if (digitalRead(PIN_DIR) == LOW) {
                            got = 1;
                            break;
                        }
                    }

                u32 t0 = TV.millis();

                if (got != HIT%2) {
                    HITS[HIT++].dt = BALL_NONE;
                }
                HITS[HIT++].dt = BALL_SERVICE;

                TV.tone(500, 30);

                IS_BACK = false;

                pserial.println(F("> saque"));
                Screen("---", 0, 0, 0);
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
                        while (1) {
                            if (digitalRead(PIN_ESQ) == LOW) {
                                got = 0;
                                break;
                            }
                            if (digitalRead(PIN_DIR) == LOW) {
                                got = 1;
                                break;
                            }
                        }

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
                        //Dump_Hit(NAMES[got],   kmh, IS_BACK);
                        //Dump_Hit(NAMES[1-got], kmh, false);
                    } else {
                        //Dump_Hit(NAMES[1-got], kmh, IS_BACK);
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
                    Screen(NULL, 1-got, kmh, IS_BACK);
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
                Screen("QUEDA", 0, 0, 0);
                pserial.println(F("QUEDA"));
                //call Dump();
            }
END:
            TV.tone(200, 2000);

            ALL();
            Screen("FIM", 0, 0, 0);
            //call Dump();
            pserial.println(F("= FIM ="));

        //spawn Serial();
        TV.delay(5000);
}
#if 0

        loop do
            par/or do
                par/or do
                    await INT0;
                with
                    await INT1;
                end
                await 500ms;
            with
                par/and do
                    await INT0;
                with
                    await INT1;
                end
                break;
            end
        end
    end
end
#endif

void loop (void) {}
