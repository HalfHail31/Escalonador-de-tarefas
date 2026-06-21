#pragma once

#include "Processo.h"

#include <vector>

#include <string>

#define ALTURA_TELA 800
#define LARGURA_TELA 1300
#define RAIO_PROCESSADOR 20

class Sistema
{
private:
    std::string algoritmo; //Algoritmo definido pelo usu�rio
    int quantum; //Quantum definido pelo usu�rio
    int CPUs; //N�mero de processadores definido pelo usu�rio
    int CPUsLivres; //N�mero de processadores livres
    float tempo; //Tempo decorrido para o quantum
    bool porEtapas; //Define se est� no modo Por Etapas
    sf::RenderWindow* janela; //Janela de exibi��o
    sf::Text legenda; //Legenda para os processadores
    sf::Font fonte; //Fonte para a legenda
    std::vector<Processo*> processos; //Lista de processos
    std::vector<sf::CircleShape*> processadores; //Lista de processadores

public:
    Sistema(std::string a, int q, int p);
    ~Sistema();

    void recebeProcesso(Processo* p);

    void cedeProcessador(Processo* p);
    void retiraProcessador(Processo* p);
    void desativaProcessadores();
    void setJanela(sf::RenderWindow* w);

    void executarPRIOp(int tempoDecorrido);
    void executarSRTF(int tempoDecorrido);
    void executar(int tempoDecorrido, int dir);
    void verificaEscalonamento(int tempoDecorrido);
    bool desempate(Processo* atual, Processo* candidato);

    void setAlgoritmo(std::string alg);/////////////////////////////////////////
    void setQuantum(int q); ////////////////Para editar o sistema///////////////
    void setProcessadores(int c); //////////////////////////////////////////////

    const std::string getAlgoritmo(); //////////////////////////////////////////
    const int getNumCPUs(); ////////////////////////////////////////////////////
    const int getNumCPUsLivres(); /////////Para conferir dados do sistema///////
    const int getQuantum(); ////////////////////////////////////////////////////

    void desenhaProcessos();
    void desenhaProcessadores();
    void mudaModo();
    bool getPorEtapas();
    void simulaEtapas(int tempoDecorrido);
    std::vector<Processo*> getProcessos();
};