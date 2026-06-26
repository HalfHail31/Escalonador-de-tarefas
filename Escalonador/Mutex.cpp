#include "Mutex.h"
#include "Processo.h"
#include <iostream>

EscalonadorMutex::EscalonadorMutex(int id) : id(id), dono(nullptr)
{
}

EscalonadorMutex::~EscalonadorMutex()
{
	delete dono;
}

bool EscalonadorMutex::solicitar(Processo* p)
{
    if (dono == nullptr)
    {
        dono = p;

        std::cout << "[MUTEX] " << p->getId() << " obteve M" << id << std::endl;

        return true;
    }

    fila.push(p);

	std::cout
		<< "[MUTEX] " << p->getId() << " entrou na fila de espera de M" << id << std::endl;

    return false;
}

void EscalonadorMutex::liberar(Processo* p)
{
    if (dono != p)
        return;

	std::cout << "[MUTEX] " << p->getId() << " liberou M" << id << std::endl;

    if (fila.empty())
    {
        dono = nullptr;
        return;
    }

    dono = fila.front();
    fila.pop();

    std::cout << "[MUTEX] " << dono->getId() << " recebeu M" << id << std::endl;
}

void EscalonadorMutex::reset()
{
    dono = nullptr;

    while (!fila.empty())
        fila.pop();
}

Processo* EscalonadorMutex::getDono()
{
    return dono;
}

int EscalonadorMutex::getId()
{
    return id;
}

bool EscalonadorMutex::estaLivre()
{
	return dono == nullptr;
}

bool EscalonadorMutex::possuiFilaEspera()
{
    return !fila.empty();
}
