int PT_Bests (s8* bests, int* min_, int* max_) {
    *min_ = bests[HITS_BESTS-1];
    *max_ = bests[0];
    for (int i=0; i<HITS_BESTS; i++) {
        if (bests[i] == 0) {
            return i;
        }
    }
    return HITS_BESTS;
}

void PT_Bests_Apply (void) {
    for (int i=0; i<2; i++) {
        for (int j=0; j<2; j++) {
            for (int k=0; k<HITS_BESTS; k++) {
                s8 v = G.bests[i][j][k];
                G.ps[i] += v*v*4;
            }
        }
    }
}

void PT_All (void) {
    G.ps[0] = 0;
    G.ps[1] = 0;
    G.time  = 0;
    G.hits  = 0;
    G.servs = 0;

    memset(G.bests, 0, 2*2*HITS_BESTS*sizeof(s8));

    u32 pace = 0;

    for (int i=0 ; i<S.hit ; i++) {
    //for (int i=0 ; i<600 ; i++) {
        u8  dt1  = S.dts[i];
        s8  kmh1 = G.kmhs[i];
        s8  kmh2 = (kmh1 >= 0 ? kmh1 : -kmh1);
        u16 pt   = ((u16)kmh2)*((u16)kmh2);

        if (dt1 == HIT_SERV) {
            G.servs++;
        }

        if (dt1!=HIT_NONE && dt1!=HIT_SERV) {
            u8 dt2 = S.dts[i+1];
            if (i==S.hit-1 || dt2==HIT_NONE || dt2==HIT_SERV) {
                // ignore last hit
            }
            else
            {
                G.hits++;
                G.ps[1-(i%2)] += pt;
                pace += kmh2;

                // bests
                s8* vec = G.bests[ 1-(i%2) ][ kmh1>0 ];
                for (int j=0; j<HITS_BESTS; j++) {
                    if (kmh2 > vec[j]) {
                        for (int k=HITS_BESTS-1; k>j; k--) {
                            vec[k] = vec[k-1];
                        }
                        vec[j] = kmh2;
                        break;
                    }
                }
            }

            G.time += dt1;
        }
    }
    G.time *= 10;

    G.pace = pace/G.hits;

    PT_Bests_Apply();

    u32 avg   = (G.ps[0] + G.ps[1]) / 2;
    u32 total = min(avg, min(G.ps[0],G.ps[1])*1.1);
    int pct   = 100 - min(100, Falls()*3);
    G.total = total * pct/10000;
}

