#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace::sf;

struct Estado
{
    bool ativa;
    bool terminada;
    bool suspensa;
    bool iniciada;
    int tempoExecutado;
    int tempoComCPU;
    int termino;
    int indiceCPU;
    bool quantumMax;
    int posX;
};

class Processo
{
private:
    std::string id; //Identificador definido pelo usuário
    sf::Color cor; //Cor definida pelo usuário
    sf::Text legenda; //Legenda dos processos
    sf::Font fonte; //Fonte para a legenda
    int ingresso; //Ingresso definido pelo usuário
    int duracao; //Duração definida pelo usuário
    int quantum; //Tempo de duração de um quantum
    int termino; //Tempo em que a tarefa terminou sua execução
    bool quantumMax; //A tarefa com essa flag ativa pode ser mudada em escalonamentos que consideram quantum
    int prioridade; //Prioridade definida pelo usuário
    int indiceCPU; //Índice da CPU recebida para executar
    int posX; //Posição em x para o processo ser desenhado, calculado com base em ingresso
    int posY; //Posição em y para o processo ser desenhado, calculada a partir da quantidade de iterações da função criaSimulador()
    bool ativa; //Define se o processo está executando
    bool terminada; //Define se o processo já terminou sua execução
    bool iniciada; //Define se o processo já iniciou, isto é, ingressou no Sistema
    bool suspensa; ///Define se o processo está suspenso
    int tempoExecutado; //Registra por quanto tempo uma tarefa esteve ativa, para definir se ela já terminou sua execução
    int tempoComCPU; //Para contar o tempo para a passagem do quantum
    int tempoOcioso; //Para processos que ficarem muito tempo ociosos possam aumentar temporariamente sua prioridade

    float timerPasso; //Pequeno delay exclusivo da execução passo a passo
    int contaPassos; //Substitui os segundos no modo passo a passo

    std::vector<RectangleShape*> tamanho; //Guarda retângulos para o processo ser exibido maior a cada avanço no tempo
    std::vector<Estado> historico; //Registra o histórico de estados do processo

public:
    Processo(std::string i, unsigned long int c, int ing, int d, int p, int y);
    ~Processo();

    void desenha(RenderWindow* window);
    void executar(int tempoDecorrido);
    void executarPassos(int tempoDecorrido, int dir);
    void setCor(RectangleShape* rect);
    void prosseguir(int tempoDecorrido);
    void retroceder(int tempoDecorrido);
    void salvarEstado();
    void recuperaEstado();
    void mudaEstado();
    void termina(int tempoDecorrido);

    const bool getAtiva();
    const bool getIniciada();
    const bool getTerminada();
    const int getPrioridade();
    const int getTempoRestante();
    const int getIngresso();
    const int getDuracao();
    const int getTermino();
    const int getEstadoQuantum();
    const bool getSuspensa();
    const int getTempoComCPU();

    void setQuantum(int q);
    void resetaQuantum();
    void recebeCPU(int indice);
    void removeCPU();
    int getIndiceCPU();

    std::string getId(); //Funções para editar uma tarefa
    void setCor(unsigned long int cor);
    void setIngresso(int ing);
    void setDuracao(int d);
    void setPrioridade(int p);
    void cancelaTermino();
};