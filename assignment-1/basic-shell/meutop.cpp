#include <iostream>
#include <fstream>
#include <iomanip>
#include "dirent.h"
#include <sys/stat.h>

// Pode?
#include <pwd.h>
#include <unistd.h>

using namespace std;

const int LINE_PID = 1;
const int LINE_NAME = 3;
const int LINE_STATE = 6;

const int PROCESSES_LIMIT = 20;

void clear()
{
	printf("\033[H\033[J");
}

void print_status(int PID)
{
	string path = "/proc/" + to_string(PID) + "/status";
	ifstream file(path);

	if (!file.is_open())
	{
		cout << "Processo de PID " << PID << " nao encontrado" << endl;
	}

	string line;
	string id = "", user = "", name = "", state = "";
	int line_counter = 0;
	while (getline(file, line))
	{
		line_counter++;
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

	file.close();
	cout << setw(5) << id << setw(5) << '|' << setw(8) << user << setw(3) << '|' << setw(18) << name << setw(3) << '|' << setw(5) << state << setw(5) << '|' << endl;
}

void read_processes()
{
	DIR *directory;
	struct dirent *entry;
	if ((directory = opendir("/proc")) != NULL)
	{
		int directory_count = 0;
		cout << setw(5) << "PID" << setw(5) << '|' << setw(8) << "User" << setw(3) << '|' << setw(18) << "PROCNAME" << setw(3) << '|' << setw(8) << "Estado" << setw(2) << '|' << endl;
		cout << "---------|----------|--------------------|---------|" << endl;
		while ((entry = readdir(directory)) != NULL)
		{
			int folder_name_number = atoi(entry->d_name);
			if (folder_name_number && folder_name_number > 999)
			{
				directory_count++;
				if (directory_count > PROCESSES_LIMIT)
				{
					break;
				}
				print_status(folder_name_number);
			}
		}
		closedir(directory);
	}
}

void sent_signal()
{
}

int main(int argc, char **argv)
{
	while (true)
	{
		cout << "LIMPA TELA" << endl;
		read_processes();
		sleep(1);
	}
}
