#pragma once
#include <iostream>
#include "Sistema.h"
#include "Processo.h"

//class Sistema;
//class Processo;

class Controlador
{
private:
	Sistema* sist;
	std::vector<Processo*> processos;
	int y;
	int tempoDecorrido;

public:
	Controlador();
	~Controlador();

	void setSistema(Sistema* s);
	void setProcessos();
	void criaProcesso(std::string id, unsigned long int c, int ing, int d, int p); //Processo sem eventos
	void criaProcesso(std::string id, unsigned long int c, int ing, int d, int p, const std::vector<OperacaoES>& listaES); //Processo com operação E/S
	void criaProcesso(std::string id, unsigned long int c, int ing, int d, int p, const std::vector<AcaoMutex>& acoes); //Processo com MUTEX
	void criaProcesso(std::string id, unsigned long int c, int ing, int d, int p, const std::vector<AcaoMutex>& acoes, const std::vector<OperacaoES>& listaES); //Processo com E/S E MUTEX
	//void removeProcesso();
	void editaProcesso(std::string id, unsigned long int c, int ing, int d, int p);
	void editaSistema(std::string a, int q, int p);
	void exibeInformacoes();
	void recebeEventosMutex();
	void executar(int tempoDecorrido);
};
