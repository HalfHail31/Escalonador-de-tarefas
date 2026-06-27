#include "Controlador.h"
#include <sstream>

Controlador::Controlador() : sist(NULL), y(0), tempoDecorrido(0)
{
}

Controlador::~Controlador()
{
	int size2 = (int)processos.size();

	for (int i = 0; i < size2; i++)
		delete processos[i];
}

void Controlador::setSistema(Sistema* s)
{
	sist = s;
	setProcessos();
}

void Controlador::setProcessos()
{
	processos = sist->getProcessos();
}

void Controlador::criaProcesso(std::string id, unsigned long int c, int ing, int d, int p)
{
	if (tempoDecorrido > ing)
	{
		std::cout << "Nao da para inserir um processo em um tempo ja decorrido" << std::endl;
		return;
	}
	//std::cout << "ID: " << id << " COR: " << c << " INGRESSO " << ing << " DURACAO " << d << " PRIORIDADE " << p << "\n";
	Processo* processo = new Processo(id, c, ing, d, p, y); //Instancia um processo para esta iteração, y define sua altura y na tela

	y++; //Incrementa para o próximo processo
	if (processo)
	{
		sist->recebeProcesso(processo); //Registra os processos no Sistema
		processos.push_back(processo); //Inclui no vector local também
	}
	else
		std::cout << "Erro ao criar processo de id: " << id << std::endl;
}

void Controlador::criaProcesso(std::string id, unsigned long int c, int ing, int d, int p, const std::vector<AcaoMutex>& acoes)
{
	if (tempoDecorrido > ing)
	{
		std::cout << "Nao da para inserir um processo em um tempo ja decorrido" << std::endl;
		return;
	}
	Processo* processo = new Processo(id, c, ing, d, p, y);

	for (const AcaoMutex& acao : acoes)
	{
		processo->adicionaAcao(
			acao.tipo,
			acao.mutexId,
			acao.instante
		);
	}

	y++;
	if (processo)
	{
		sist->recebeProcesso(processo);
		processos.push_back(processo);
		processo->exibeAcoes();

	}
	else
	{
		std::cout << "Erro ao criar processo de id: " << id << std::endl;
	}
}

//void Controlador::removeProcesso() {}

void Controlador::editaProcesso(std::string id, unsigned long int c, int ing, int d, int p)
{
	int tam = processos.size();

	for (int i = 0; i < tam; i++)
	{
		if (processos[i]->getId() == id) //Precisamos encontrar o processo a editar com base no id
		{
			processos[i]->setCor(c);

			if (tempoDecorrido < ing && !processos[i]->getIniciada()) //Não podemos inserir processos "no passado" e nem editar o ingresso de um processo que já ingressou
				processos[i]->setIngresso(ing);

			processos[i]->setDuracao(d);
			processos[i]->setPrioridade(p);
			processos[i]->cancelaTermino();
			break;
		}
	}
}

void Controlador::editaSistema(std::string a, int q, int p)
{
	//std::cout << a << " " << q << " " << p << "\n";
	sist->setAlgoritmo(a);
	if (q > 0)
		sist->setQuantum(q);
	if (p > 0)
		sist->setProcessadores(p);
}

void Controlador::exibeInformacoes()
{
	int tam = processos.size();

	std::cout << std::endl << "-----------------------------------------------" << std::endl;

	std::cout << "Informacoes do Sistema:" << std::endl << std::endl;

	std::string alg = sist->getAlgoritmo();
	int quantum = sist->getQuantum();
	int cpus = sist->getNumCPUs();
	int cpusLivres = sist->getNumCPUsLivres();

	std::cout << "Escalonador: " << alg << std::endl;
	std::cout << "Quantum: " << quantum << "s " << std::endl;
	std::cout << "Quantidade de CPUs: " << cpus << std::endl;
	std::cout << "Quantidade de CPUs livres: " << cpusLivres;

	std::cout << std::endl << "-----------------------------------------------" << std::endl;

	std::cout << "Processos:" << std::endl << std::endl;

	for (int i = 0; i < tam; i++)
	{
		std::string id = processos[i]->getId();
		int ingresso = processos[i]->getIngresso();
		int duracao = processos[i]->getDuracao();
		int termino = processos[i]->getTermino();
		int prioridade = processos[i]->getPrioridadeD();
		int estadoQuantum = processos[i]->getTempoComCPU();
		bool ativa = processos[i]->getAtiva();
		bool suspensa = processos[i]->getSuspensa();
		int indiceCPU = processos[i]->getIndiceCPU();
		bool terminada = processos[i]->getTerminada();
		int tempoRestante = processos[i]->getTempoRestante();

		std::cout << "ID: " << id << std::endl;
		std::cout << "Ingresso: " << ingresso << std::endl;
		std::cout << "Duracao: " << duracao << std::endl;
		std::cout << "Prioridade atual: " << prioridade << std::endl;
		std::cout << "Prioridade base: " << processos[i]->getPrioridade() << std::endl;

		if (!terminada) //Dado não relevante em processo terminado
			std::cout << "Tempo decorrido do Quantum: " << estadoQuantum << std::endl;

		if (ativa)
		{
			std::cout << "Status ativa: SIM" << std::endl;
			std::cout << "Indice da CPU ocupada: " << cpus << std::endl;
		}
		else
			std::cout << "Status ativa: NAO" << std::endl;

		if (suspensa)
			std::cout << "Status suspensa: SIM" << std::endl;
		else
			std::cout << "Status suspensa: NAO" << std::endl;

		if (terminada)
		{
			std::cout << "Status terminada: SIM" << std::endl;
			std::cout << "Terminou em: " << termino << std::endl;
		}
		else
		{
			std::cout << "Status terminada: NAO" << std::endl;
			std::cout << "Tempo restante para terminar: " << tempoRestante << std::endl;
		}

		std::cout << "-----------------------------------------------" << std::endl;
	}
}

