#include "Processo.h"
#include "Sistema.h"
#include <iostream>


Processo::Processo(std::string i, unsigned long int c, int ing, int d, int p, int y) : id(i), cor((c << 8) | 0xFF), ingresso(ing), duracao(d), prioridade(p), usaMutex(false), prioridadeD(p),
ativa(false), terminada(false), suspensa(false), terminoSuspensao(-1), recebeuMutex(false), iniciada(false), tempoExecutado(0), tempoComCPU(0), termino(-1), indiceCPU(-1), quantumMax(false)
{
	posX = ingresso * TAM;
	posY = y * TAM * 1.5;


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
}
/*
Processo::Processo(std::string i, unsigned long int c, int ing, int d, int p, int y, int iniOpES, int termiOpES) : id(i), cor((c << 8) | 0xFF), ingresso(ing), duracao(d), prioridade(p), usaMutex(false), prioridadeD(p),
ativa(false), /*inicioOperacaoES(iniOpES), duracaoOperacaoES(termiOpES), tempoOperacaoES(-1), terminada(false), suspensa(false), recebeuMutex(false), iniciada(false), tempoExecutado(0), tempoComCPU(0), termino(-1), indiceCPU(-1), quantumMax(false)
{
	posX = ingresso * TAM;
	posY = y * TAM * 1.5;


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
}*/

Processo::~Processo()
{
	int size = (int)tamanho.size();

	for (int i = 0; i < size; i++)
	{
		delete tamanho[i];
	}
}

void Processo::desenha(RenderWindow* window) //Desenha o processo
{
	int size = (int)tamanho.size();

	for (int i = 0; i < size; i++)
	{
		window->draw(*(tamanho[i]));

		if (tamanho[i]->getFillColor() == sf::Color::Black) //Destaca uma tarefa suspensa com uma grade #
		{
			sf::Vector2f pos = tamanho[i]->getPosition();
			sf::Vector2f sizeRect = tamanho[i]->getSize();

			sf::Color corGrade(sf::Color::Red);

			if (usaMutex && tempoExecutado >= getAcoes().back().instante) //Cerquilha azul se a suspensão for por mutex
				corGrade = sf::Color::Blue;

			float tercoX = sizeRect.x / 3.0f;
			float tercoY = sizeRect.y / 3.0f;

			sf::Vertex hLine1[] = {
				sf::Vertex(sf::Vector2f(pos.x, pos.y + tercoY), corGrade),
				sf::Vertex(sf::Vector2f(pos.x + sizeRect.x, pos.y + tercoY), corGrade)
			};
			sf::Vertex hLine2[] = {
				sf::Vertex(sf::Vector2f(pos.x, pos.y + 2 * tercoY), corGrade),
				sf::Vertex(sf::Vector2f(pos.x + sizeRect.x, pos.y + 2 * tercoY), corGrade)
			};

			sf::Vertex vLine1[] = {
				sf::Vertex(sf::Vector2f(pos.x + tercoX, pos.y), corGrade),
				sf::Vertex(sf::Vector2f(pos.x + tercoX, pos.y + sizeRect.y), corGrade)
			};
			sf::Vertex vLine2[] = {
				sf::Vertex(sf::Vector2f(pos.x + 2 * tercoX, pos.y), corGrade),
				sf::Vertex(sf::Vector2f(pos.x + 2 * tercoX, pos.y + sizeRect.y), corGrade)
			};
			window->draw(hLine1, 2, sf::Lines);
			window->draw(hLine2, 2, sf::Lines);
			window->draw(vLine1, 2, sf::Lines);
			window->draw(vLine2, 2, sf::Lines);
		}
	}
	if (iniciada)
	{
		legenda.setPosition(posX + TAM, posY);
		window->draw(legenda);
	}
}

void Processo::executar(int tempoDecorrido)
{
	if (tempoDecorrido < ingresso || terminada)
		return;
	
	if (!iniciada)
		iniciada = true;

	salvarEstado();
	if (!suspensa && ativa) //Se não está suspensa, verifica se existem operações de E/S
	{
		if (!operacoesES.empty() && tempoExecutado == operacoesES.front().instanteInicio) //Se existir uma operação E/S pendente, deve ser realizada
		{
			mudaEstadoSuspensa();
			tempoOperacaoES = operacoesES.front().duracao;
			operacoesES.pop();
		}
	}
	else if (suspensa && tempoOperacaoES > 0) //Tarefa executando operação E/S fica inativa por um tempo
	{
		tempoOperacaoES--;
		if (tempoOperacaoES == 0)
		{
			mudaEstadoSuspensa();
			terminoSuspensao = tempoDecorrido;
		}
	}
	prosseguir(tempoDecorrido);

	if (ativa)
		tempoComCPU++;
	else
		resetaQuantum();

	if (tempoComCPU >= quantum)
		quantumMax = true;

	if (ativa && !suspensa && !terminada)
		tempoExecutado++;

	if (tempoExecutado >= duracao && !terminada)
		termina(tempoDecorrido);
}

