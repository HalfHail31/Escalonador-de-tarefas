#include "Sistema.h"
#include "Processo.h"
#include <iostream>

Sistema::Sistema(std::string a, int q, int p) : algoritmo(a), quantum(q), CPUs(p), CPUsLivres(p), tempo(0), porEtapas(false), posTempo(0)
{
    std::cout << "-------- Escalonador de Tarefas --------" << std::endl;
    std::cout << "Sistema operacional de " << CPUs << " processadores e quantum de " << quantum << "s" << std::endl;
    std::cout << "Algoritmo de escalonamento selecionado: " << algoritmo << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Aperte ESPACO para ativar/desativar o modo passo a passo;" << std::endl;
    std::cout << "Aperte CIMA para ativar/desativar o modo fast_forward;" << std::endl;
    std::cout << "Aperte CONTROL para verificar o controle de tarefas;" << std::endl;

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

    for (int i = 0; i < size; i++)
        delete processadores[i];

    processadores.clear();

    for (EscalonadorMutex* mutex : mutexes)
        delete mutex;

    mutexes.clear();
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
    if (p->getAtiva() || p->getTerminada()) //Processos que j� possuem uma CPU ou j� terminaram n�o precisam de uma CPU de novo
        return;
    sf::Color cinza(0x898989FF);

    if (CPUsLivres > 0 && p->getAtiva() == false)
    {
        //Busca por processador livre ou inativo para ced�-lo
        for (int i = 0; i < CPUs; i++)
        {
            if (processadores[i]->getFillColor() == sf::Color::Green || processadores[i]->getFillColor() == cinza)
            {
                p->mudaEstado();
                p->recebeCPU(i);
                p->resetaEnvelhecimento();
                processadores[i]->setFillColor(sf::Color::Red);
                CPUsLivres--;
                break;
            }
        }
    }
}

void Sistema::retiraProcessador(Processo* p)
{
    //Libera a CPU associada ao processo se ele possuir um indice v�lido.
    int idx = p->getIndiceCPU();
    if (idx != -1 && idx >= 0 && idx < (int)processadores.size())
    {
        processadores[idx]->setFillColor(sf::Color::Green);
        //Garantir que o estado ativo fique desativado
        if (p->getAtiva())
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
    if (processos.empty()) //N�o continua se n�o existirem processos
        return;

    int tam = processos.size();

    for (int i = 0; i < tam; i++)
    {
        if (processos[i]->getTerminada()) //Retira CPUs cedidas a processos que j� terminaram
            retiraProcessador(processos[i]);
    }

    for (int i = 0; i < tam; i++)
    {
        if (processos[i]->getEstadoQuantum()) //Retira CPUs cedidas a processos que j� passaram por um quantum
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
                    prioritario = j;

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

        if (CPUsLivres > 0) //Se tiver CPUs livres, pode ceder uma para o priorit�rio
            cedeProcessador(processos[prioritario]);

        else if (CPUsLivres == 0) //S� pode retirar CPUs se nenhuma CPU estiver livre e existirem processos inativos
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
                //Se a menor prioridade for menor que o priorit�rio, seu processador ser� removido e entregue ao priorit�rio
                retiraProcessador(processos[menorPrioridade]);
                cedeProcessador(processos[prioritario]);
            }
        }
    }
}

void Sistema::executarSRTF(int tempoDecorrido)
{
    if (processos.empty())
        return;

    int tam = processos.size();

    for (int i = 0; i < tam; i++)
    {
        if (processos[i]->getTerminada()) //Retira CPUs cedidas a processos que j� terminaram
            retiraProcessador(processos[i]);
    }

    for (int i = 0; i < tam; i++)
    {
        if (processos[i]->getEstadoQuantum()) //Retira CPUs cedidas a processos que j� passaram por um quantum
            retiraProcessador(processos[i]);
    }

    //Para cada CPU
    for (int i = 0; i < CPUs; i++)
    {
        int menor = -1;

        //Busca o processo com menor tempo restante
        for (int j = 0; j < tam; j++)
        {
            if (tempoDecorrido >= processos[j]->getIngresso() && !processos[j]->getTerminada() && !processos[j]->getAtiva())
            {
                if (menor == -1 || processos[j]->getTempoRestante() < processos[menor]->getTempoRestante())
                    menor = j;

                if (menor != j && processos[menor]->getPrioridade() == processos[j]->getPrioridade())
                    if (desempate(processos[menor], processos[j]))
                        menor = j;
            }
        }

        //Nenhum processo dispon�vel
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

        else if (CPUsLivres == 0) //S� pode retirar CPUs se nenhuma CPU estiver livre e existirem processos inativos
        {
            int maiorTempoRestante = -1;

            for (int j = 0; j < tam; j++) //Percorre a lista de processos
            {
                if (processos[j]->getAtiva()) //Busca os processos ativos
                {
                    //Busca aquele com a maior tempo restante
                    if (maiorTempoRestante == -1 || processos[j]->getTempoRestante() > processos[maiorTempoRestante]->getTempoRestante())
                        maiorTempoRestante = j;
                }
            }

            if (maiorTempoRestante != -1 && processos[maiorTempoRestante]->getTempoRestante() > processos[menor]->getTempoRestante())
            {
                //Se a menor prioridade for menor que o priorit�rio, seu processador ser� removido e entregue ao priorit�rio
                retiraProcessador(processos[maiorTempoRestante]);
                cedeProcessador(processos[menor]);
            }
        }
    }
}

