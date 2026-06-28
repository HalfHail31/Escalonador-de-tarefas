#include "Sistema.h"
#include "Controlador.h"
#include "LeitorDeArquivo.h"

int tempoSimulador = 0; //Contador de tempo inteiro para evitar erros de c�lculo, atua como um contador de itera��es

int main()
{
    Controlador control; //Controle de tasks
    Sistema* sistema = criaSimulador(&control); //Cria uma inst�ncia de Sistema para a simula��o

    sf::RenderWindow window(sf::VideoMode(LARGURA_TELA, ALTURA_TELA), "Escalonador", sf::Style::Titlebar | sf::Style::Close); //Inst�ncia de janela SFML
    window.setPosition(sf::Vector2i(0, 150));

    sf::Clock clock; //Vari�vel de registro de tempo da SFML
    float timer = 0; //Contador de tempo em float para chamada executar do sistema (a cada 1s)
    float tempoQuantum = 0; //Contador de tempo inteiro para receber dados para c�lculo de quantum
    float timerTecla = 0; //Delay entre o aperto de teclas
    float dt = 0; //Contador de tempo padr�o, recebe o tempo armazenado na vari�vel Clock (é o delta Tempo)
    bool fast = false; //Define se o sistema est� no modo fast-forward
    sf::Color background(127, 125, 255); //Cor do fundo

    sistema->setJanela(&window); //Envia ao Sistema uma refer�ncia para a janela onde ele deve desenhar os Processos

    while (window.isOpen())
    {
        dt = clock.restart().asSeconds(); //Recebe e armazena tempo decorrido em segundos
        timer += dt; //Adiciona para o timer a cada iteração
        timerTecla += dt;
        float tick = 1;

        if (fast) //Modo fast-foward faz o tick ser mais curto
            tick = 0.1;

        if (sistema->getPorEtapas())
            tick = 0.1;

        sf::Event event;

        while (window.pollEvent(event)) //Formato de evento padr�o da SFML, permite que a janela seja fechada
        {
            if (event.type == sf::Event::Closed)
            {
                sf::Texture texture;
                texture.create(window.getSize().x, window.getSize().y); //Cria uma textura com o tamanho da tela
                texture.update(window);

                sf::Image screenshot = texture.copyToImage(); //Tira uma captura da tela

                if (screenshot.saveToFile("GraficoGant.png")) //Salva como GraficoGant.png
                    std::cout << "Screenshot salva como GraficoGant.png" << std::endl;
                else
                    std::cout << "Erro ao salvar screenshot" << std::endl;

                window.close();
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Space) && timerTecla > 0.25) //Altera o modo do Sistema de autom�tico para passo a passo e vice-versa
        {
            sistema->mudaModo();
            timerTecla = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl) && timerTecla > 0.25) //Ativa o controle de tarefas
        {
            control.executar(tempoSimulador);
            timerTecla = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up) && timerTecla > 0.25) //Ativa ou desativa o modo fast-forward
        {
            if (fast)
                std::cout << "Modo por fast-forward desativado!" << std::endl;
            else
                std::cout << "Modo fast-forward ativado!" << std::endl;
            fast = !fast;
            timerTecla = 0;
        }

        window.clear(background); //Limpa a janela
        if (timer >= tick) //Se passou um tick, o sistema executa
        {
            if (!sistema->getPorEtapas()) //A cada segundo decorrido (modo autom�tico)
            {
                sistema->executar(tempoSimulador, 1);
                tempoSimulador++; //tempoSimulador registra o tempo global do Sistema, portanto n�o � resetado
                timer = 0;
            }

            else if (sistema->getPorEtapas())
            {
                if (Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::Right)))//A cada 0.2 segundo decorrido (modo passo a passo)
                {
                    if (Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right))
                    {
                        if (tempoSimulador > 0)
                            tempoSimulador--;
                        sistema->executar(tempoSimulador, -1);
                        timer = 0;
                        std::cout << "Tempo global: " << tempoSimulador << std::endl;
                    }

                    if (Keyboard::isKeyPressed(Keyboard::Right) && !Keyboard::isKeyPressed(Keyboard::Left))
                    {
                        sistema->executar(tempoSimulador, 1);
                        tempoSimulador++;
                        timer = 0;
                        std::cout << "Tempo global: " << tempoSimulador << std::endl;
                    }
                }
            }
        }

        //Desenha na janela
        sistema->desenhaProcessos();
        sistema->desenhaProcessadores();
        sistema->desenhaLinhaDoTempo();
        window.display(); //Exibe o conte�do desenhado na janela
    }
    return 0;
}