#pragma once
#include <queue>
#include <vector>

class Processo;

class EscalonadorMutex
{
private:
    int id;
    Processo* dono;
    std::queue<Processo*> fila;

public:
    EscalonadorMutex(int id);
    ~EscalonadorMutex();

    bool solicitar(Processo* p);
    void liberar(Processo* p);
    void reset();

    Processo* getDono();
    int getId();
    bool estaLivre();
    bool possuiFilaEspera();
};