void Processo::executarPassos(int tempoDecorrido, int dir)
{
	if (tempoDecorrido < ingresso) //Correção de alguns problemas com lixo de memória ao retroceder até 0
	{
		iniciada = false;
		ativa = false;
		terminada = false;
		suspensa = false;
		tempoExecutado = 0;
		tempoComCPU = 0;
		termino = -1;
		indiceCPU = -1;
		quantumMax = false;
		tempoOperacaoES = -1;
		while (!operacoesES.empty())
			operacoesES.pop();
		int sz = (int)tamanho.size();
		for (int i = 0; i < sz; i++)
			delete tamanho[i];
		tamanho.clear();
		historico.clear();

		posX = ingresso * TAM;
		legenda.setFillColor(sf::Color::White);
		return;
	}

	if (terminada && dir == 1)
		return;

	if (!iniciada)
		iniciada = true;

	if (dir == -1)
		retroceder(tempoDecorrido);
	else if (dir == 1)
	{
		salvarEstado();
		if (!suspensa && ativa) //Se não está suspensa, verifica se existem operações de E/S
		{
			if (!operacoesES.empty() && tempoExecutado == operacoesES.front().instanteInicio) //Se existir uma operação E/S pendente, deve ser realizada
			{
				mudaEstadoSuspensa();
				tempoOperacaoES = operacoesES.front().duracao;
				operacoesES.pop();
			}
		}
		else if (suspensa && tempoOperacaoES > 0) //Tarefa executando operação E/S fica inativa por um tempo
		{
			tempoOperacaoES--;
			if (tempoOperacaoES == 0)
			{
				mudaEstadoSuspensa();
				terminoSuspensao = tempoDecorrido;
			}
		}
		prosseguir(tempoDecorrido);

		if (ativa)
			tempoComCPU++;
		else
			resetaQuantum();

		if (tempoComCPU >= quantum)
			quantumMax = true;

		if (ativa && !suspensa && !terminada)
			tempoExecutado++;

		if (tempoExecutado >= duracao && !terminada)
			termina(tempoDecorrido);
	}
	if (tempoExecutado == 0)
		legenda.setFillColor(sf::Color::Transparent); //Oculta a legenda caso retroceda até a tarefa não ter iniciado
}

void Processo::setCor(RectangleShape* rect)
{
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
	else if (ativa)
	{
		rect->setFillColor(cor);
		legenda.setFillColor(cor);
	}
	else
	{
		rect->setFillColor(sf::Color::White);
		legenda.setFillColor(sf::Color::White);
	}
}

void Processo::prosseguir(int tempoDecorrido)
{
	posX = tempoDecorrido * TAM;
	RectangleShape* rect = new RectangleShape;

	rect->setSize(Vector2f(TAM, TAM));
	rect->setPosition(Vector2f(posX, posY));
	setCor(rect);
	tamanho.push_back(rect);
}

void Processo::salvarEstado()
{
	Estado e;
	e.ativa = ativa;
	e.terminada = terminada;
	e.suspensa = suspensa;
	e.iniciada = iniciada;
	e.tempoExecutado = tempoExecutado;
	e.tempoComCPU = tempoComCPU;
	e.termino = termino;
	e.indiceCPU = indiceCPU;
	e.quantumMax = quantumMax;
	e.posX = posX;
	e.prioridadeD = prioridadeD;
	e.tempoOcioso = tempoOcioso;
	e.tempoOpES = tempoOperacaoES;

	e.operacoesESRestantes.clear();
	std::queue<OperacaoES> copiaFila = operacoesES;
	while (!copiaFila.empty()) //Salva a fila de operações E/S
	{
		e.operacoesESRestantes.push_back(copiaFila.front());
		copiaFila.pop();
	}
	//e.estadoAcoes.clear();
	/*for (const AcaoMutex& a : acoes)
	{
		EstadoAcao ea;
		ea.executada = a.executada;

		e.estadoAcoes.push_back(ea);
	}*/

	historico.push_back(e);
}

void Processo::recuperaEstado()
{
	if (historico.empty())
		return;

	Estado e = historico.back();
	historico.pop_back();

	ativa = e.ativa;
	terminada = e.terminada;
	suspensa = e.suspensa;
	iniciada = e.iniciada;
	tempoExecutado = e.tempoExecutado;
	tempoComCPU = e.tempoComCPU;
	termino = e.termino;
	indiceCPU = e.indiceCPU;
	quantumMax = e.quantumMax;
	posX = e.posX;
	prioridadeD = e.prioridadeD;
	tempoOcioso = e.tempoOcioso;
	tempoOperacaoES = e.tempoOpES;

	while (!operacoesES.empty()) //Recupera a fila de operações de E/S
	{
		operacoesES.pop();
	}
	for (const auto& op : e.operacoesESRestantes)
	{
		operacoesES.push(op);
	}

	/*for (size_t i = 0;
		i < acoes.size() && i < e.estadoAcoes.size();
		i++)
	{
		acoes[i].executada =
			e.estadoAcoes[i].executada;
	}*/
}

