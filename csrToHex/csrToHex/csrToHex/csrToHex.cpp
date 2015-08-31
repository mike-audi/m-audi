#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>	
#include <string>
#include <stdint.h>

using namespace std;

string getAddress(string s);
string getData(string s);
string makeCheckSum(string s);

int main(int argc, char * argv[])
{
	string inputLine, addr, data, output;
	fstream infile, outfile;
	if (argc != 3){
		cout << "Dude you need an [in].img & [out].hex filepath" << endl;
		exit(EXIT_FAILURE);
	}

	infile.open(argv[1], ios::in);
	outfile.open(argv[2], ios::out | ios::trunc);
	if (!infile.is_open()){
		cout << "Uh your file couldn't be opened" << endl;
		exit(EXIT_FAILURE);
	}

	while (getline(infile, inputLine)){
		addr = getAddress(inputLine);
		data = getData(inputLine);
		if (!addr.empty() && !data.empty()){
			output = ":04" + addr + "00" + data;
			output += makeCheckSum(output);
			outfile << output << '\r';
		}
	}
	outfile << ":00000001FF" << '\r';
	return 0;
}

string getAddress(string s){
	unsigned int addr;
	char * p;

	if (s.compare("// Control Header") == 0){
		return "";
	}else{
		s.erase(0, s.find('@')+1);
		s.erase(6);
		addr = strtoul(s.c_str(),&p,16);
		if (addr > 0xffff){
			cout << "Program doesn't support addresses over 0xffff" << endl;
			return "";
		}else{
			s.erase(0, 2);
			return s;
		}
	}
}

string getData(string s){
	if (s.compare("// Control Header") == 0){
		return "";
	}else{
		s.erase(0, s.find('@') + 1);
		s.erase(0,6);
		while(s.at(0) == ' ') 
			s.erase(0, 1);
		return s;
	}
}

string makeCheckSum(string s){
	char * p;
	char buffer[4];
	s.erase(0, 1);
	unsigned long val = 0;
	for (unsigned char i = 0; i < s.length(); i+=2){
		val += strtoul((s.substr(i, 2)).c_str(),&p,16);
	}
	signed char csum = -((signed char)(val & 0xff));
	sprintf_s(buffer,"%02x",(unsigned char)csum);
	string check(buffer);
	return check;
}