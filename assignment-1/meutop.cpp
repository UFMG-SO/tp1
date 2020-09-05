#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/stat.h>
#include <signal.h>
#include "dirent.h"

// Pode?
#include <vector>
#include <pwd.h>
#include <unistd.h>

using namespace std;

const int LINE_PID = 1;
const int LINE_NAME = 3;
const int LINE_STATE = 6;

const int PROCESSES_LIMIT = 999999;

void clear()
{
	cout << "\033[2J\033[1;1H";
}

// Retorna se foi possivel ler o processso
bool read_process(int PID, bool need_print)
{
	// Define o caminho do arquivo e tenta abri-lo
	string path = "/proc/" + to_string(PID) + "/status";
	ifstream file(path);

	// Caso não tenha aberto o arquivo nao deve prosseguir
	if (!file.is_open())
	{
		if (need_print)
		{
			cout << "Processo de PID " << PID << " nao encontrado" << endl;
		}
		return false;
	}

	string line;
	int line_counter = 0;
	string id = "", user = "", name = "", state = "";

	// Lê o arquivo linha a linha
	while (getline(file, line))
	{
		line_counter++;
		// Caso a linha em que estamos seja uma das pré-definidas
		if (line_counter == LINE_PID || line_counter == LINE_NAME || line_counter == LINE_STATE)
		{
			line.erase(line.find('\t'), 1);								   // Remove o tab (\t) da string
			string description = line.substr(0, line.find(':'));		   // Pega o que está antes do ':'
			string value = line.substr(line.find(':') + 1, line.length()); // Pega o que está depois do ':'
			if (description == "Name")
			{
				name = value;
			}
			else if (description == "State")
			{
				state = value.substr(0, value.find(' '));
			}
			else if (description == "Pid")
			{
				id = value;
			}
		}
	}

	// Pegando o dono do arquivo
	struct stat info;
	if (stat(path.c_str(), &info) == 0)
	{
		struct passwd *pw = getpwuid(info.st_uid);
		if (pw != 0)
		{
			user = pw->pw_name;
		}
	}

	// Fecha o arquivo e mostra as informações do processo
	file.close();
	if (need_print)
	{
		cout << setw(5) << id << setw(5) << '|' << setw(18) << user << setw(4) << '|' << setw(18) << name << setw(3) << '|' << setw(5) << state << setw(5) << '|' << endl;
	}
	return true;
}

void read_processes()
{
	DIR *directory;
	struct dirent *entry;
	// Tenta abrir a pasta /proc
	if ((directory = opendir("/proc")) != NULL)
	{
		int processes_count = 0;
		// Mostra o cabeçalho da tabela
		cout << setw(5) << "PID" << setw(5) << '|' << setw(18) << "User" << setw(4) << '|' << setw(18) << "PROCNAME" << setw(3) << '|' << setw(8) << "Estado" << setw(2) << '|' << endl;
		cout << "---------|---------------------|--------------------|---------|" << endl;
		// Itera sob os diretorios da /proc
		while ((entry = readdir(directory)) != NULL)
		{
			// Tenta converter o nome para um
			int folder_name_number = atoi(entry->d_name);
			// Caso tenha conseguido converter é porque é um processo
			if (folder_name_number)
			{
				processes_count++;
				// Se passar do limite de processos cai fora
				if (processes_count > PROCESSES_LIMIT)
				{
					break;
				}
				read_process(folder_name_number, true);
			}
		}
		closedir(directory);
	}
}

void send_signal()
{
	cout << "> ";

	string first_char;
	first_char = cin.get();
	// Se o usuário digitar enter cai fora
	if (first_char == "\n")
	{
		return;
	}

	// Le o PID e o sinal
	string inputed_PID, inputed_signal;
	cin >> inputed_PID >> inputed_signal;

	// Insere o 1o caracter lido no inicio da string do PID
	inputed_PID.insert(0, first_char);

	try
	{
		// Tenta converter para int
		int PID = stoi(inputed_PID);
		int signal = stoi(inputed_signal);

		// Se converteu ambas variaveis
		if (PID && signal)
		{
			// Verifica se o PID é valido
			if (read_process(PID, false))
			{
				// Envia o sinal
				kill(PID, signal);
			}
			else
			{
				cout << "PID invalido" << endl;
			}
		}
		else
		{
			cout << "Problema na leitura do PID e/ou sinal" << endl;
		}
	}
	catch (...)
	{
		cout << "Problema na leitura do PID e/ou sinal" << endl;
	}
}

int main(int argc, char **argv)
{
	while (true)
	{
		clear();
		read_processes();
		send_signal();
		sleep(1);
	}
	return 0;
}