void Processo::retroceder(int tempoDecorrido)
{
	if (terminada && tempoDecorrido > termino) //Uma tarefa terminada só recupera estado no momento em que terminou
		return;

	int size = (int)tamanho.size();
	if (size == 0)
		return;

	delete tamanho.back();
	tamanho.pop_back();

	recuperaEstado();

	//if (terminada)
		//legenda.setFillColor(sf::Color::Blue);
	if (suspensa)
		legenda.setFillColor(sf::Color::Black);
	else if (ativa)
		legenda.setFillColor(cor);
	else
		legenda.setFillColor(sf::Color::White);

	if (tamanho.empty())
		posX = ingresso * TAM;
}

void Processo::mudaEstadoAtiva()
{
	ativa = !ativa;
}

void Processo::mudaEstadoSuspensa()
{
	suspensa = !suspensa;
	if (ativa) mudaEstadoAtiva();
}

void Processo::termina(int tempoDecorrido)
{
	terminada = true;
	termino = tempoDecorrido;
	quantumMax = false;
}

void Processo::adicionaAcao(TipoAcao tipo, int mutexId, int instante)
{
	AcaoMutex a;
	a.tipo = tipo;
	a.mutexId = mutexId;
	a.instante = instante;

	acoes.push_back(a);
}

void Processo::exibeAcoes()
{
	std::cout << "Acoes de " << id << ":" << std::endl;

	for (const auto& a : acoes)
	{
		std::cout
			<< (a.tipo == LOCK ? "LOCK" : "UNLOCK")
			<< " M"
			<< a.mutexId
			<< " @ "
			<< a.instante
			<< std::endl;
	}
}

const int Processo::getSaiuSuspensao()
{
	return terminoSuspensao;
}

const bool Processo::getAtiva()
{
	return ativa;
}

const bool Processo::getIniciada()
{
	return iniciada;
}

const bool Processo::getTerminada()
{
	return terminada;
}

const int Processo::getPrioridade()
{
	return prioridade;
}

const int Processo::getPrioridadeD()
{
	return prioridadeD;
}

const int Processo::getTempoRestante()
{
	return duracao - tempoExecutado;
}

int Processo::getTempoExecutado()
{
	return tempoExecutado;
}

const int Processo::getIngresso()
{
	return ingresso;
}

const bool Processo::getSuspensa()
{
	return suspensa;
}

const int Processo::getTempoComCPU()
{
	return tempoComCPU;
}

const std::vector<AcaoMutex>& Processo::getAcoes()
{
	return acoes;
}

const int Processo::getDuracao()
{
	return duracao;
}

const int Processo::getTermino()
{
	return termino;
}

const int Processo::getEstadoQuantum()
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

	//std::cout << "CPU indice " << indiceCPU << " cedida ao processo " << id << std::endl;
}

void Processo::removeCPU()
{
	//std::cout << "CPU indice " << indiceCPU << " retirada do processo " << id << std::endl;

	indiceCPU = -1;
}

int Processo::getIndiceCPU()
{
	return indiceCPU;
}

void Processo::incrementaTempoOcioso()
{
	tempoOcioso++;
	if (tempoOcioso >= quantum)
	{
		int antiga = prioridadeD;

		prioridadeD++;
		tempoOcioso = 0;

		std::cout
			<< "[AGING] Processo "
			<< id
			<< " prioridade "
			<< antiga
			<< " -> "
			<< prioridadeD
			<< std::endl;
	}
}

void Processo::resetaEnvelhecimento()
{
	if (prioridadeD != prioridade)
	{
		std::cout
			<< "[AGING] Processo "
			<< id
			<< " prioridade restaurada "
			<< prioridadeD
			<< " -> "
			<< prioridade
			<< std::endl;
	}

	prioridadeD = prioridade;
	tempoOcioso = 0;
}

void Processo::verificaAcoesMutex(Sistema* sistema)
{
	for (AcaoMutex& acao : acoes)
	{
		if (acao.executada)
			continue;

		if (acao.instante != tempoExecutado)
			continue;

		EscalonadorMutex* mutex =
			sistema->getMutex(acao.mutexId);

		if (!mutex)
			continue;

		if (acao.tipo == LOCK)
		{
			std::cout
				<< "[T="
				<< tempoExecutado
				<< "] "
				<< id
				<< " solicita M"
				<< acao.mutexId
				<< std::endl;

			mutex->solicitar(this);
		}
		else
		{
			std::cout
				<< "[T="
				<< tempoExecutado
				<< "] "
				<< id
				<< " libera M"
				<< acao.mutexId
				<< std::endl;

			mutex->liberar(this);
		}
		acao.executada = true;
	}
}

void Processo::adicionaOperacaoES(int inicio, int duracao)
{
	OperacaoES opES;
	opES.instanteInicio = inicio;
	opES.duracao = duracao;
	operacoesES.push(opES);
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
	if (ing >= 0)
		ingresso = ing;
}

void Processo::setDuracao(int d)
{
	if (d > 0)
		duracao = d;
}

void Processo::setPrioridade(int p)
{
	if (p > 0) 
	{
		prioridade = p;
		prioridadeD = p;
	}
}

void Processo::cancelaTermino()
{
	terminada = false;
}