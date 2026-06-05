#include "Sistema.h"
#include <iostream>

Sistema::Sistema(std::string a, int q, int p) : algoritmo(a), quantum(q), CPUs(p), CPUsLivres(p), tempo(0), porEtapas(false)
{
    std::cout << algoritmo << std::endl;
    std::cout << "Ola, sou um sistema operacional de " << CPUs << " processadores e quantum de " << quantum << "s\n";

    for (int i = CPUs; i >= 0; i--) //Cria processadores
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
        for (int i = 0; i < CPUs; i++) {
            if (processadores[i]->getFillColor() == sf::Color::Green || processadores[i]->getFillColor() == cinza) {
                processadores[i]->setFillColor(sf::Color::Red);
                break;
            }
        }
        p->mudaEstado();
        CPUsLivres--;
    }
}

void Sistema::retiraProcessador(Processo* p)
{
    if (p->getAtiva() == true)
    {
        //Vermelho = ocupado
        for (int i = CPUs; i >= 0; i--) {
            if (processadores[i]->getFillColor() == sf::Color::Red) {
                processadores[i]->setFillColor(sf::Color::Green);
                break;
            }
        }
        p->mudaEstado();
        CPUsLivres++;
    }
}

void Sistema::desativaProcessadores()
{
    sf::Color cinza(0x898989FF);

    for (int i = 0; i < CPUs; i++) {
        if (processadores[i]->getFillColor() == sf::Color::Green) {
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

void Sistema::executarPRIOp(int tempoDecorrido, float dt)
{
    if (processos.empty()) //Não continua se não existirem processos
        return;

    int tam = processos.size();

    for (int i = 0; i < tam; i++) //Retira CPUs cedidas a processos que já terminaram
    {
        if (processos[i]->getTerminada() && processos[i]->getAtiva())
            retiraProcessador(processos[i]);
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

void Sistema::executarSRTF(int tempoDecorrido, float dt)
{
    tempo += dt;

    if (processos.empty())
        return;

    int tam = processos.size();

    // Remove CPUs de processos terminados
    for (int i = 0; i < tam; i++)
    {
        if (processos[i]->getTerminada() && processos[i]->getAtiva())
            retiraProcessador(processos[i]);
    }

    // Para cada CPU
    for (int cpu = 0; cpu < CPUs; cpu++)
    {
        int menor = -1;

        // Busca o processo com menor tempo restante
        for (int i = 0; i < tam; i++)
        {
            if (tempoDecorrido >= processos[i]->getIngresso() &&
                !processos[i]->getTerminada() && !processos[i]->getAtiva())
            {
                if (menor == -1 || desempate(processos[menor], processos[i]))
                    menor = i;
            }
        }

        // Nenhum processo disponível
        if (menor == -1)
        {
            desativaProcessadores();
            break;
        }

        // CPU livre
        if (CPUsLivres > 0)
        {
            if (!processos[menor]->getAtiva())
                cedeProcessador(processos[menor]);
        }

        /*else

        {

            // Busca o processo ATIVO com maior tempo restante

            int maiorAtivo = -1;



            for (int i = 0; i < tam; i++)

            {

                if (processos[i]->getAtiva())

                {

                    if (maiorAtivo == -1 ||

                        processos[i]->getTempoRestante() >

                        processos[maiorAtivo]->getTempoRestante())

                    {

                        maiorAtivo = i;

                    }

                }

            }



            // Preempção

            if (maiorAtivo != -1 &&

                processos[menor]->getTempoRestante() <

                processos[maiorAtivo]->getTempoRestante())

            {

                retiraProcessador(processos[maiorAtivo]);



                if (!processos[menor]->getAtiva())

                {

                    cedeProcessador(processos[menor]);

                }

            }

        }*/
    }
}

void Sistema::executar(int tempoDecorrido, float dt)
{
    if (processos.empty()) //Não continua se não existirem processos
        return;

    //tempo += dt;

    int tam = processos.size();
    int inativos = 0;

    //Executa PRIOp
    if (algoritmo == "priop")
    {
        for (int i = 0; i < tam; i++) //É necessário escalonar?
        {
            //Se alguma tarefa acabou de ingressar ou terminar, ou já executou durante o tempo de um quantum, devemos chamar o escalonamento
            if (processos[i]->getIngresso() == tempoDecorrido || (processos[i]->getTerminada()) || processos[i]->getEstadoQuantum())
            {
                std::cout << "Escalonando com PRIOp" << std::endl;
                executarPRIOp(tempoDecorrido, dt);
                break;
            }
        }
    }

    //Executa SRTF
    else if (algoritmo == "srtf")
    {
        std::cout << "srtf" << std::endl;
        executarSRTF(tempoDecorrido, dt);
    }

    //Se por etapas, utiliza a própria função
    if (porEtapas)
    {
        for (int i = 0; i < tam; i++)
            processos[i]->executarPassos(tempoDecorrido);
    }

    else
    {
        for (int i = 0; i < tam; i++)
            if (!processos[i]->getTerminada()) //Não precisa mais executar uma tarefa que já terminou
                processos[i]->executar(tempoDecorrido);
    }

}

bool Sistema::desempate(Processo* atual, Processo* candidato)
{
    if (atual == nullptr)
        return true;

    // Desempate (1)
    // Processo já executando
    std::cout << "Empate" << std::endl;
    std::cout << "Mantendo aquele que não passou um quantum." << std::endl;
    if (candidato->getEstadoQuantum() && !atual->getEstadoQuantum())
        return true;

    if (!candidato->getEstadoQuantum() && atual->getEstadoQuantum())
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
    algoritmo = alg;
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
    for (int i = 0; i < CPUs; i++) {
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

//Função para modo Por Etapas
void Sistema::simulaEtapas(int tempoDecorrido)
{
    int tam = processos.size();

    if (Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right))
    {
        for (int i = 0; i < tam; i++)
            processos[i]->retroceder();
    }

    if (Keyboard::isKeyPressed(Keyboard::Right) && !Keyboard::isKeyPressed(Keyboard::Left))
    {
        for (int i = 0; i < tam; i++)
            processos[i]->executarPassos(tempoDecorrido);
    }
}