void Sistema::executar(int tempoDecorrido, int dir)
{
    if (processos.empty()) //Não continua se não existirem processos
        return;

    int tam = processos.size();

    for (Processo* p : processos)
    {
        if (!p->getTerminada() &&
            !p->getAtiva() &&
            tempoDecorrido >= p->getIngresso())
        {
            p->incrementaTempoOcioso();
        }
    }

    /*std::cout << "\nTempo: " << tempoDecorrido << '\n';

    for (Processo* p : processos)
    {
        if (tempoDecorrido >= p->getIngresso() &&
            !p->getTerminada())
        {
            std::cout
                << p->getId()
                << " -> prioridade: "
                << p->getPrioridade()
                << '\n';
        }
    }*/

    if (!porEtapas) {
        avancaTempo(tempoDecorrido);
        verificaEscalonamento(tempoDecorrido);
    }
    //Se por etapas, utiliza a pr�pria fun��o
    if (porEtapas)
        simulaEtapas(tempoDecorrido);
    else
    {
        for (int i = 0; i < tam; i++)
            if (!processos[i]->getTerminada()) { //N�o precisa mais executar uma tarefa que j� terminou
                processos[i]->executar(tempoDecorrido);
                processos[i]->verificaAcoesMutex(this);
            }

    }
}

void Sistema::avancaTempo(int tempoDecorrido)
{
    posTempo = tempoDecorrido * TAM;
    RectangleShape* rect = new RectangleShape;

    rect->setSize(Vector2f(TAM, 1));
    rect->setPosition(Vector2f(posTempo, ALTURA_TELA - 5));
    tempoSimulado.push_back(rect);
}

void Sistema::retornaTempo()
{
    int size = (int)tempoSimulado.size();
    if (size == 0)
        return;

    delete tempoSimulado.back();
    tempoSimulado.pop_back();
}

void Sistema::verificaEscalonamento(int tempoDecorrido) //Verifica se � necess�rio fazer um escalonamento
{
    int tam = processos.size();

    for (int i = 0; i < tam; i++) //Verifica se � necess�rio escalonar algum dos processos
    {
        //Se a tarefa acabou de ingressar ou terminar, ou j� executou durante o tempo de um quantum, devemos chamar o escalonamento
        if (processos[i]->getIngresso() == tempoDecorrido || processos[i]->getTermino() + 1 == tempoDecorrido || processos[i]->getEstadoQuantum())
        {
            if (algoritmo == "priop") //Escalonamento PRIOp
            {
                std::cout << "Escalonando com PRIOp" << std::endl;
                executarPRIOp(tempoDecorrido);
                break; //N�o tem porque continuar iterando se o escalonador j� foi chamado
            }
            else if (algoritmo == "srtf") //Escalonamento SRTF
            {
                std::cout << "Escalonando com SRTF" << std::endl;
                executarSRTF(tempoDecorrido);
                break; //N�o tem porque continuar iterando se o escalonador j� foi chamado
            }
        }
    }
}

