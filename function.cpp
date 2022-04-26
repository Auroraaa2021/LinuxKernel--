#include "function.h"

char* GetUser()
{
	uid_t userid;
	struct passwd* pwd;
	userid = getuid();
	pwd = getpwuid(userid);
	return pwd->pw_name;
}

void Banner(char* user, char* buffer)
{
	cout << "\033[32m" << user;
	cout << "\033[37m" << ':';
	cout << "\033[34m" << buffer;
	cout << "\033[37m" << "$ ";
	return;
}

bool GetCommand(char* buffer)
{
	Banner(GetUser(), buffer);
	string line, t;
	vector<string> command;
	getline(cin, line);
	stringstream linestream;
	linestream.str(line);
	while(linestream >> t)
		command.push_back(t);
	if(!CommandTeller(command, buffer))
		return false;
	return true;
}

bool CommandTeller(vector<string> command, char* buffer)
{
	if(command.empty())
		return true;
	string c = command[0];
	command.erase(command.begin(), command.begin() + 1);
	if (c == "cp") 
		Copy(command);
	else if (c == "cat")
		Concatenate(command);
	else if (c == "cmp")
		Compare(command);
	else if (c == "man")
		Manual(command);
	else if (c == "wc")
		WordCount(command);
	else if (c == "pwd")
		Path(buffer);
	else if (c[0] == '.' && c[1] == '/')
	{
		if(!ExecuteSH(c, buffer))
			return false;
	}
	else if(c == "exit")
		return false;
	else 
		cout << c << ": command not found" << endl;
	return true;
}

void Path(char* buffer)
{
	cout << buffer << endl;
	return;
}

void Copy(vector<string> command)
{
	if(command[0][0] == '-' && command[0] != "-r" && command[0] != "--recursive")
	{
		cout << "cp: invalid option -- \'" << command[0].substr(1, command[0].length() - 1) << "\'" << endl;
			return;
	}
	if(command[0] == "-r" || command[0] == "--recursive")
	{
		if(command.size() == 1)
		{
			cout << "cp: missing file operand" << endl;
			return;
		}
		else if(command.size() == 2)
		{
			cout << "cp: missing destination file operand after \'" << command[1] << "\'" << endl;
			return;
		}
		else if(command.size() == 3)
		{
			DIR* d = opendir(command[1].c_str());
			struct stat dstat;
			if(d == NULL)
			{
				vector<string> command1;
				command1.push_back(command[1]);
				command1.push_back(command[2]);
				Copy(command1);
			}
			else if(stat(command[1].c_str(), &dstat) != 0)
			{
				cout << "cp: cannot stat \'" << command[1] 
				<< "\': No such file or directory" << endl;
				return;
			}
			else
			{
				mkdir((command[2] + "/" + command[1]).c_str(), dstat.st_mode);
				struct dirent* d1;
				while(d1 = readdir(d))
				{
					vector<string> command1;
					string named1 = d1->d_name;
					if(d1->d_type == DT_DIR && named1 != "." && named1 != "..")
					{
						command1.push_back(command[0]);
						command1.push_back(command[1] + "/" + named1);
						command1.push_back(command[2]);
						Copy(command1);
					}
					else if(d1->d_type != DT_DIR)
					{
						command1.push_back(named1);
						command1.push_back(command[2] + "/" + command[1]);
						Copy(command1);
					}					
				}
			}
			closedir(d);
		}
		else
		{
			vector<string> command1;
			command1.push_back(command[0]);
			command1.push_back(command[1]);
			command1.push_back(command[command.size() - 1]);
			Copy(command1);
			command.erase(command.begin() + 1, command.begin() + 2);
			Copy(command);
		}
		
	}
	else
	{
		if(command.size() == 0)
		{
			cout << "cp: missing file operand" << endl;
			return;
		}
		else if(command.size() == 1)
		{
			cout << "cp: missing destination file operand after \'" 
			<< command[0] << "\'" << endl;
			return;
		}
		else if(command.size() == 2)
		{
			ifstream f1(command[0].c_str());
			struct stat f1stat;
			if(!f1.is_open() || stat(command[0].c_str(), &f1stat) != 0)
			{
				cout << "cp: cannot stat \'" << command[0] 
				<< "\': No such file or directory" << endl;
				return;
			}
			DIR* d = opendir(command[1].c_str());
			if(d)
			{
				string ss = "/";
				command[1] = command[1] + ss + command[0];
			}
			if (command[0] == command[1])
			{
				cout << "cp: \'" << command[0] << "\' and \'" 
					<< command[1] << "\' are the same file" << endl;
				f1.close();
				return;
			}		
			ofstream f2(command[1].c_str(), ios::out | ios::trunc);
			chmod(command[1].c_str(), f1stat.st_mode);
			char c;
			f1 >> noskipws >> c;
			while(f1.good())
			{
				f2 << c;
				f1 >> noskipws >> c;
			}
			f1 >> skipws;
			f1.close();
			f2.close();
		}
		else
		{
			DIR* d = opendir(command[command.size() - 1].c_str());
			if(!d)
			{
				cout << "cp: target \'" << command[command.size() - 1]
				<< "\' is not a directory" << endl;
				return;
			}
			vector<string> command1;
			command1.push_back(command[0]);
			command1.push_back(command[command.size() - 1]);
			Copy(command1);
			command.erase(command.begin(), command.begin() + 1);
			Copy(command);
		}
	}
}