void Controlador::executar(int tempoDecorrido)
{
	this->tempoDecorrido = tempoDecorrido; //Diz ao controlador o tempo atual do sistema

	int opcao = -1; ///////Variável genérica para switch case////////////

	std::string id; ////////////////////////////////////////////////////
	unsigned long int c;/////////CONTROLE DE PROCESSOS//////////////////
	int ing, d, p; /////////////////////////////////////////////////////
	std::string mutex;
	std::vector<AcaoMutex> acoes;
	std::string token;

	std::string alg;////////////////////////////////////////////////////
	int q; //////////////////////CONTROLE DO SISTEMA////////////////////
	int cpus; //////////////////////////////////////////////////////////

	char sep; ////////////Separador para pular a vírgula////////////////

	while (opcao != 0)
	{
		std::cout << std::endl;
		std::cout << "--------- Controle de tarefas ---------" << std::endl;
		std::cout << "Escolha sua opcao:" << std::endl;
		std::cout << "0 - Sair" << std::endl;
		std::cout << "1 - Criar um processo" << std::endl;
		std::cout << "2 - Editar um processo" << std::endl;
		std::cout << "3 - Editar o SO" << std::endl;
		std::cout << "4 - Exibir as informacoes do SO" << std::endl;
		std::cout << ">";

		std::cin >> opcao;

		if (opcao == 1)
		{
			std::stringstream ss;
			std::cout << "Insira os dados do processo a ser criado no formato: id cor ingresso duracao prioridade" << std::endl << ">";
			std::cin >> std::ws;
			std::cin >> std::dec >> id >> std::hex >> c >> std::dec >> ing >> d >> p;
			while (std::getline(ss, token, ';'))
			{
				AcaoMutex acao;

				if (token.substr(0, 2) == "ML")
					acao.tipo = LOCK;
				else if (token.substr(0, 2) == "MU")
					acao.tipo = UNLOCK;
				else
					continue;

				acao.mutexId = std::stoi(token.substr(2, 2));

				acao.instante = std::stoi(token.substr(5, 2));

				acoes.push_back(acao);
				sist->criaMutex(acao.mutexId);
			}
			std::cout << id << " " << c << " " << ing << " " << d << " " << p << "\n";

			criaProcesso(id, c, ing, d, p);
		}

		else if (opcao == 2)
		{
			std::cout << "Insira os dados do processo a ser editado no formato: id cor ingresso duracao prioridade" << std::endl << "Use id para identificar o processo a ser editado" << std::endl << ">";
			std::cin >> std::ws;
			std::cin >> std::dec >> id >> std::hex >> c >> std::dec >> ing >> d >> p;
			editaProcesso(id, c, ing, d, p);
		}

		else if (opcao == 3)
		{
			std::cout << "Insira os novos dados do SO no formato: algoritmo quantum processadores" << std::endl << ">";
			std::cin >> std::ws;
			std::cin >> std::dec >> alg >> q >> cpus;
			editaSistema(alg, q, cpus);
		}

		else if (opcao == 4)
			exibeInformacoes();

		if (std::cin.fail())
		{
			std::cin.clear(); // Limpa as flags de erro (como failbit)
			std::cin.ignore(10000, '\n');
		}
		//else
			//std::cout << "Opcao invalida!" << std::endl;
	}
}