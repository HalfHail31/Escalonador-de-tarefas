#include "Processo.h"
#include <iostream>

#define TAM 15

Processo::Processo(std::string i, unsigned long int c, int ing, int d, int p, int y): id(i), cor((c << 8) | 0xFF), ingresso(ing), duracao(d), prioridade(p),
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
	if (tempoDecorrido < ingresso || terminada)
		return;

	if (!iniciada)
		iniciada = true;

	if (ativa)
		tempoComCPU++;
	else
		resetaQuantum();

	if (tempoComCPU == quantum)
		quantumMax = true;

	if (ativa && !terminada)
		tempoExecutado++;

	prosseguir(tempoDecorrido);

	if (tempoExecutado >= duracao && !terminada)
		termina(tempoDecorrido);
}

void Processo::executarPassos(int tempoDecorrido, int dir)
{
	if (tempoDecorrido < ingresso)
		return;
	if (terminada && dir == 1)
		return;

	if (!iniciada)
		iniciada = true;

	if (dir == -1)
		retroceder(tempoDecorrido);
		
	else if (dir == 1)
	{
		salvarEstado();
		prosseguir(tempoDecorrido);

		if (ativa)
			tempoExecutado++;

		if (tempoExecutado >= duracao && !terminada)
			termina(tempoDecorrido);
	}
}

void Processo::setCor(RectangleShape* rect)
{
	if (terminada)
	{
		rect->setFillColor(sf::Color::Blue);
		legenda.setFillColor(sf::Color::Blue);
	}

	if (suspensa)
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
	e.posX = posX;

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
	posX = e.posX;
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

	if (terminada)
		legenda.setFillColor(sf::Color::Blue);
	else if (suspensa)
		legenda.setFillColor(sf::Color::Black);
	else if (ativa)
		legenda.setFillColor(cor);
	else
		legenda.setFillColor(sf::Color::White);

	if (tamanho.empty())
		posX = ingresso * TAM;
}

void Processo::mudaEstado()
{
	ativa = !ativa;
}

void Processo::termina(int tempoDecorrido)
{
	terminada = true;
	termino = tempoDecorrido;
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

const int Processo::getTempoRestante()
{
	return duracao - tempoExecutado;
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

	std::cout << "CPU indice " << indiceCPU << " cedida ao processo " << id << std::endl;
}

void Processo::removeCPU()
{
	std::cout << "CPU indice " << indiceCPU << " retirada do processo " << id << std::endl;

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
		prioridade = p;
}
