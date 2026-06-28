#pragma once

#include "Sistema.h"
#include "Controlador.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

Sistema* criaSimulador(Controlador* controle)
{
    std::string sistema; //Linha dedicada � leitura dos dados do sistema
    std::string processo; //Linhas dedicadas � leitura dos dados dos processos

    std::string arq;// = "Processo.txt";
    std::cout << "Insira o nome do arquivo a ser lido" << std::endl << "Exemplo: arquivo.txt" << std::endl << ">";
    std::cin >> arq;

    std::ifstream arquivo(arq); //Nome padr�o da entrada
    int y = 0; //Contador de linhas para instanciar processos em valores y distintos da tela

    if (arquivo.is_open())
    {
        std::getline(arquivo, sistema); //Armazena o conte�do da primeira linha em sistema
        std::stringstream ss(sistema); //stringstream, serve para ler strings de arquivos e converter para diferentes tipos

        int quantum, cpus; //Vari�veis para instanciar Sistema
        char sep; //Vari�vel para servir de filtro e ignorar o caractere que n�o � n�mero, no caso, o ";"
        std::string algoritmo; // Algoritmo a ser usado pelo sistema

        std::getline(ss, algoritmo, ';'); //Anota a string para escolher o algoritmo

        // Converte para min�sculo
        std::transform(
            algoritmo.begin(),
            algoritmo.end(),
            algoritmo.begin(),
            ::tolower
        );

        ss >> quantum >> sep; //Anota n�meros antes de ; em quantum
        ss >> cpus >> sep; //Anota n�meros antes de ; em cpus
        Sistema* sist = new Sistema(algoritmo, quantum, cpus); //Instancia o sistema com os valores recolhidos
        controle->setSistema(sist); //Para controlar e consultar os dados do sistema

        while (std::getline(arquivo, processo)) //Enquanto ainda houverem linhas para serem lidas
        {
            std::stringstream ss(processo);
            std::string id;
            int ingresso, duracao, prioridade; //Vari�veis para instanciar Processos
            unsigned long int cor; //Idem
            char sep; //Vari�vel para servir de filtro e ignorar o caractere que n�o � n�mero, no caso, o ";"

            //int iniOpES; //Para tarefas que realizam oerações de E/S
            //int termiOpES;
            //bool realizaOpES = false;

            std::getline(ss, id, ';');

            //ss >> id >> sep;
            ss >> std::hex >> cor >> std::dec >> sep; //Aceita valor em hexadecimal, letras passam a serem vistas como n�meros para sep n�o parar em uma letra
            ss >> ingresso >> sep;
            ss >> duracao >> sep;
            ss >> prioridade >> sep;

            std::vector<AcaoMutex> acoes;
            std::vector<OperacaoES> listaES;
            std::string token;

            while (std::getline(ss, token, ';'))
            {
                AcaoMutex acao;

                if (token.substr(0, 2) == "ML" || token.substr(0, 2) == "MU")
                {
                    if (token.substr(0, 2) == "ML")
                        acao.tipo = LOCK;
                    else if (token.substr(0, 2) == "MU")
                        acao.tipo = UNLOCK;

                    acao.mutexId = std::stoi(token.substr(2, 2));

                    acao.instante = std::stoi(token.substr(5, 2));

                    acoes.push_back(acao);
                    sist->criaMutex(acao.mutexId);
                }
                else if (token.substr(0, 2) == "IO")
                {
                    std::stringstream ssIO(token);
                    char separador;
                    int inicio, duracao;
                    //Para ler no formato IO:xx-yy
                    ssIO >> separador >> separador;
                    ssIO >> separador;
                    ssIO >> inicio;
                    ssIO >> separador;
                    ssIO >> duracao;

                    OperacaoES op;
                    op.instanteInicio = inicio;
                    op.duracao = duracao;
                    listaES.push_back(op);
                }
                else
                    continue;
            }

            /*for (const auto& a : acoes)
            {
                std::cout
                    << id
                    << " -> "
                    << (a.tipo == LOCK ? "LOCK" : "UNLOCK")
                    << " M"
                    << a.mutexId
                    << " @ "
                    << a.instante
                    << std::endl;
            }*/
            controle->criaProcesso(id, cor, ingresso, duracao, prioridade, acoes, listaES); //Cria processos que ser�o inseridos no sistema
        }
        arquivo.close();
        return sist;
    }
    else
    {
        std::cout << "Erro ao abrir o arquivo" << std::endl;
        return criaSimulador(controle); //Chamada recursiva em caso de erro na leitura
    }
}
