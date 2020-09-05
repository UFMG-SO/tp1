#include <iostream>
#include <fstream>
#include <iomanip>
#include "dirent.h"
#include <sys/stat.h>
#include <signal.h>

// Pode?
#include <pwd.h>
#include <unistd.h>
#include <thread>
#include <vector>

using namespace std;

const int LINE_PID = 1;
const int LINE_NAME = 3;
const int LINE_STATE = 6;

const int PROCESSES_LIMIT = 9999999;

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
			// if (folder_name_number)
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
	char *inputed_PID = new char[256];
	char *inputed_signal = new char[256];
	cout << "> ";
	cin >> inputed_PID >> inputed_signal;
	int PID = atoi(inputed_PID);
	int signal = atoi(inputed_signal);
	if (PID && signal)
	{
		if (read_process(PID, false))
		{
			// kill(PID, signal);
			cout << "PID valido" << endl;
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
	delete inputed_PID;
	delete inputed_signal;
}

void teste()
{
	while (1)
	{
		read_processes();
		sleep(1);
		clear();
	}
}

void teste2()
{
	while (1)
	{
		send_signal();
	}
}

int main(int argc, char **argv)
{
	clear();
	thread thread1 = thread(teste);
	thread thread2 = thread(teste2);
	thread1.join();
	thread2.join();
	return 0;
}
