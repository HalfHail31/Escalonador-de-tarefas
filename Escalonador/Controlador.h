#pragma once
#include "Sistema.h"
#include "Processo.h"

//class Sistema;
//class Processo;

class Controlador
{
private:
	Sistema* sist;
	std::vector<Processo*> processos;

public:
	Controlador();
	~Controlador();

	void setSistema(Sistema* s);
	void setProcessos();
	void criaProcesso(std::string id, unsigned long int c, int ing, int d, int p);
	//void removeProcesso();
	void editaProcesso(std::string id, unsigned long int c, int ing, int d, int p);
	void editaSistema(std::string a, int q, int p);
	void exibeInformacoes();
	void executar(int tempoDecorrido);
};

