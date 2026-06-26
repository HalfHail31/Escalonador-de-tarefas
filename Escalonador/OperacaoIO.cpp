#include "OperacaoIO.h"

OperacaoIO::OperacaoIO(int i): id(i), dono(nullptr)
{
}

OperacaoIO::~OperacaoIO()
{
}

bool OperacaoIO::solicitar(Processo* p)
{
    if (dono == nullptr)
    {
        dono = p;

        //std::cout << "[MUTEX] " << p->getId() << " obteve M" << id << std::endl;

        return true;
    }

    fila.push(p);

    //std::cout << "[MUTEX] " << p->getId() << " entrou na fila de espera de M" << id << std::endl;

    return false;
}

Processo* OperacaoIO::liberar(Processo* p)
{
    if (dono != p)
        return nullptr;

    //std::cout << "[MUTEX] " << p->getId() << " liberou M" << id << std::endl;

    if (fila.empty())
    {
        dono = nullptr;
        return nullptr;
    }

    dono = fila.front();
    fila.pop();

    //std::cout << "[MUTEX] " << dono->getId() << " recebeu M" << id << std::endl;
    return dono;
}

void OperacaoIO::reset()
{
    dono = nullptr;

    while (!fila.empty())
        fila.pop();
}

Processo* OperacaoIO::getDono()
{
    return dono;
}

int OperacaoIO::getId()
{
    return id;
}

bool OperacaoIO::estaLivre()
{
    return dono == nullptr;
}

bool OperacaoIO::possuiFilaEspera()
{
    return !fila.empty();
}