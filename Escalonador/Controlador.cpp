#include "Controlador.h"

Controlador::Controlador()
{
}

Controlador::~Controlador()
{
}

void Controlador::setSistema(Sistema* s)
{
	sist = s;
	setProcessos();
}

void Controlador::setProcessos()
{
	processos = sist->getProcessos();
}

void Controlador::criaProcesso(std::string id, unsigned long int c, int ing, int d, int p)
{
}

//void Controlador::removeProcesso() {}

void Controlador::editaProcesso(std::string id, unsigned long int c, int ing, int d, int p)
{
	int tam = processos.size();

	for (int i = 0; i < tam; i++)
	{
		if (processos[i]->getId() == id)
		{
			processos[i]->setCor(c);
			processos[i]->setIngresso(ing);
			processos[i]->setDuracao(d);
			processos[i]->setPrioridade(p);
			break;
		}
	}
}

void Controlador::editaSistema(std::string a, int q, int p)
{
}

void Controlador::exibeInformacoes()
{
}
