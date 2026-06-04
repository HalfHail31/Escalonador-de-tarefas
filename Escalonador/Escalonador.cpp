#include "Sistema.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

Sistema* criaSimulador()
{
    std::string sistema; //Linha dedicada à leitura dos dados do sistema
    std::string processo; //Linha/s dedicada/s à leitura dos dados do/s processo/s
    std::ifstream arquivo("Processo.txt"); //Nome padrão da entrada
    int y = 0; //Contador de linhas para instanciar processos em valores y distintos da tela

    if (arquivo.is_open())
    {
        std::getline(arquivo, sistema); //Armazena o conteúdo da primeira linha em sistema
        std::stringstream ss(sistema); //stringstream, serve para ler strings de arquivos e converter para diferentes tipos

        int quantum, cpus; //Variáveis para instanciar Sistema
        char sep; //Variável para servir de filtro e ignorar o caractere que não é número, no caso, o ";"
        std::string algoritmo; // Algoritmo a ser usado pelo sistema

        std::getline(ss, algoritmo, ';'); //Anota a string para escolher o algoritmo

        // Converte para minúsculo
        std::transform(
            algoritmo.begin(),
            algoritmo.end(),
            algoritmo.begin(),
            ::tolower
        );

        ss >> quantum >> sep; //Anota números antes de ; em quantum
        ss >> cpus >> sep; //Anota números antes de ; em cpus
        Sistema* sist = new Sistema(algoritmo, quantum, cpus); //Instancia o sistema com os valores recolhidos

        while (std::getline(arquivo, processo)) //Enquanto ainda houverem linhas para serem lidas, faça
        {
            std::stringstream ss(processo);
            std::string id;
            int ingresso, duracao, prioridade; //Variáveis para instanciar Processos
            unsigned long int cor; //Idem
            char sep; //Variável para servir de filtro e ignorar o caractere que não é número, no caso, o ";"

            std::getline(ss, id, ';');

            //ss >> id >> sep;
            ss >> std::hex >> cor >> std::dec >> sep; //Aceita valor em hexadecimal, letras passam a serem vistas como números para sep não parar em uma letra
            ss >> ingresso >> sep;
            ss >> duracao >> sep;
            ss >> prioridade >> sep;

            Processo* p = new Processo(id, cor, ingresso, duracao, prioridade, y); //Instancia um processo para esta iteração, y define sua altura y na tela
            y++; //Incrementa para o próximo processo
            if (p)
                sist->recebeProcesso(p); //Registra os processos no Sistema
            //std::cout << id << " " << cor << " " << ingresso << " " << duracao << " " << prioridade << std::endl;
        }
        arquivo.close();
        return sist;
    }
    else
    {
        std::cout << "Erro ao abrir o arquivo" << std::endl;
        return NULL;
    }

}



int main()
{
    Sistema* sistema = criaSimulador(); //Cria uma instância de Sistema para a simulação
    sf::RenderWindow window(sf::VideoMode(LARGURA_TELA, ALTURA_TELA), "Escalonador"); //Instância de janela SFML

    sf::Clock clock; //Variável de registro de tempo da SFML
    float timer = 0; //Contador de tempo em float para chamada executar do sistema (a cada 1s)
    int tempoSimulador = 0; //Contador de tempo inteiro para evitar erros de cálculo, atua como um contador de iterações
    float tempoQuantum = 0; //Contador de tempo inteiro para receber dados para cálculo de quantum
    float timerTecla = 0;
    float dt = 0; //Contador de tempo padrão, recebe o tempo armazenado na variável Clock (é o delta Tempo)
    bool fast = false;
    sf::Color background(127, 125, 255);

    sistema->setJanela(&window); //Envia ao Sistema uma referência para a janela onde ele deve desenhar os Processos

    while (window.isOpen())
    {
        dt = clock.restart().asSeconds(); //Recebe e armazena tempo decorrido em segundos
        timer += dt; //Adiciona para o timer a cada iteração
        timerTecla += dt;
        float tick = 1;

        if (fast)
            tick = 0.1;

        if (sistema->getPorEtapas())
            tick = 0.5;

        sf::Event event;

        while (window.pollEvent(event)) //Formato de evento padrão da SFML, permite que a janela seja fechada
        {
            if (event.type == sf::Event::Closed)
            {
                sf::Texture texture;
                texture.create(window.getSize().x, window.getSize().y); //Cria uma textura com o tamanho da tela
                texture.update(window);

                sf::Image screenshot = texture.copyToImage();//Tira uma captura da tela

                if (screenshot.saveToFile("GraficoGrant.png")) //Salva como GraficoGrant.png
                    std::cout << "Screenshot salva como GraficoGrant.png" << std::endl;
                else
                    std::cout << "Erro ao salvar screenshot" << std::endl;

                window.close();
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Space) && timerTecla > 0.25) //Altera o modo do Sistema de automático para passo a passo e vice-versa
        {
            std::cout << "AOBA";
            sistema->mudaModo();
            timerTecla = 0;
        }

        if (Keyboard::isKeyPressed(Keyboard::Down) && timerTecla > 0.25) //Altera o modo do Sistema de automático para passo a passo e vice-versa
        {
            fast = true;
            timerTecla = 0;
        }

        window.clear(background); //Limpa a janela
        if (timer >= tick)
        {
            if (!sistema->getPorEtapas()) //A cada segundo decorrido, faça (modo automático)
            {
                tempoQuantum = 1; //Envia o tempo a cada 1s para Sistema
                
                sistema->executar(tempoSimulador, tempoQuantum);
                std::cout << "Tempo global: " << tempoSimulador << std::endl;
                tempoSimulador++; //tempoSimulador registra o tempo global do Sistema, portanto não é resetado
                timer = 0;
                tempoQuantum = 0;
            }

            else if (sistema->getPorEtapas())
            {
                if (Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::Right)))//A cada 0.2 segundo decorrido, faça (modo passo a passo)
                {
                    tempoQuantum = 1; //Arredonda o tempo para 1 segundo
                    std::cout << "Tempo global: " << tempoSimulador << std::endl;

                    if (Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right))
                    {
                        if (tempoSimulador > 0)
                            tempoSimulador--;
                        //tempoQuantum--;

                    }
                    //if (tempoSimulador < 0)
                        //tempoSimulador = 0;
                    if (Keyboard::isKeyPressed(Keyboard::Right) && !Keyboard::isKeyPressed(Keyboard::Left))
                    {
                        tempoSimulador++; //Atualiza o tempo global do Simulador

                    }
                    
                    tempoQuantum = 1;
                        sistema->executar(tempoSimulador, tempoQuantum);

                        timer = 0;
                }
            }

        }

        sistema->desenhaProcessos(); //Desenha na janela
        sistema->desenhaProcessadores();
        window.display(); //Exibe o conteúdo desenhado na janela
    }
    return 0;
}