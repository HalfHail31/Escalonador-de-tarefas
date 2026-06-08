#include "Sistema.h"
#include <iostream>

Sistema::Sistema(std::string a, int q, int p) : algoritmo(a), quantum(q), CPUs(p), CPUsLivres(p), tempo(0), porEtapas(true)
{
    std::cout << algoritmo << std::endl;
    std::cout << "Ola, sou um sistema operacional de " << CPUs << " processadores e quantum de " << quantum << "s\n";

    for (int i = CPUs; i > 0; i--) //Cria processadores
    {
        sf::CircleShape* circulo = new sf::CircleShape;
        circulo->setRadius(RAIO_PROCESSADOR);
        circulo->setFillColor(sf::Color::Green);
        circulo->setPosition(LARGURA_TELA - RAIO_PROCESSADOR * (i * 2) - 5 * i, ALTURA_TELA - 2.5 * RAIO_PROCESSADOR);
        processadores.push_back(circulo);
    }

    //Cria legenda
    sf::Vector2f posUltimaCPU = processadores[0]->getPosition();
    if (!fonte.loadFromFile("OpenSans.ttf"))
        std::cout << "Erro ao carregar fonte!" << std::endl;
    legenda.setFont(fonte);
    legenda.setString("Processadores:");
    legenda.setCharacterSize(18);
    legenda.setFillColor(sf::Color::Black);
    legenda.setStyle(sf::Text::Bold);
    legenda.setPosition(posUltimaCPU.x - 150, posUltimaCPU.y + 10);
}

Sistema::~Sistema()
{
    int size = (int)processadores.size();
    int size2 = (int)processos.size();

    for (int i = 0; i < size; i++)
    {
        delete processadores[i];
    }
    for (int i = 0; i < size2; i++)
    {
        delete processos[i];
    }
}

void Sistema::recebeProcesso(Processo* p)
{
    if (p)
    {
        p->setQuantum(quantum);
        processos.push_back(p);
    }
}

void Sistema::cedeProcessador(Processo* p)
{
    sf::Color cinza(0x898989FF);

    if (CPUsLivres > 0 && p->getAtiva() == false)
    {
        //Busca por processador livre ou inativo para cedê-lo
        for (int i = 0; i < CPUs; i++)
        {
            if (processadores[i]->getFillColor() == sf::Color::Green || processadores[i]->getFillColor() == cinza) 
            {
                p->mudaEstado();
                p->recebeCPU(i);
                processadores[i]->setFillColor(sf::Color::Red);
                CPUsLivres--;
                break;
            }
        }
    }
}

void Sistema::retiraProcessador(Processo* p)
{
    if (p->getAtiva() == true)
    {
        processadores[p->getIndiceCPU()]->setFillColor(sf::Color::Green);
        p->mudaEstado();
        p->removeCPU();
        CPUsLivres++;
    }
}

void Sistema::desativaProcessadores()
{
    sf::Color cinza(0x898989FF);

    for (int i = 0; i < CPUs; i++) {
        if (processadores[i]->getFillColor() == sf::Color::Green)
        {
            processadores[i]->setFillColor(cinza);
            break;
        }
    }
}

void Sistema::setJanela(sf::RenderWindow* w)
{
    if (w)
        janela = w;
}

