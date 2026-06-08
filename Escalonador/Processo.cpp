#include "Processo.h"
#include <iostream>

#define TAM 15

Processo::Processo(std::string i, unsigned long int c, int ing, int d, int p, int y) : id(i), cor((c << 8) | 0xFF), ingresso(ing), duracao(d), prioridade(p),
ativa(false), terminada(false), suspensa(false), iniciada(false), tempoExecutado(0), tempoComCPU(0), termino(-1), indiceCPU(-1)
{
    posX = ingresso * TAM;
    posY = y * TAM * 2;

    if (fonte.loadFromFile("OpenSans.ttf"))
    {
        legenda.setFont(fonte);
        legenda.setString(id);
        legenda.setCharacterSize(TAM);
        legenda.setFillColor(sf::Color::White);
        legenda.setStyle(sf::Text::Bold);
    }
    else
        std::cout << "Erro ao carregar fonte!" << std::endl;
    //std::cout << posX << std::endl;
    //std::cout << "Ola, eu existo e tenho" << "\n" << "id: " << id << " cor: " << c << " ingresso: " << ingresso << " duracao: " << duracao << " prioridade: " << prioridade << std::endl;
}

Processo::~Processo()
{
    int size = (int)tamanho.size();

    for (int i = 0; i < size; i++)
    {
        delete tamanho[i];
    }
}

//Desenha o processo
void Processo::desenha(RenderWindow* window)
{
    int size = (int)tamanho.size();

    for (int i = 0; i < size; i++)
    {
        window->draw(*(tamanho[i]));
    }
    if (iniciada)
    {
        legenda.setPosition(posX + TAM, posY);
        window->draw(legenda);
    }
}

void Processo::executar(int tempoDecorrido)
{
    if (tempoDecorrido < ingresso || terminada) //Não tem porque seguir se o processo ainda não ingressou ou já terminou
        return;

    if (!iniciada)
        iniciada = true;

    if (ativa)
        tempoComCPU++;
    else
        resetaQuantum();
    
    if (tempoComCPU == quantum)
        quantumMax = true;

    //Se ativa e não terminada incrementa o tempoExecutado
    if (ativa && !terminada)
        tempoExecutado++;

    prosseguir(tempoDecorrido);

    if (tempoExecutado >= duracao && !terminada)
        termina(tempoDecorrido);
}

//Função para o modo Por Etapas
void Processo::executarPassos(int tempoDecorrido, int dir)
{
    if (tempoDecorrido < ingresso) //Não tem porque seguir se o processo ainda não ingressou
        return;

    if (!iniciada)
        iniciada = true;

    //Detecta teclas do teclado
    if (dir == -1)
        retroceder(tempoDecorrido);

    if (dir == 1)
        prosseguir(tempoDecorrido);

    if (ativa && dir == 1)
        tempoExecutado++;

    if (tempoExecutado >= duracao && !terminada)
        termina(tempoDecorrido);
}

void Processo::setCor(RectangleShape* rect) //Define a cor do processo
{
    //Se estiver terminada, azul
    if (terminada)
    {
        rect->setFillColor(sf::Color::Blue);
        legenda.setFillColor(sf::Color::Blue);
    }

    if (suspensa) //Se estiver sispensa, preto
    {
        rect->setFillColor(sf::Color::Black);
        legenda.setFillColor(sf::Color::Black);
    }
    else if (ativa) //Se estiver ativa, usa a cor definida pelo usuário
    {
        rect->setFillColor(cor);
        legenda.setFillColor(cor);
    }
    else if (!ativa) //Se estiver inativa, branco
    {
        rect->setFillColor(sf::Color::White);
        legenda.setFillColor(sf::Color::White);
    }
}

