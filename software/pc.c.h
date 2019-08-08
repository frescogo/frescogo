enum {
    PC_RESTART = 0,
    PC_HIT     = 1,
    PC_TICK    = 2,
    PC_FALL    = 3,
    PC_END     = 4
};

void PC_Restart (void) {
    Serial.println(PC_RESTART);         // codigo de reinicio
    Serial.println(S.timeout/1000);     // tempo total de jogo
    Serial.println(S.names[0]);         // atleta a esquerda
    Serial.println(S.names[1]);         // atleta a direita
}

void PC_Player (int I) {
    Serial.println(G.ps[I]/100);        // pontuacao
    Serial.println(PT_Behind() == I ? 1 : 0);   // 1=atras | 0=ok

    int n, min_, max_;
    n = PT_Bests(G.bests[I][0], &min_, &max_);
    Serial.println(n);                  // total de revezes
    Serial.println(max_);               // maxima de revez
    n = PT_Bests(G.bests[I][1], &min_, &max_);
    Serial.println(n);                  // total normais
    Serial.println(max_);               // maxima normal
}

void PC_Hit (int player, int is_back, int kmh) {
    Serial.println(PC_HIT);             // codigo de golpe
    Serial.println(player);             // 0=esquerda | 1=direita
    Serial.println(is_back);            // 0=normal   | 1=revez
    Serial.println(kmh);                // velocidade
    PC_Player(player);
}

void PC_Tick (void) {
    Serial.println(PC_TICK);            // codigo de tick
    Serial.println(G.time);             // tempo jogado em ms
    Serial.println(G.total);            // total da dupla
}

void PC_Fall (void) {
    Serial.println(PC_FALL);            // codigo de queda
    Serial.println(Falls());            // total de quedas
}

void PC_End (void) {
    Serial.println(PC_END);             // codigo de fim
}

void PC_Nop (void) {
}