void Sistema::executarPRIOp(int tempoDecorrido)
{
    if (processos.empty()) //Não continua se não existirem processos
        return;

    int tam = processos.size();

    for (int i = 0; i < tam; i++) 
    {
        if (processos[i]->getTerminada() && processos[i]->getAtiva()) //Retira CPUs cedidas a processos que já terminaram
        {
            retiraProcessador(processos[i]);
            processos[i]->resetaQuantum();
        }
    }

    for (int i = 0; i < tam; i++)
    {
        if (processos[i]->getAtiva() && processos[i]->getEstadoQuantum()) //Retira CPUs cedidas a processos que já passaram por um quantum
        {
            retiraProcessador(processos[i]);
            processos[i]->resetaQuantum();
        }
    }

    for (int i = 0; i < CPUs; i++) //Itera para cada CPU
    {
        int prioritario = -1;

        for (int j = 0; j < tam; j++) //Percorre a lista de processos
        {
            //Busca os processos iniciados e sem CPUs
            if (tempoDecorrido >= processos[j]->getIngresso() && !processos[j]->getTerminada() && !processos[j]->getAtiva())
            {
                if (prioritario == -1 || processos[j]->getPrioridade() > processos[prioritario]->getPrioridade())
                {
                    prioritario = j;
                }
                if (prioritario != j && processos[prioritario]->getPrioridade() == processos[j]->getPrioridade())
                    if (desempate(processos[prioritario], processos[j]))
                        prioritario = j;

            }
        }

        if (prioritario == -1) //Nenhum processo precisa de CPU no momento
        {
            desativaProcessadores();
            break;
        }
        
        if (CPUsLivres > 0) //Se tiver CPUs livres, pode ceder uma para o prioritário
            cedeProcessador(processos[prioritario]);

        else if (CPUsLivres == 0)// && inativos > 0)//Só pode retirar CPUs se nenhuma CPU estiver livre e existirem processos inativos
        {
            int menorPrioridade = -1;

            for (int j = 0; j < tam; j++) //Percorre a lista de processos
            {
                if (processos[j]->getAtiva()) //Busca os processos ativos
                {
                    //Busca aquele com a menor prioridade
                    if (menorPrioridade == -1 || processos[j]->getPrioridade() < processos[menorPrioridade]->getPrioridade())
                        menorPrioridade = j;
                }
            }
            
            if (menorPrioridade != -1 && processos[prioritario]->getPrioridade() > processos[menorPrioridade]->getPrioridade())
            {
                //Se a menor prioridade for menor que o prioritário, seu processador será removido e entregue ao prioritário
                retiraProcessador(processos[menorPrioridade]);
                cedeProcessador(processos[prioritario]);
            }
        }
    }
}

void Sistema::executarSRTF(int tempoDecorrido)
{
    //tempo += dt;

    if (processos.empty())
        return;

    int tam = processos.size();

    //Remove CPUs de processos terminados
    for (int i = 0; i < tam; i++)
    {
        if (processos[i]->getTerminada() && processos[i]->getAtiva())
            retiraProcessador(processos[i]);
    }

    //Para cada CPU
    for (int cpu = 0; cpu < CPUs; cpu++)
    {
        int menor = -1;

        //Busca o processo com menor tempo restante
        for (int i = 0; i < tam; i++)
        {
            if (tempoDecorrido >= processos[i]->getIngresso() &&
                !processos[i]->getTerminada() && !processos[i]->getAtiva())
            {
                if (menor == -1 || desempate(processos[menor], processos[i]))
                    menor = i;
            }
        }

        //Nenhum processo disponível
        if (menor == -1)
        {
            desativaProcessadores();
            break;
        }

        //CPU livre
        if (CPUsLivres > 0)
        {
            if (!processos[menor]->getAtiva())
                cedeProcessador(processos[menor]);
        }
    }
}

void Sistema::executar(int tempoDecorrido, int dir)
{
    if (processos.empty()) //Não continua se não existirem processos
        return;

    int tam = processos.size();
    int inativos = 0;

    for (int i = 0; i < tam; i++) //Verifica se é necessário escalonar
    {
        verificaEscalonamento(processos[i], tempoDecorrido);
    }

    //Se por etapas, utiliza a própria função
    //if (porEtapas)
    //    simulaEtapas(tempoDecorrido);
    //else
    //{
    //    for (int i = 0; i < tam; i++)
    //        if (!processos[i]->getTerminada()) //Não precisa mais executar uma tarefa que já terminou
    //            processos[i]->executar(tempoDecorrido);
    //}

    for (int i = 0; i < tam; i++)
    {
        if (porEtapas)
        {
            if (dir == -1)
                processos[i]->retroceder(tempoDecorrido);
            else if (!processos[i]->getTerminada()) //Uma tarefa terminada não precisa executar
                processos[i]->executar(tempoDecorrido);
        }
        else
        {
            if (!processos[i]->getTerminada())
                processos[i]->executar(tempoDecorrido);
        }
    }
}

void Sistema::verificaEscalonamento(Processo* p, int tempoDecorrido) //Verifica se é necessário fazer um escalonamento
{
    //Se a tarefa acabou de ingressar ou terminar, ou já executou durante o tempo de um quantum, devemos chamar o escalonamento
    if (p->getIngresso() == tempoDecorrido || p->getTermino() == tempoDecorrido || p->getEstadoQuantum())
    {
        if (algoritmo == "priop") //Escalonamento PRIOp
        {
            std::cout << "Escalonando com PRIOp" << std::endl;
            executarPRIOp(tempoDecorrido);
        }
        else if (algoritmo == "srtf") //Escalonamento SRTF
        {
            std::cout << "Escalonando com SRTF" << std::endl;
            executarSRTF(tempoDecorrido);
        }
    }
}

