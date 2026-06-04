#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace::sf;

class Processo

{

public:
    std::string id; //Identificador definido pelo usuário
    sf::Color cor; //Cor definida pelo usuário
    sf::Text legenda; //Legenda dos processos
    sf::Font fonte; //Fonte para a legenda
    int ingresso; //Ingresso definido pelo usuário
    int duracao; //Duração definida pelo usuário
    int prioridade; //Prioridade definida pelo usuário
    int posX; //Posição em x para o processo ser desenhado, calculado com base em ingresso
    int posY; //Posição em y para o processo ser desenhado, calculada a partir da quantidade de iterações da função criaSimulador()
    bool ativa; //Define se o processo está executando
    bool terminada; //Define se o processo já terminou sua execução
    bool iniciada; //Define se o processo já iniciou, isto é, ingressou no Sistema
    bool suspensa; ///Define se o processo está suspenso
    int tempoExecutado; //Registra por quanto tempo uma tarefa esteve ativa, para definir se ela já terminou sua execução
    int tempoOcioso; //Para processos que ficarem muito tempo ociosos possam aumentar temporariamente sua prioridade
    float timerPasso; //Pequeno delay exclusivo da execução passo a passo
    int contaPassos; //Substitui os segundos no modo passo a passo

    std::vector<RectangleShape*> tamanho; //Guarda retângulos para o processo ser exibido maior a cada avanço no tempo

public:

    Processo(std::string i, unsigned long int c, int ing, int d, int p, int y);

    ~Processo();

    void desenha(RenderWindow* window);
    void executar(int tempoDecorrido);
    void executarPassos(int tempoDecorrido);
    void setCor(RectangleShape* rect);
    void prosseguir(int tempoDecorrido);
    void retroceder();
    void mudaEstado();
    void termina();

    bool getAtiva();
    bool getIniciada();
    bool getTerminada();
    int getPrioridade();
    int getTempoRestante();
    int getIngresso();
    int getDuracao();
};