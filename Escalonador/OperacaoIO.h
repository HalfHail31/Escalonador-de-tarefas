#pragma once

#include <iostream>
#include <queue>
#include "Processo.h"

class OperacaoIO
{
private:
    int id;
    Processo* dono;
    std::queue<Processo*> fila;

public:
    OperacaoIO(int i);
    ~OperacaoIO();

    bool solicitar(Processo* p);
    Processo* liberar(Processo* p);
    void reset();
    Processo* getDono();
    int getId();
    bool estaLivre();
    bool possuiFilaEspera();
};