void Compare(vector<string> command)
{
	if(command.size() == 0 || command.size() == 1)
	{
		cout << "cmp: missing file operand" << endl;
		return;
	}
	if(command.size() > 2)
	{
		cout << "cmp: invalid --ignore-initial value \'" << command[2] << "\'" << endl;
		return;
	}
	ifstream f1(command[0].c_str());
	if(!f1.is_open())
	{
		cout << "cmp: " << command[0] << ": No such file or directory" << endl;
		return;
	}
	ifstream f2(command[1].c_str());
	if(!f2.is_open())
	{
		cout << "cmp: " << command[1] << ": No such file or directory" << endl;
		f1.close();
		return;
	}
	char c1, c2;
	f1 >> noskipws >> c1;
	f2 >> noskipws >> c2;
	int b = 0, l = 0;
	while(f1.good() && f2.good())
	{
		++b;
		if(c1 == '\n' && c2 == '\n')
			++l;
		if(c1 != c2)
		{
			cout << command[0] << ' ' << command[1] << " differ: byte " << b << ", line " << l + 1 << endl;
			f1 >> skipws;
			f2 >> skipws;
			f1.close();
			f2.close();
			return;
		}
		f1 >> noskipws >> c1;
		f2 >> noskipws >> c2;
	}
	if(!f1.good() && f2.good())
		cout << "cmp: EOF on " << command[0] << endl;
	else if(f1.good() && !f2.good())
		cout << "cmp: EOF on " << command[1] << endl;
	f1.close();
	f2.close();
	return;
}

void Concatenate(vector<string> command)
{
	if(command.size() == 0)
	{
		cout << "cat: missing file operand" << endl;
		return;
	}
	else for(unsigned int k = 0; k < command.size(); ++k)
	{
		if(!PrintFlie(command[k]))
		{
			cout << "cat: " << command[k] << ": No such file or directory" << endl;
			continue;
		}
	}
	return;
}

bool PrintFlie(string s)
{
	ifstream f(s.c_str());
	if(!f.is_open())
		return false;
	char c;
	f >> noskipws >> c;
	while(f.good())
	{
		cout << c;
		f >> noskipws >> c;
	}
	f >> skipws;
	f.close();
	return true;
}

void WordCount(vector<string> command)
{
	if(command.empty())
	{
		cout << "wc:  missing file operand" << endl;
		return;
	}
	bool bl = false, bw = false, bb = false;
	if(command[0][0] != '-')
	{
		bl = true;
		bw = true;
		bb = true;
	}
	while(!command.empty())
	{
		if(command[0][0] != '-')
			break;
		if(command[0] == "-c" || command[0] == "--bytes")
			bb = true;
		else if(command[0] == "-w" || command[0] == "--words")
			bw = true;
		else if(command[0] == "-l" || command[0] == "--lines")
			bl = true;
		else
		{
			cout << "wc: invalid option -- \'" << command[0].substr(1, command[0].length() - 1) << "\'" << endl;
			return;
		}
		command.erase(command.begin(), command.begin() + 1);
	}
	int line = 0, word = 0, byte = 0;
	if(command.empty())
	{
		cout << "wc:  missing file operand" << endl;
		return;
	}
	for(unsigned int i = 0; i < command.size(); ++i)
	{
		int l = 0, w = 0, b = 0;
		ifstream f(command[i].c_str());
		if(!f.is_open())
		{
			cout << "wc: " << command[i] << ": No such file or directory" << endl;
			continue;
		}
		string s;
		f >> s;
		while(f.good())
		{
			++w;
			b += s.length();
			f >> s;
		}
		f.close();
		ifstream f1(command[i].c_str());
		char c;
		f1 >> noskipws >> c;
		while(f1.good())
		{
			if(c == '\n')
			{
				++b;
				++l;
			}
			else if(c == ' ') ++b;
			f1 >> noskipws >> c;
		}
		f1 >> skipws;
		f1.close();
		if(bl)
			cout << setw(3) << l << ' ';
		if(bw)
			cout << setw(3) << w << ' ';
		if(bb)
			cout << setw(3) << b << ' ';
		cout << command[i] << endl;
		line += l;
		word += w;
		byte += b;
	}
	if(command.size() >= 2)
	{
		if(bl)
			cout << setw(3) << line << ' ';
		if(bw)
			cout << setw(3) << word << ' ';
		if(bb)
			cout << setw(3) << byte << ' ';
		cout << "total" << endl;
	}
	return;
}

void Manual(vector<string> command)
{
	for(unsigned int k = 0; k < command.size(); ++k)
	{
		if (k != 0)
			cout << endl;
		if(!PrintFlie(command[k]))
			cout << "No manual entry for " << command[k] << endl;
	}
	return;
}

bool ExecuteSH(string f, char* buffer)
{
	ifstream f1(f.substr(2).c_str());
	if(!f1.is_open())
	{
		cout << "bash: " << f << ": No such file or directory" << endl;
		return true;
	}
	while(f1.good())
	{
		string line, t;
		vector<string> command;
		getline(f1, line);
		for (int i = 0; i < line.length(); ++i)
		{
			if(line[i] == '#')
			{
				line = line.substr(0, i);
				break;
			}
		}
		stringstream linestream;
		linestream.str(line);
		while(linestream >> t)
			command.push_back(t);
		if (command.empty())
			continue;
		if(!CommandTeller(command, buffer))
		{
			f1.close();
			return false;
		}
	}
	return true;
}