void Processo::prosseguir(int tempoDecorrido)
{
    //Cria um novo retângulo para adicionar ao tamanho
    posX = tempoDecorrido * TAM;
    RectangleShape* rect = new RectangleShape;

    rect->setSize(Vector2f(TAM, TAM));
    rect->setPosition(Vector2f(posX, posY));
    setCor(rect);
    tamanho.push_back(rect);
}
/*
void Processo::retroceder(int tempoDecorrido) //Remove o último retângulo e recupera seu estado anterior
{
    if (tamanho.empty()) //Verificação de segurança: Vector vazio
        return;
    if (tempoDecorrido < ingresso) //Uma tarefa que não ingressou não possui um estado anterior
        return;
    if (terminada && tempoDecorrido > termino) //Uma tarefa terminada só precisa retroceder em seu momento de termino
        return;

    int tam = (int)tamanho.size();
    //if (tam < 2) //Não tem para onde retroceder se o Vector não tem mais elementos
       // return;

    RectangleShape* rect = tamanho.back(); //Pega o último elemento do vector

    //Se estiver no momento de término, houve mudança de estado para terminado e devemos desfazê-la ao retroceder
    if (terminada)
    {
        terminada = false;
        termino = -1;
    }
    //Se o atual for "cor" ele executou, e ao retroceder, seu tempo executado deve ser decrementado
    if (rect->getFillColor() == cor)
    {
        if (tam > 2 && tamanho[tam - 2]->getFillColor() != cor) //Se estava inativa na etapa anterior, volta a ficar inativa
            ativa = false;
            
        if (tempoExecutado > 0)
            tempoExecutado--;
    }
    posX -= TAM;
    tamanho.pop_back();
    if(!tamanho.empty())
        setCor(tamanho.back());
    delete rect;
}*/
/*
void Processo::retroceder(int tempoDecorrido)
{
    if (tempoDecorrido < ingresso) return;
    if (tamanho.empty()) return;

    RectangleShape* last = tamanho.back();
    int lastTime = static_cast<int>(last->getPosition().x / TAM);
    if (lastTime <= tempoDecorrido) return; // nada a desfazer

    int tam = (int)tamanho.size();
    sf::Color lastColor = last->getFillColor();
    sf::Color prevColor = (tam >= 2) ? tamanho[tam - 2]->getFillColor() : sf::Color::White;

    // desfaz término se terminou exatamente naquele último passo
    if (terminada && termino == lastTime) {
        terminada = false;
        termino = -1;
    }

    // ajustar tempoExecutado / ativa com base na cor do último e no anterior
    if (lastColor == cor) 
    {
        if (tempoExecutado > 0) tempoExecutado--;
    }
    ativa = (prevColor == cor);

    delete last;
    tamanho.pop_back();
    posX = tamanho.empty() ? ingresso * TAM : tamanho.back()->getPosition().x;
    if (!tamanho.empty()) setCor(tamanho.back());
    else iniciada = false;
}VERSAO DO COPILOT*/

void Processo::retroceder(int tempoDecorrido)
{
    if (tamanho.empty()) return;

    // Remove com segurança os retângulos que estão à frente do tempo atual
    while (!tamanho.empty() && static_cast<int>(tamanho.back()->getPosition().x / TAM) > tempoDecorrido)
    {
        RectangleShape* last = tamanho.back();
        int lastTime = static_cast<int>(last->getPosition().x / TAM);

        if (terminada && termino == lastTime) {
            terminada = false;
            termino = -1;
        }

        // Se o que estamos apagando era um bloco executado, devolve o tempo
        if (last->getFillColor() == cor)
        {
            if (tempoExecutado > 0) tempoExecutado--;
        }

        delete last;
        tamanho.pop_back();
    }

    // Recalcula o estado com base no novo topo do vetor (se ainda houver elementos)
    int tamAtual = (int)tamanho.size();
    if (tamAtual > 0)
    {
        posX = tamanho.back()->getPosition().x;
        sf::Color prevColor = tamanho.back()->getFillColor();
        ativa = (prevColor == cor);
        setCor(tamanho.back());
    }
    else
    {
        // Se limpou tudo, reseta o estado inicial do processo
        posX = ingresso * TAM;
        iniciada = false;
        ativa = false;
        removeCPU();
    }
}//VERSAO DO GEMINI

void Processo::mudaEstado()
{
    ativa = !ativa;
}

void Processo::termina(int tempoDecorrido)
{
    terminada = true;
    termino = tempoDecorrido;
    //std::cout << "terminado" << termino << std::endl;
}

bool Processo::getAtiva()
{
    return ativa;
}

bool Processo::getIniciada()
{
    return iniciada;
}

bool Processo::getTerminada()
{
    return terminada;
}

int Processo::getPrioridade()
{
    return prioridade;
}

int Processo::getTempoRestante()
{
    return duracao - tempoExecutado;
}

int Processo::getIngresso()
{
    return ingresso;
}

int Processo::getDuracao()
{
    return duracao;
}

int Processo::getTermino()
{
    return termino;
}

int Processo::getEstadoQuantum()
{
    return quantumMax;
}

void Processo::setQuantum(int q)
{
    quantum = q;
}

void Processo::resetaQuantum()
{
    quantumMax = false;
    tempoComCPU = 0;
}

void Processo::recebeCPU(int indice)
{
    indiceCPU = indice;
}

void Processo::removeCPU()
{
    indiceCPU = -1;
}

int Processo::getIndiceCPU()
{
    return indiceCPU;
}

std::string Processo::getId()
{
    return id;
}

void Processo::setCor(unsigned long int cor)
{
    sf::Color c((cor << 8) | 0xFF);
    this->cor = c;
}

void Processo::setIngresso(int ing)
{
    ingresso = ing;
}

void Processo::setDuracao(int d)
{
    duracao = d;
}

void Processo::setPrioridade(int p)
{
    prioridade = p;
}