bool Sistema::desempate(Processo* atual, Processo* candidato)
{
    if (atual == nullptr)
        return true;

    // Desempate (1)
    // Processo com quantum completo
    std::cout << "Empate" << std::endl;
    std::cout << "Mantendo aquele que nao passou um quantum." << std::endl;
    if (!candidato->getEstadoQuantum() && atual->getEstadoQuantum())
        return true;

    if (candidato->getEstadoQuantum() && !atual->getEstadoQuantum())
        return false;

    // Desempate (2)
    // Processo já executando
    std::cout << "Mantendo o ja ativo." << std::endl;
    if (candidato->getAtiva() && !atual->getAtiva())
        return true;

    if (!candidato->getAtiva() && atual->getAtiva())
        return false;

    // Desempate (3)
    // Menor ingresso
    std::cout << "Mantendo o de menor ingresso." << std::endl;
    if (candidato->getIngresso() < atual->getIngresso())
        return true;

    if (candidato->getIngresso() > atual->getIngresso())
        return false;

    // Desempate (4)
    // Menor duração
    std::cout << "Mantendo o de menor duracao." << std::endl;
    if (candidato->getDuracao() < atual->getDuracao())
        return true;

    if (candidato->getDuracao() > atual->getDuracao())
        return false;

    // Desempate (5)
    // Sorteio
    std::cout << "Sorteando aleatorio." << std::endl;
    return rand() % 2;
}

void Sistema::setAlgoritmo(std::string alg)
{
    // Converte para minúsculo
    std::transform(
        alg.begin(),
        alg.end(),
        alg.begin(),
        ::tolower
    );
    algoritmo = alg;
}

void Sistema::setQuantum(int q)
{
    if (q > 0)
        quantum = q;
}

void Sistema::setProcessadores(int c)
{
    if (c > 0)
    {
        CPUs = c;

        int size = (int)processadores.size();
        for (int i = 0; i < size; i++)
            delete processadores[i];
        
        for (int i = CPUs; i > 0; i--) //Recria processadores
        {
            sf::CircleShape* circulo = new sf::CircleShape;
            circulo->setRadius(RAIO_PROCESSADOR);
            circulo->setFillColor(sf::Color::Green);
            circulo->setPosition(LARGURA_TELA - RAIO_PROCESSADOR * (i * 2) - 5 * i, ALTURA_TELA - 2.5 * RAIO_PROCESSADOR);
            processadores.push_back(circulo);
        }
        //Recria legenda
        sf::Vector2f posUltimaCPU = processadores[0]->getPosition();
        if (!fonte.loadFromFile("OpenSans.ttf"))
            std::cout << "Erro ao carregar fonte!" << std::endl;
        legenda.setFont(fonte);
        legenda.setString("Processadores:");
        legenda.setCharacterSize(18);
        legenda.setFillColor(sf::Color::Black);
        legenda.setStyle(sf::Text::Bold);
        legenda.setPosition(posUltimaCPU.x - 150, posUltimaCPU.y + 10);
    }
}

std::string Sistema::getAlgoritmo()
{
    return algoritmo;
}

void Sistema::desenhaProcessos()
{
    int tam = processos.size();
    for (int i = 0; i < tam; i++)
        processos[i]->desenha(janela);
}

void Sistema::desenhaProcessadores()
{
    for (int i = 0; i < CPUs; i++) 
    {
        janela->draw(*(processadores[i]));
    }
    janela->draw(legenda);
}

void Sistema::mudaModo()
{
    porEtapas = !porEtapas;
}

bool Sistema::getPorEtapas()
{
    return porEtapas;
}

void Sistema::simulaEtapas(int tempoDecorrido) //Função para modo Por Etapas
{
    int tam = processos.size();

    if (Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right)) //Esquerda retrocede
    {
        for (int i = 0; i < tam; i++)
        {
            processos[i]->executarPassos(tempoDecorrido, -1);
        }
        for (int i = 0; i < tam; i++)
        {
            verificaEscalonamento(processos[i], tempoDecorrido);
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Right) && !Keyboard::isKeyPressed(Keyboard::Left)) //Direita avança
    {  
        for (int i = 0; i < tam; i++)
        {
            processos[i]->executarPassos(tempoDecorrido, 1);
        }
        for (int i = 0; i < tam; i++)
        {
            verificaEscalonamento(processos[i], tempoDecorrido);
        }
    }
}

std::vector<Processo*> Sistema::getProcessos()
{
    return processos;
}
