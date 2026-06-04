#include "Processo.h"
#include <iostream>

#define TAM 21

Processo::Processo(std::string i, unsigned long int c, int ing, int d, int p, int y) : id(i), cor((c << 8) | 0xFF), ingresso(ing), duracao(d), prioridade(p),
ativa(false), terminada(false), suspensa(false), iniciada(false), tempoExecutado(0)
{
    posX = ingresso * TAM;
    posY = y * TAM * 2;

    if (!fonte.loadFromFile("OpenSans.ttf"))
        std::cout << "Erro ao carregar fonte!" << std::endl;
    legenda.setFont(fonte);
    legenda.setString(id);
    legenda.setCharacterSize(TAM);
    legenda.setFillColor(sf::Color::White);
    legenda.setStyle(sf::Text::Bold);

    std::cout << posX << std::endl;
    std::cout << "Ola, eu existo e tenho" << "\n" << "id: " << id << " cor: " << c << " ingresso: " << ingresso << " duracao: " << duracao << " prioridade: " << prioridade << std::endl;
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
    legenda.setPosition(posX + TAM, posY);
    window->draw(legenda);
}

void Processo::executar(int tempoDecorrido)
{
    if (tempoDecorrido < ingresso) //Não tem porque seguir se o processo ainda não ingressou
        return;

    if (!iniciada)
        iniciada = true;

    //Se ativada e não terminada, executa e adiciona ao tempo de execução
    if (ativa && !terminada)
        tempoExecutado++;

    prosseguir(tempoDecorrido);

    if (tempoExecutado >= duracao && !terminada)
        termina();
}

//Função para o modo Por Etapas
void Processo::executarPassos(int tempoDecorrido)
{
    if (tempoDecorrido < ingresso) //Não tem porque seguir se o processo ainda não ingressou
        return;

    if (!iniciada)
        iniciada = true;

    //Detecta teclas do teclado
    if (Keyboard::isKeyPressed(Keyboard::Left))
        retroceder();

    if (Keyboard::isKeyPressed(Keyboard::Right))
        prosseguir(tempoDecorrido);

    if (ativa && !terminada)
        tempoExecutado++;

    if (tempoExecutado >= duracao && !terminada)
        termina();
}

//Define a cor do processo
void Processo::setCor(RectangleShape* rect)
{
    //Se estiver terminada, azul
    if (terminada)
    {
        rect->setFillColor(sf::Color::Blue);
        legenda.setFillColor(sf::Color::Blue);
    }

    else if (suspensa)
    {
        rect->setFillColor(sf::Color::Black);
        legenda.setFillColor(sf::Color::Black);
    }

    //Se estiver ativa, a cor definida pelo usuário
    else if (ativa)
    {
        rect->setFillColor(cor);
        legenda.setFillColor(cor);
    }

    //Se estiver inativa, branco
    else if (!ativa)
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

void Processo::retroceder() //Função incompleta, remove o último retângulo e recupera seu estado anterior
{
    if (!tamanho.empty())
    {
        int tam = (int)tamanho.size();

        if (tam < 2)
            return;
        RectangleShape* rect = tamanho.back();

        //Se o atual for azul, e o anterior não, houve mudança de estado para terminado.
        if (rect->getFillColor() == sf::Color::Blue && tamanho[tam - 2]->getFillColor() != sf::Color::Blue && terminada == true)
            terminada = false;
        else if (rect->getFillColor() == cor && tamanho[tam - 2]->getFillColor() != cor) {
            ativa = false;
            tempoExecutado--;
        }
        //Se o atual for verde, ele executou, e ao retroceder, seu tempo executado deve ser decrementado
        else if (rect->getFillColor() == cor && terminada == false)
            tempoExecutado--;
        setCor(rect);
        posX -= TAM;
        tamanho.pop_back();
        delete rect;
    }
}

void Processo::mudaEstado()
{
    ativa = !ativa;
}

void Processo::termina()
{
    terminada = true;
    std::cout << "terminado" << std::endl;
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