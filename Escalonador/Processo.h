#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#define TAM 10


class Sistema;
using namespace::sf;

enum TipoAcao
{
    LOCK,
    UNLOCK
};

struct AcaoMutex
{
    TipoAcao tipo;
    int mutexId;
    int instante;

    bool executada = false;
};

/*struct EstadoAcao
{
    bool executada;
};*/

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
    int prioridadeD;
    int tempoOcioso;
    int inicioOpES;
    int duracaoOpES;
    //std::vector<EstadoAcao> estadoAcoes;
};

class Processo
{
private:
    std::string id; //Identificador definido pelo usu�rio
    sf::Color cor; //Cor definida pelo usu�rio
    sf::Text legenda; //Legenda dos processos
    sf::Font fonte; //Fonte para a legenda
    int ingresso; //Ingresso definido pelo usuário
    int duracao; //Duração definida pelo usuário
    int quantum; //Tempo de duração de um quantum
    int termino; //Tempo em que a tarefa terminou sua execu��o
    bool quantumMax; //A tarefa com essa flag ativa pode ser mudada em escalonamentos que consideram quantum
    int prioridade; //Prioridade definida pelo usu�rio
	int prioridadeD;//Prioridade que pode ser alterada por envelhecimento
    int indiceCPU; //�ndice da CPU recebida para executar
    std::vector<AcaoMutex> acoes;
    bool usaMutex; //Define se o processo faz uso de mutex
    int inicioOperacaoES; //Se o processo faz alguma operação de E/S, ela deve iniciar após a tarefa executar por x segundos
    int duracaoOperacaoES; //Se o processo faz alguma operção de E/S, ela deve encerrar após a tarefa executar por y segundos

    int posX; //Posição em x para o processo ser desenhado, calculado com base em ingresso
    int posY; //Posição em y para o processo ser desenhado, calculada a partir da quantidade de itera��es da fun��o criaSimulador()
    
    bool ativa; //Define se o processo está executando
    bool terminada; //Define se o processo já terminou sua execução
    bool iniciada; //Define se o processo já iniciou, isto é, ingressou no Sistema
    bool suspensa; ///Define se o processo est� suspenso
    bool recebeuMutex; //Define se possui mutex

    int tempoExecutado; //Registra por quanto tempo uma tarefa esteve ativa, para definir se ela j� terminou sua execu��o
    int tempoComCPU; //Para contar o tempo para a passagem do quantum
    int tempoOcioso; //Para processos que ficarem muito tempo ociosos possam aumentar temporariamente sua prioridade

    float timerPasso; //Pequeno delay exclusivo da execu��o passo a passo
    int contaPassos; //Substitui os segundos no modo passo a passo

    std::vector<RectangleShape*> tamanho; //Guarda ret�ngulos para o processo ser exibido maior a cada avan�o no tempo
    std::vector<Estado> historico; //Registra o hist�rico de estados do processo

public:
    Processo(std::string i, unsigned long int c, int ing, int d, int p, int y);
    Processo(std::string i, unsigned long int c, int ing, int d, int p, int y, int iniOpES, int termiOpES);
    ~Processo();

    void desenha(RenderWindow* window);
    void executar(int tempoDecorrido);
    void executarPassos(int tempoDecorrido, int dir);
    void setCor(RectangleShape* rect);
    void prosseguir(int tempoDecorrido);
    void retroceder(int tempoDecorrido);
    void salvarEstado();
    void recuperaEstado();
    void mudaEstadoAtiva();
    void mudaEstadoSuspensa();
    void termina(int tempoDecorrido);
    void adicionaAcao(TipoAcao tipo, int mutexId, int instante);
    void exibeAcoes();

    const bool getAtiva();
    const bool getIniciada();
    const bool getTerminada();
    const int getPrioridade();
	const int getPrioridadeD();
    const int getTempoRestante();
    int getTempoExecutado();
    const int getIngresso();
    const int getDuracao();
    const int getTermino();
    const int getEstadoQuantum();
    const bool getSuspensa();
    const int getTempoComCPU();
    const std::vector<AcaoMutex>& getAcoes();

    void setQuantum(int q);
    void resetaQuantum();
    void recebeCPU(int indice);
    void removeCPU();
    int getIndiceCPU();
    void incrementaTempoOcioso();
    void resetaEnvelhecimento();
    void verificaAcoesMutex(Sistema* sistema);

    std::string getId(); //Fun��es para editar uma tarefa
    void setCor(unsigned long int cor);
    void setIngresso(int ing);
    void setDuracao(int d);
    void setPrioridade(int p);
    void cancelaTermino();
};