bool Sistema::desempate(Processo* atual, Processo* candidato)
{
    // Se n�o houver atual, candidato vence
    if (atual == nullptr)
        return true;

    // 1) Preferir processo que n�o completou o quantum
    if (atual->getEstadoQuantum() && !candidato->getEstadoQuantum())
        return true; // candidato preferido
    if (!atual->getEstadoQuantum() && candidato->getEstadoQuantum())
        return false; // atual preferido

    // 2) Preferir processo j� ativo (manter quem est� executando)
    if (atual->getAtiva() && !candidato->getAtiva())
        return false; // manter atual
    if (!atual->getAtiva() && candidato->getAtiva())
        return true; // escolher candidato

    // 3) Menor ingresso (chegou antes)
    if (candidato->getIngresso() < atual->getIngresso())
        return true;
    if (candidato->getIngresso() > atual->getIngresso())
        return false;

    // 4) Menor dura��o
    if (candidato->getDuracao() < atual->getDuracao())
        return true;
    if (candidato->getDuracao() > atual->getDuracao())
        return false;

    // 5) Sorteio
    // 5) Desempate determin�stico: escolher por id (lexicogr�fico)
    return candidato->getId() < atual->getId();
}

void Sistema::setAlgoritmo(std::string alg)
{
    // Converte para min�sculo
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

    int tam = (int)processos.size();
    for (int i = 0; i < tam; i++)
        processos[i]->setQuantum(quantum); //Atualiza o valor de quantum dos processos
}

void Sistema::setProcessadores(int c)
{
    if (c > 0)
    {
        CPUs = c;
        CPUsLivres = CPUs;

        int tam = (int)processos.size();
        for (int i = 0; i < tam; i++)
        {
            if (processos[i]->getAtiva()) //Armazena as tarefas ativas para reativ�-las ap�s mudar o n�mero de processadores
                retiraProcessador(processos[i]);
        }

        int size = (int)processadores.size();
        for (int i = 0; i < size; i++)
            delete processadores[i];
        processadores.clear(); //Remove os ponteiros vazios


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

EscalonadorMutex* Sistema::criaMutex(int id)
{
    EscalonadorMutex* mutex = getMutex(id);

    if (mutex != nullptr)
        return mutex;

    mutex = new EscalonadorMutex(id);

    mutexes.push_back(mutex);

    return mutex;
}

void Sistema::reconstruirMutexes()
{
    for (auto mutex : mutexes)
        mutex->reset();

    for (auto processo : processos)
    {
        int tempoExecutado =
            processo->getTempoExecutado();

        const std::vector<AcaoMutex>& acoes =
            processo->getAcoes();

        for (const AcaoMutex& acao : acoes)
        {
            if (acao.instante > tempoExecutado)
                continue;

            EscalonadorMutex* mutex =
                getMutex(acao.mutexId);

            if (!mutex)
                continue;

            if (acao.tipo == LOCK)
            {
                mutex->solicitar(processo);
            }
            else
            {
                mutex->liberar(processo);
            }
        }
    }

}

const std::string Sistema::getAlgoritmo()
{
    return algoritmo;
}

const int Sistema::getNumCPUs()
{
    return CPUs;
}

const int Sistema::getNumCPUsLivres()
{
    return CPUsLivres;
}

const int Sistema::getQuantum()
{
    return quantum;
}

EscalonadorMutex* Sistema::getMutex(int id)
{
    for (EscalonadorMutex* mutex : mutexes)
    {
        if (mutex->getId() == id)
            return mutex;
    }

    return nullptr;
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

void Sistema::simulaEtapas(int tempoDecorrido) //Fun��o para modo Por Etapas
{
    int tam = processos.size();

    if (Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right)) //Esquerda retrocede
    {
        for (int i = 0; i < tam; i++)
        {
            processos[i]->executarPassos(tempoDecorrido, -1);
        }
        reconstruirMutexes();
        CPUsLivres = CPUs;
        for (int i = 0; i < CPUs; i++)
            processadores[i]->setFillColor(sf::Color::Green);

        // Marcar processadores ocupados conforme indiceCPU de cada processo
        for (int i = 0; i < tam; i++)
        {
            int idx = processos[i]->getIndiceCPU();
            if (idx != -1 && idx >= 0 && idx < (int)processadores.size())
            {
                processadores[idx]->setFillColor(sf::Color::Red);
                CPUsLivres--;
            }
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Right) && !Keyboard::isKeyPressed(Keyboard::Left)) //Direita avan�a
    {
        reconstruirMutexes();
        verificaEscalonamento(tempoDecorrido);

        for (int i = 0; i < tam; i++)
        {
            processos[i]->executarPassos(tempoDecorrido, 1);
            processos[i]->verificaAcoesMutex(this);
        }
        // Ao avan�ar no modo passo-a-passo tamb�m devemos executar o escalonador
        // para ajustar atribui��es de CPU (cede/retira) conforme o estado restaurado.
        //verificaEscalonamento(tempoDecorrido);

    }
}

std::vector<Processo*> Sistema::getProcessos()
{
    return processos;
}