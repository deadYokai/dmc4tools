#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>

using namespace std;

void conv2msg(char* fname){
	char* textfn = new char[strlen(fname)+4];
	strcpy(textfn, fname);
	strcat(textfn, ".txt");

	char* msg2fn = new char[strlen(fname)+1];
	strcpy(msg2fn, fname);
	strcat(msg2fn, "2");


	std::ifstream msgFile(fname, std::ios::binary);
	if(!msgFile.is_open()){
		printf("Unable to open file: %s\n", fname);
		exit(1);
	}

	char arr[4];
	char arr2[2];
	array<array<char, 4>, 2000> arr3;
	char arr4[8];
	char arr5[52];

	int n = 0;
	int n2 = 0;
	int n3 = 0;

	msgFile.read(reinterpret_cast<char*>(&arr4), 8);
	if(arr4[0] != 'M' || arr4[1] != 'S' || arr4[2] != 'G' || arr4[3] != '2')
	{
		printf("File '%s' if not MSG2\n", fname);
		exit(1);
	}

	msgFile.read(reinterpret_cast<char*>(&arr), 4);
	msgFile.read(reinterpret_cast<char*>(&arr5), 52);
	msgFile.seekg(64, ios::beg);

	while(n != -1)
	{
		n = msgFile.get();
		msgFile.seekg(-1, ios::cur);
		if(n == -1) break;

		msgFile.read(reinterpret_cast<char*>(&arr), 4);

		if(arr[0] == 8 && arr[1] == 1)
		{
			arr3[n2][0] = arr[0];
			arr3[n2][1] = arr[1];
			arr3[n2][2] = arr[2];
			arr3[n2][3] = arr[3];
			n2++;
		}

		if(arr[0] == 0)
		{
			if((arr[2] != (char)0x10 || arr[3] != 4) && (arr[2] != 3 || arr[3] != 4) && ((arr[0] == 0 && arr[1] == 0) || arr[2] == (char)0x11))
			{
				arr3[n2][0] = arr[0];
				arr3[n2][1] = arr[1];
				arr3[n2][2] = arr[2];
				arr3[n2][3] = arr[3];
				n2++;

			}
		}
		else if(arr[0] == 1)
		{
			arr3[n2][0] = arr[0];
			arr3[n2][1] = arr[1];
			arr3[n2][2] = arr[2];
			arr3[n2][3] = arr[3];
			n2++;
		}
	}

	msgFile.close();

	std::ifstream txtFile(textfn, std::ios::binary);
	if(!txtFile.is_open()){
		printf("Unable to open file: %s\n", textfn);
		exit(1);
	}

	txtFile.seekg(0, ios::end);
	size_t txtSize = static_cast<size_t>(txtFile.tellg());


	char endCheck[1];
	txtFile.seekg(-1, ios::end);
	txtFile.read(endCheck, 1);
	int off = 0;
	while(endCheck[0] != '\xa4'){
		txtFile.seekg(-2, ios::cur);
		txtFile.read(endCheck, 1);
		off++;
	}

	if(off > 1){
		txtSize = txtSize - off + 1;
		txtFile.seekg(0, ios::beg);
		char* buff = new char[txtSize];
		txtFile.read(buff, txtSize);
		txtFile.close();

		ofstream modTxt(textfn);
		modTxt.write(buff, txtSize);
		modTxt.close();
		delete[] buff;
		txtFile.open(textfn, ios::binary);
	}

	int n4;
	txtSize = txtSize-2;

	txtFile.seekg(2, ios::beg);
	while(n4 < txtSize)
	{
		txtFile.read(reinterpret_cast<char*>(&arr2), 2);
		n4++;
		if(arr2[0] == (char)13)
		{
			txtFile.read(reinterpret_cast<char*>(&arr2), 2);
			if(arr2[0] == (char)10)
			{
				n3++;
				n4 += 2;
			}
		}
		n4++;
	}

	std::ofstream newMsgFile(msg2fn, std::ios::binary);
	newMsgFile.write(arr4, 8);
	long n5 = (64 + n3 * 4) + (txtSize / 2 - (n3 * 2)) * 4;
	memcpy(arr, &n5, sizeof(arr));
	newMsgFile.write(arr, 4);
	newMsgFile.write(arr5, 52);
	txtFile.seekg(2, ios::beg);
	n4 = 0;
	while(n2 != n4)
	{
		txtFile.read(reinterpret_cast<char*>(&arr2), 2);

		if(arr2[0] == (char)164)
		{
			arr[0] = arr3[n4][0];
			arr[1] = arr3[n4][1];
			arr[2] = arr3[n4][2];
			arr[3] = arr3[n4][3];
			newMsgFile.write(arr, 4);
			n4++;
		}
		else
		{
			if(arr2[1] == 0)
			{
				uint8_t b = (uint8_t)arr2[0];
				switch (b)
				{
					case 13:
						txtFile.read(reinterpret_cast<char*>(&arr2), 2);
						arr[0] = (char)0;
						arr[1] = (char)0;
						arr[2] = (char)16;
						arr[3] = (char)4;
						break;
					case 34:
						arr[0] = '(';
						arr[1] = (char)0;
						arr[2] = ')';
						arr[3] = (char)0;
						break;
					case 32:
						arr[0] = (char)28;
						arr[1] = (char)0;
						arr[2] = (char)40;
						arr[3] = (char)0;
						break;
					case 33:
						arr[0] = (char)20;
						arr[1] = (char)0;
						arr[2] = (char)23;
						arr[3] = (char)0;
						break;
					case 39:
						arr[0] = (char)42;
						arr[1] = (char)0;
						arr[2] = (char)23;
						arr[3] = (char)0;
						break;
					case 44:
						arr[0] = (char)36;
						arr[1] = (char)0;
						arr[2] = (char)22;
						arr[3] = (char)0;
						break;
					case 45:
						arr[0] = (char)46;
						arr[1] = (char)0;
						arr[2] = (char)32;
						arr[3] = (char)0;
						break;
					case 46:
						arr[0] = (char)38;
						arr[1] = (char)0;
						arr[2] = (char)23;
						arr[3] = (char)0;
						break;
					case 48:
						arr[0] = (char)0;
						arr[1] = (char)0;
						arr[2] = (char)77;
						arr[3] = (char)0;
						break;
					case 49:
						arr[0] = (char)2;
						arr[1] = (char)0;
						arr[2] = (char)46;
						arr[3] = (char)0;
						break;
					case 50:
						arr[0] = (char)4;
						arr[1] = (char)0;
						arr[2] = (char)68;
						arr[3] = (char)0;
						break;
					case 51:
						arr[0] = (char)6;
						arr[1] = (char)0;
						arr[2] = (char)68;
						arr[3] = (char)0;
						break;
					case 52:
						arr[0] = (char)8;
						arr[1] = (char)0;
						arr[2] = (char)75;
						arr[3] = (char)0;
						break;
					case 53:
						arr[0] = (char)10;
						arr[1] = (char)0;
						arr[2] = (char)70;
						arr[3] = (char)0;
						break;
					case 54:
						arr[0] = (char)12;
						arr[1] = (char)0;
						arr[2] = (char)72;
						arr[3] = (char)0;
						break;
					case 55:
						arr[0] = (char)14;
						arr[1] = (char)0;
						arr[2] = (char)66;
						arr[3] = (char)0;
						break;
					case 56:
						arr[0] = (char)16;
						arr[1] = (char)0;
						arr[2] = (char)72;
						arr[3] = (char)0;
						break;
					case 57:
						arr[0] = (char)18;
						arr[1] = (char)0;
						arr[2] = (char)73;
						arr[3] = (char)0;
						break;
					case 63:
						arr[0] = (char)22;
						arr[1] = (char)0;
						arr[2] = (char)60;
						arr[3] = (char)0;
						break;
					case 65:
						arr[0] = (char)56;
						arr[1] = (char)0;
						arr[2] = (char)97;
						arr[3] = (char)0;
						break;
					case 66:
						arr[0] = (char)58;
						arr[1] = (char)0;
						arr[2] = (char)83;
						arr[3] = (char)0;
						break;
					case 67:
						arr[0] = (char)60;
						arr[1] = (char)0;
						arr[2] = (char)82;
						arr[3] = (char)0;
						break;
					case 68:
						arr[0] = (char)62;
						arr[1] = (char)0;
						arr[2] = (char)91;
						arr[3] = (char)0;
						break;
					case 69:
						arr[0] = (char)64;
						arr[1] = (char)0;
						arr[2] = (char)81;
						arr[3] = (char)0;
						break;
					case 70:
						arr[0] = (char)66;
						arr[1] = (char)0;
						arr[2] = (char)77;
						arr[3] = (char)0;
						break;
					case 71:
						arr[0] = (char)68;
						arr[1] = (char)0;
						arr[2] = (char)91;
						arr[3] = (char)0;
						break;
					case 72:
						arr[0] = (char)70;
						arr[1] = (char)0;
						arr[2] = (char)79;
						arr[3] = (char)0;
						break;
					case 73:
						arr[0] = (char)72;
						arr[1] = (char)0;
						arr[2] = (char)30;
						arr[3] = (char)0;
						break;
					case 74:
						arr[0] = (char)74;
						arr[1] = (char)0;
						arr[2] = (char)71;
						arr[3] = (char)0;
						break;
					case 75:
						arr[0] = (char)76;
						arr[1] = (char)0;
						arr[2] = (char)84;
						arr[3] = (char)0;
						break;
					case 76:
						arr[0] = (char)78;
						arr[1] = (char)0;
						arr[2] = (char)75;
						arr[3] = (char)0;
						break;
					case 77:
						arr[0] = (char)80;
						arr[1] = (char)0;
						arr[2] = (char)111;
						arr[3] = (char)0;
						break;
					case 78:
						arr[0] = (char)82;
						arr[1] = (char)0;
						arr[2] = (char)90;
						arr[3] = (char)0;
						break;
					case 79:
						arr[0] = (char)84;
						arr[1] = (char)0;
						arr[2] = (char)99;
						arr[3] = (char)0;
						break;
					case 80:
						arr[0] = (char)86;
						arr[1] = (char)0;
						arr[2] = (char)72;
						arr[3] = (char)0;
						break;
					case 81:
						arr[0] = (char)88;
						arr[1] = (char)0;
						arr[2] = (char)81;
						arr[3] = (char)0;
						break;
					case 82:
						arr[0] = (char)90;
						arr[1] = (char)0;
						arr[2] = (char)81;
						arr[3] = (char)0;
						break;
					case 83:
						arr[0] = (char)92;
						arr[1] = (char)0;
						arr[2] = (char)77;
						arr[3] = (char)0;
						break;
					case 84:
						arr[0] = (char)94;
						arr[1] = (char)0;
						arr[2] = (char)81;
						arr[3] = (char)0;
						break;
					case 85:
						arr[0] = (char)96;
						arr[1] = (char)0;
						arr[2] = (char)126;
						arr[3] = (char)0;
						break;
					case 86:
						arr[0] = (char)98;
						arr[1] = (char)0;
						arr[2] = (char)84;
						arr[3] = (char)0;
						break;
					case 87:
						arr[0] = (char)100;
						arr[1] = (char)0;
						arr[2] = (char)125;
						arr[3] = (char)0;
						break;
					case 88:
						arr[0] = (char)102;
						arr[1] = (char)0;
						arr[2] = (char)125;
						arr[3] = (char)0;
						break;
					case 89:
						arr[0] = (char)104;
						arr[1] = (char)0;
						arr[2] = (char)80;
						arr[3] = (char)0;
						break;
					case 94:
						arr[0] = (char)106;
						arr[1] = (char)0;
						arr[2] = (char)73;
						arr[3] = (char)0;
						break;
					case 97:
						arr[0] = (char)108;
						arr[1] = (char)0;
						arr[2] = (char)61;
						arr[3] = (char)0;
						break;
					case 98:
						arr[0] = (char)110;
						arr[1] = (char)0;
						arr[2] = (char)71;
						arr[3] = (char)0;
						break;
					case 99:
						arr[0] = (char)112;
						arr[1] = (char)0;
						arr[2] = (char)64;
						arr[3] = (char)0;
						break;
					case 100:
						arr[0] = (char)114;
						arr[1] = (char)0;
						arr[2] = (char)69;
						arr[3] = (char)0;
						break;
					case 101:
						arr[0] = (char)116;
						arr[1] = (char)0;
						arr[2] = (char)65;
						arr[3] = (char)0;
						break;
					case 102:
						arr[0] = (char)118;
						arr[1] = (char)0;
						arr[2] = (char)43;
						arr[3] = (char)0;
						break;
					case 103:
						arr[0] = (char)120;
						arr[1] = (char)0;
						arr[2] = (char)67;
						arr[3] = (char)0;
						break;
					case 104:
						arr[0] = (char)122;
						arr[1] = (char)0;
						arr[2] = (char)69;
						arr[3] = (char)0;
						break;
					case 105:
						arr[0] = (char)124;
						arr[1] = (char)0;
						arr[2] = (char)29;
						arr[3] = (char)0;
						break;
					case 106:
						arr[0] = (char)126;
						arr[1] = (char)0;
						arr[2] = (char)24;
						arr[3] = (char)0;
						break;
					case 107:
						arr[0] = (char)128;
						arr[1] = (char)0;
						arr[2] = (char)70;
						arr[3] = (char)0;
						break;
					case 108:
						arr[0] = (char)130;
						arr[1] = (char)0;
						arr[2] = (char)28;
						arr[3] = (char)0;
						break;
					case 109:
						arr[0] = (char)132;
						arr[1] = (char)0;
						arr[2] = (char)109;
						arr[3] = (char)0;
						break;
					case 110:
						arr[0] = (char)134;
						arr[1] = (char)0;
						arr[2] = (char)68;
						arr[3] = (char)0;
						break;
					case 111:
						arr[0] = (char)136;
						arr[1] = (char)0;
						arr[2] = (char)69;
						arr[3] = (char)0;
						break;
					case 112:
						arr[0] = (char)138;
						arr[1] = (char)0;
						arr[2] = (char)70;
						arr[3] = (char)0;
						break;
					case 113:
						arr[0] = (char)140;
						arr[1] = (char)0;
						arr[2] = (char)68;
						arr[3] = (char)0;
						break;
					case 114:
						arr[0] = (char)142;
						arr[1] = (char)0;
						arr[2] = (char)44;
						arr[3] = (char)0;
						break;
					case 115:
						arr[0] = (char)144;
						arr[1] = (char)0;
						arr[2] = (char)59;
						arr[3] = (char)0;
						break;
					case 116:
						arr[0] = (char)146;
						arr[1] = (char)0;
						arr[2] = (char)40;
						arr[3] = (char)0;
						break;
					case 117:
						arr[0] = (char)148;
						arr[1] = (char)0;
						arr[2] = (char)70;
						arr[3] = (char)0;
						break;
					case 118:
						arr[0] = (char)150;
						arr[1] = (char)0;
						arr[2] = (char)65;
						arr[3] = (char)0;
						break;
					case 119:
						arr[0] = (char)152;
						arr[1] = (char)0;
						arr[2] = (char)95;
						arr[3] = (char)0;
						break;
					case 120:
						arr[0] = (char)154;
						arr[1] = (char)0;
						arr[2] = (char)95;
						arr[3] = (char)0;
						break;
					case 121:
						arr[0] = (char)156;
						arr[1] = (char)0;
						arr[2] = (char)67;
						arr[3] = (char)0;
						break;
					case 122:
						arr[0] = (char)158;
						arr[1] = (char)0;
						arr[2] = (char)55;
						arr[3] = (char)0;
						break;
					case 124:
						arr[0] = (char)0;
						arr[1] = (char)0;
						arr[2] = (char)3;
						arr[3] = (char)4;
						break;
					case 201:
						arr[0] = (char)184;
						arr[1] = 0;
						arr[2] = (char)81;
						arr[3] = 0;
						break;
					case 193:
						arr[0] = (char)174;
						arr[1] = 0;
						arr[2] = (char)97;
						arr[3] = 0;
						break;
					case 211:
						arr[0] = (char)202;
						arr[1] = 0;
						arr[2] = (char)99;
						arr[3] = 0;
						break;
					case 214:
						arr[0] = (char)206;
						arr[1] = 0;
						arr[2] = (char)99;
						arr[3] = 0;
						break;
					case 205:
						arr[0] = (char)192;
						arr[1] = 0;
						arr[2] = (char)30;
						arr[3] = 0;
						break;
					case 218:
						arr[0] = (char)208;
						arr[1] = 0;
						arr[2] = (char)126;
						arr[3] = 0;
						break;
					case 220:
						arr[0] = (char)212;
						arr[1] = 0;
						arr[2] = (char)126;
						arr[3] = 0;
						break;
					case 224:
						arr[0] = (char)0xD8;
						arr[1] = 0;
						arr[2] = (char)0x3D;
						arr[3] = 0;
						break;
					case 226:
						arr[0] = (char)0xDC;
						arr[1] = 0;
						arr[2] = (char)0x3D;
						arr[3] = 0;
						break;
					case 225:
						arr[0] = (char)218;
						arr[1] = 0;
						arr[2] = (char)61;
						arr[3] = 0;
						break;
					case 233:
						arr[0] = (char)228;
						arr[1] = 0;
						arr[2] = (char)65;
						arr[3] = 0;
						break;
					case 234:
						arr[0] = (char)0xe6;
						arr[1] = 0;
						arr[2] = (char)0x41;
						arr[3] = 0;
						break;
					case 237:
						arr[0] = (char)236;
						arr[1] = 0;
						arr[2] = (char)29;
						arr[3] = 0;
						break;
					case 243:
						arr[0] = (char)246;
						arr[1] = 0;
						arr[2] = (char)69;
						arr[3] = 0;
						break;
					case 246:
						arr[0] = (char)250;
						arr[1] = 0;
						arr[2] = (char)69;
						arr[3] = 0;
						break;
					case 250:
						arr[0] = (char)254;
						arr[1] = 0;
						arr[2] = (char)70;
						arr[3] = 0;
						break;
					case 252:
						arr[0] = 2;
						arr[1] = 1;
						arr[2] = (char)70;
						arr[3] = 0;
						break;
					case 242:
						arr[0] = (char)244;
						arr[1] = 0;
						arr[2] = (char)69;
						arr[3] = 0;
						break;
					case 191:
						arr[0] = (char)166;
						arr[1] = 0;
						arr[2] = (char)60;
						arr[3] = 0;
						break;
					case 202:
						arr[1] = 0;
						arr[3] = 0;
						arr[0] = (char)0xba;
						arr[2] = (char)0x51;
						break;
					case 231:
						arr[0] = (char)0xe0;
						arr[2] = (char)0x40;
						arr[1] = 0;
						arr[3] = 0;
						break;
					case 239:
						arr[0] = (char)0xf0;
						arr[2] = (char)0x1d;
						arr[1] = 0;
						arr[3] = 0;
						break;
					case 199:
						arr[0] = (char)0xb4;
						arr[2] = (char)0x52;
						arr[3] = 0;
						arr[1] = 0;
						break;
					case 210:
						arr[0] = (char)0xc8; 
						arr[2] = (char)0x63;
						arr[3] = 0;
						arr[1] = 0;
						break;
					default:
						arr[0] = (char)22;
						arr[1] = 0;
						arr[2] = (char)60;
						arr[3] = 0;
						break;
				}
			}else if(arr2[1] == 4){
				uint8_t bb = (uint8_t)arr2[0];
				switch(bb){
					case 0x10:
						arr[0] = 0x20;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x11:
						arr[0] = 0x22;
						arr[1] = 0x01;
						arr[2] = (char)71;
						arr[3] = 0;
						break;
					case 0x12:
						arr[0] = 0x24;
						arr[1] = 0x01;
						arr[2] = (char)79;
						arr[3] = 0;
						break;
					case 0x13:
						arr[0] = 0x26;
						arr[1] = 0x01;
						arr[2] = (char)56;
						arr[3] = 0;
						break;
					case 0x90:
						arr[0] = 0x28;
						arr[1] = 0x01;
						arr[2] = (char)56;
						arr[3] = 0;
						break;
					case 0x14:
						arr[0] = 0x2a;
						arr[1] = 0x01;
						arr[2] = (char)87;
						arr[3] = 0;
						break;
					case 0x15:
						arr[0] = 0x2c;
						arr[1] = 0x01;
						arr[2] = (char)75;
						arr[3] = 0;
						break;
					case 0x04:
						arr[0] = 0x2e;
						arr[1] = 0x01;
						arr[2] = (char)79;
						arr[3] = 0;
						break;
					case 0x16:
						arr[0] = 0x30;
						arr[1] = 0x01;
						arr[2] = (char)119;
						arr[3] = 0;
						break;
					case 0x17:
						arr[0] = 0x32;
						arr[1] = 0x01;
						arr[2] = (char)74;
						arr[3] = 0;
						break;
					case 0x18:
						arr[0] = 0x34;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x06:
						arr[0] = 0x36;
						arr[1] = 0x01;
						arr[2] = (char)28;
						arr[3] = 0;
						break;
					case 0x07:
						arr[0] = 0x38;
						arr[1] = 0x01;
						arr[2] = (char)50;
						arr[3] = 0;
						break;
					case 0x19:
						arr[0] = 0x3a;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x1a:
						arr[0] = 0x3c;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x1b:
						arr[0] = 0x3e;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x1c:
						arr[0] = 0x40;
						arr[1] = 0x01;
						arr[2] = (char)100;
						arr[3] = 0;
						break;
					case 0x1d:
						arr[0] = 0x42;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x1e:
						arr[0] = 0x44;
						arr[1] = 0x01;
						arr[2] = (char)92;
						arr[3] = 0;
						break;
					case 0x1f:
						arr[0] = 0x46;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x20:
						arr[0] = 0x48;
						arr[1] = 0x01;
						arr[2] = (char)72;
						arr[3] = 0;
						break;
					case 0x21:
						arr[0] = 0x4a;
						arr[1] = 0x01;
						arr[2] = (char)80;
						arr[3] = 0;
						break;
					case 0x22:
						arr[0] = 0x4c;
						arr[1] = 0x01;
						arr[2] = (char)80;
						arr[3] = 0;
						break;
					case 0x23:
						arr[0] = 0x4e;
						arr[1] = 0x01;
						arr[2] = (char)80;
						arr[3] = 0;
						break;
					case 0x24:
						arr[0] = 0x50;
						arr[1] = 0x01;
						arr[2] = (char)106;
						arr[3] = 0;
						break;
					case 0x25:
						arr[0] = 0x52;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x26:
						arr[0] = 0x54;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
					case 0x27:
						arr[0] = 0x56;
						arr[1] = 0x01;
						arr[2] = (char)72;
						arr[3] = 0;
						break;
					case 0x28:
						arr[0] = 0x58;
						arr[1] = 0x01;
						arr[2] = (char)112;
						arr[3] = 0;
						break;
					case 0x29:
						arr[0] = 0x5a;
						arr[1] = 0x01;
						arr[2] = (char)112;
						arr[3] = 0;
						break;
					case 0x2c:
						arr[0] = 0x5c;
						arr[1] = 0x01;
						arr[2] = (char)72;
						arr[3] = 0;
						break;
					case 0x2e:
						arr[0] = 0x5e;
						arr[1] = 0x01;
						arr[2] = (char)118;
						arr[3] = 0;
						break;
					case 0x2f:
						arr[0] = 0x60;
						arr[1] = 0x01;
						arr[2] = (char)86;
						arr[3] = 0;
						break;
						/// 
						///
						///
						/// SMOL LETTERS
						///
						///
						///
						///
					case 0x30:
						arr[0] = 0x62;
						arr[1] = 0x01;
						arr[2] = (char)56;
						arr[3] = 0;
						break;
					case 0x31:
						arr[0] = 0x64;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x32:
						arr[0] = 0x66;
						arr[1] = 0x01;
						arr[2] = (char)60;
						arr[3] = 0;
						break;
					case 0x33:
						arr[0] = 0x68;
						arr[1] = 0x01;
						arr[2] = (char)48;
						arr[3] = 0;
						break;
					case 0x91:
						arr[0] = 0x6a;
						arr[1] = 0x01;
						arr[2] = (char)48;
						arr[3] = 0;
						break;
					case 0x34:
						arr[0] = 0x6c;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x35:
						arr[0] = 0x6e;
						arr[1] = 0x01;
						arr[2] = (char)56;
						arr[3] = 0;
						break;
					case 0x54:
						arr[0] = 0x70;
						arr[1] = 0x01;
						arr[2] = (char)56;
						arr[3] = 0;
						break;
					case 0x36:
						arr[0] = 0x72;
						arr[1] = 0x01;
						arr[2] = (char)92;
						arr[3] = 0;
						break;
					case 0x37:
						arr[0] = 0x74;
						arr[1] = 0x01;
						arr[2] = (char)52;
						arr[3] = 0;
						break;
					case 0x38:
						arr[0] = 0x76;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x56:
						arr[0] = 0x78;
						arr[1] = 0x01;
						arr[2] = (char)24;
						arr[3] = 0;
						break;
					case 0x57:
						arr[0] = 0x7a;
						arr[1] = 0x01;
						arr[2] = (char)40;
						arr[3] = 0;
						break;
					case 0x39:
						arr[0] = 0x7c;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x3a:
						arr[0] = 0x7e;
						arr[1] = 0x01;
						arr[2] = (char)68;
						arr[3] = 0;
						break;
					case 0x3b:
						arr[0] = 0x80;
						arr[1] = 0x01;
						arr[2] = (char)60;
						arr[3] = 0;
						break;
					case 0x3c:
						arr[0] = 0x82;
						arr[1] = 0x01;
						arr[2] = (char)76;
						arr[3] = 0;
						break;
					case 0x3d:
						arr[0] = 0x84;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x3e:
						arr[0] = 0x86;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x3f:
						arr[0] = 0x88;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x40:
						arr[0] = 0x8a;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x41:
						arr[0] = 0x8c;
						arr[1] = 0x01;
						arr[2] = (char)56;
						arr[3] = 0;
						break;
					case 0x42:
						arr[0] = 0x8e;
						arr[1] = 0x01;
						arr[2] = (char)96;
						arr[3] = 0;
						break;
					case 0x43:
						arr[0] = 0x90;
						arr[1] = 0x01;
						arr[2] = (char)64;
						arr[3] = 0;
						break;
					case 0x44:
						arr[0] = 0x92;
						arr[1] = 0x01;
						arr[2] = (char)80;
						arr[3] = 0;
						break;
					case 0x45:
						arr[0] = 0x94;
						arr[1] = 0x01;
						arr[2] = (char)60;
						arr[3] = 0;
						break;
					case 0x46:
						arr[0] = 0x96;
						arr[1] = 0x01;
						arr[2] = (char)68;
						arr[3] = 0;
						break;
					case 0x47:
						arr[0] = 0x98;
						arr[1] = 0x01;
						arr[2] = (char)60;
						arr[3] = 0;
						break;
					case 0x48:
						arr[0] = 0x9a;
						arr[1] = 0x01;
						arr[2] = (char)96;
						arr[3] = 0;
						break;
					case 0x49:
						arr[0] = 0x9c;
						arr[1] = 0x01;
						arr[2] = (char)100;
						arr[3] = 0;
						break;
					case 0x4c:
						arr[0] = 0x9e;
						arr[1] = 0x01;
						arr[2] = (char)56;
						arr[3] = 0;
						break;
					case 0x4e:
						arr[0] = 0xa0;
						arr[1] = 0x01;
						arr[2] = (char)88;
						arr[3] = 0;
						break;
					case 0x4f:
						arr[0] = 0xa2;
						arr[1] = 0x01;
						arr[2] = (char)56;
						arr[3] = 0;
						break;
					default:
						arr[0] = (char)22;
						arr[1] = 0;
						arr[2] = (char)60;
						arr[3] = 0;
						break;
				}
			}else{
				uint8_t b2 = (uint8_t)arr2[0];
				switch (b2) {
					case 80:
						arr[0] = (char)204;
						arr[1] = 0;
						arr[2] = (char)99;
						arr[3] = 0;
						break;
					case 81:
						arr[0] = (char)248;
						arr[1] = 0;
						arr[2] = (char)69;
						arr[3] = 0;
						break;
					case 112:
						arr[0] = (char)210;
						arr[1] = 0;
						arr[2] = (char)126;
						arr[3] = 0;
						break;
					case 113:
						arr[0] = (char)0;
						arr[1] = 1;
						arr[2] = (char)70;
						arr[3] = 0;
						break;
					default:
						break;
				}
			}
			newMsgFile.write(arr, 4);
		}
	}

	txtFile.close();
	msgFile.close();
	newMsgFile.close();

}

void conv2text(char* fname){
	char* textfn = new char[strlen(fname)+4];
	strcpy(textfn, fname);
	strcat(textfn, ".txt");

	std::ifstream msgFile(fname, std::ios::binary);
	if(!msgFile.is_open()){
		printf("Unable to open file: %s\n", fname);
		exit(1);
	}
	std::ofstream txtFile(textfn, std::ios::binary);

	char arr[4];
	msgFile.read(reinterpret_cast<char*>(&arr), 4);
	if(equal(begin(arr), end(arr), begin({'M', 'S', 'G', '2'}))){
		msgFile.seekg(64);
		int n = 255;
		txtFile << (char)n;
		n = 254;
		txtFile << (char)n;
		while(n != -1){
			n = msgFile.get();
			msgFile.seekg(-1, std::ios::cur);
			if(n == -1) break;

			msgFile.read(reinterpret_cast<char*>(&arr), 4);

			if((arr[0] == 8) && arr[1] == 1){
				n = 164;
				txtFile << (char)n;
				n = 0;
				txtFile << (char)n;
			}

			if(arr[1] == 1){
				uint8_t b = (uint8_t)arr[0];
				switch (b) 
				{
					case 0:
						arr[0] = 0xFB;
						arr[1] = 0;
						break;
					case 2:
						arr[0] = 0xFC;
						arr[1] = 0;
						break;
					case 4:
						arr[0] = 0x52;
						arr[1] = 1;
						break;
					case 6:
						arr[0] = 0x53;
						arr[1] = 1;
						break;
					case 10:
						arr[0] = 0x3D;
						arr[1] = 0;
						break;
					case 16:
						arr[0] = 0xD9;
						arr[1] = 0;
						break;
					case 18:
						arr[0] = 0xBA;
						arr[1] = 0;
						break;
					case 20:
						arr[0] = 0xAA;
						arr[1] = 0;
						break;
					case 22:
						arr[0] = (char)37;
						arr[1] = 0;
						break;
					case 24:
						arr[0] = (char)124;
						arr[1] = 0;
						break;
					// case 32:
					// 	arr[0] = (char)112;
					// 	arr[1] = 1;
					// 	break;
					// case 34:
					// 	arr[0] = (char)113;
					// 	arr[1] = 1;
					// 	break;
					// case 36:
					// 	arr[0] = (char)80;
					// 	arr[1] = 1;
					// 	break;
					// case 38:
					// 	arr[0] = (char)81;
					// 	arr[1] = 1;
					// 	break;
					case 0x20:
						arr[0] = 0x10;
						arr[1] = 0x04;
						break;
					case 0x22:
						arr[0] = 0x11;
						arr[1] = 0x04;
						break;
					case 0x24:
						arr[0] = 0x12;
						arr[1] = 0x04;
						break;
					case 0x26:
						arr[0] = 0x13;
						arr[1] = 0x04;
						break;
					case 0x28:
						arr[0] = 0x90;
						arr[1] = 0x04;
						break;
					case 0x2a:
						arr[0] = 0x14;
						arr[1] = 0x04;
						break;
					case 0x2c:
						arr[0] = 0x15;
						arr[1] = 0x04;
						break;
					case 0x2e:
						arr[0] = 0x04;
						arr[1] = 0x04;
						break;
					case 0x30:
						arr[0] = 0x16;
						arr[1] = 0x04;
						break;
					case 0x32:
						arr[0] = 0x17;
						arr[1] = 0x04;
						break;
					case 0x34:
						arr[0] = 0x18;
						arr[1] = 0x04;
						break;
					case 0x36:
						arr[0] = 0x06;
						arr[1] = 0x04;
						break;
					case 0x38:
						arr[0] = 0x07;
						arr[1] = 0x04;
						break;
					case 0x3a:
						arr[0] = 0x19;
						arr[1] = 0x04;
						break;
					case 0x3c:
						arr[0] = 0x1a;
						arr[1] = 0x04;
						break;
					case 0x3e:
						arr[0] = 0x1b;
						arr[1] = 0x04;
						break;
					case 0x40:
						arr[0] = 0x1c;
						arr[1] = 0x04;
						break;
					case 0x42:
						arr[0] = 0x1d;
						arr[1] = 0x04;
						break;
					case 0x44:
						arr[0] = 0x1e;
						arr[1] = 0x04;
						break;
					case 0x46:
						arr[0] = 0x1f;
						arr[1] = 0x04;
						break;
					case 0x48:
						arr[0] = 0x20;
						arr[1] = 0x04;
						break;
					case 0x4a:
						arr[0] = 0x21;
						arr[1] = 0x04;
						break;
					case 0x4c:
						arr[0] = 0x22;
						arr[1] = 0x04;
						break;
					case 0x4e:
						arr[0] = 0x23;
						arr[1] = 0x04;
						break;
					case 0x50:
						arr[0] = 0x24;
						arr[1] = 0x04;
						break;
					case 0x52:
						arr[0] = 0x25;
						arr[1] = 0x04;
						break;
					case 0x54:
						arr[0] = 0x26;
						arr[1] = 0x04;
						break;
					case 0x56:
						arr[0] = 0x27;
						arr[1] = 0x04;
						break;
					case 0x58:
						arr[0] = 0x28;
						arr[1] = 0x04;
						break;
					case 0x5a:
						arr[0] = 0x29;
						arr[1] = 0x04;
						break;
					case 0x5c:
						arr[0] = 0x2c;
						arr[1] = 0x04;
						break;
					case 0x5e:
						arr[0] = 0x2e;
						arr[1] = 0x04;
						break;
					case 0x60:
						arr[0] = 0x2f;
						arr[1] = 0x04;
						break;
					case 0x62:
						arr[0] = 0x30;
						arr[1] = 0x04;
						break;
					case 0x64:
						arr[0] = 0x31;
						arr[1] = 0x04;
						break;
					case 0x66:
						arr[0] = 0x32;
						arr[1] = 0x04;
						break;
					case 0x68:
						arr[0] = 0x33;
						arr[1] = 0x04;
						break;
					case 0x6a:
						arr[0] = 0x91;
						arr[1] = 0x04;
						break;
					case 0x6c:
						arr[0] = 0x34;
						arr[1] = 0x04;
						break;
					case 0x6e:
						arr[0] = 0x35;
						arr[1] = 0x04;
						break;
					case 0x70:
						arr[0] = 0x54;
						arr[1] = 0x04;
						break;
					case 0x72:
						arr[0] = 0x36;
						arr[1] = 0x04;
						break;
					case 0x74:
						arr[0] = 0x37;
						arr[1] = 0x04;
						break;
					case 0x76:
						arr[0] = 0x38;
						arr[1] = 0x04;
						break;
					case 0x78:
						arr[0] = 0x56;
						arr[1] = 0x04;
						break;
					case 0x7a:
						arr[0] = 0x57;
						arr[1] = 0x04;
						break;
					case 0x7c:
						arr[0] = 0x39;
						arr[1] = 0x04;
						break;
					case 0x7e:
						arr[0] = 0x3a;
						arr[1] = 0x04;
						break;
					case 0x80:
						arr[0] = 0x3b;
						arr[1] = 0x04;
						break;
					case 0x82:
						arr[0] = 0x3c;
						arr[1] = 0x04;
						break;
					case 0x84:
						arr[0] = 0x3d;
						arr[1] = 0x04;
						break;
					case 0x86:
						arr[0] = 0x3e;
						arr[1] = 0x04;
						break;
					case 0x88:
						arr[0] = 0x3f;
						arr[1] = 0x04;
						break;
					case 0x8a:
						arr[0] = 0x40;
						arr[1] = 0x04;
						break;
					case 0x8c:
						arr[0] = 0x41;
						arr[1] = 0x04;
						break;
					case 0x8e:
						arr[0] = 0x42;
						arr[1] = 0x04;
						break;
					case 0x90:
						arr[0] = 0x43;
						arr[1] = 0x04;
						break;
					case 0x92:
						arr[0] = 0x44;
						arr[1] = 0x04;
						break;
					case 0x94:
						arr[0] = 0x45;
						arr[1] = 0x04;
						break;
					case 0x96:
						arr[0] = 0x46;
						arr[1] = 0x04;
						break;
					case 0x98:
						arr[0] = 0x47;
						arr[1] = 0x04;
						break;
					case 0x9a:
						arr[0] = 0x48;
						arr[1] = 0x04;
						break;
					case 0x9c:
						arr[0] = 0x49;
						arr[1] = 0x04;
						break;
					case 0x9e:
						arr[0] = 0x4c;
						arr[1] = 0x04;
						break;
					case 0xa0:
						arr[0] = 0x4e;
						arr[1] = 0x04;
						break;
					case 0xa2:
						arr[0] = 0x4f;
						arr[1] = 0x04;
						break;	
					default:
						break;	
				}
				txtFile << arr[0] << arr[1];
			}else{
				uint8_t bb = (uint8_t)arr[0];
				switch (bb)
				{
					case 0:
						if ((arr[2] == (char)16) & (arr[3] == (char)4))
						{
							n = 13;
							txtFile << (char)n;
							n = 0;
							txtFile << (char)n;
							n = 10;
							txtFile << (char)n;
							n = 0;
							txtFile << (char)n;
						}
						else if ((arr[2] == (char)3) & (arr[3] == (char)4))
						{
							arr[0] = (char)124;
							arr[1] = 0;
						}
						else if ((arr[0] == 0) & (arr[1] == 0) & (arr[2] != (char)77))
						{
							n = 164;
							txtFile << (char)n;
							n = 0;
							txtFile << (char)n;
						}
						else
						{
							arr[0] = (char)48;
							arr[1] = 0;
						}
						break;
					case 2:
						arr[0] = (char)49;
						arr[1] = 0;
						break;
					case 4:
						arr[0] = (char)50;
						arr[1] = 0;
						break;
					case 6:
						arr[0] = (char)51;
						arr[1] = 0;
						break;
					case 8:
						arr[0] = (char)52;
						arr[1] = 0;
						break;
					case 10:
						arr[0] = (char)53;
						arr[1] = 0;
						break;
					case 12:
						arr[0] = (char)54;
						arr[1] = 0;
						break;
					case 14:
						arr[0] = (char)55;
						arr[1] = 0;
						break;
					case 16:
						arr[0] = (char)56;
						arr[1] = 0;
						break;
					case 18:
						arr[0] = (char)57;
						arr[1] = 0;
						break;
					case 20:
						arr[0] = (char)33;
						arr[1] = 0;
						break;
					case 22:
						arr[0] = (char)63;
						arr[1] = 0;
						break;
					case 24:
						arr[0] = (char)40;
						arr[1] = 0;
						break;
					case 26:
						arr[0] = (char)41;
						arr[1] = 0;
						break;
					case 28:
						arr[0] = (char)32;
						arr[1] = 0;
						break;
					case 30:
						arr[0] = (char)38;
						arr[1] = 0;
						break;
					case 32:
						arr[0] = (char)58;
						arr[1] = 0;
						break;
					case 34:
						arr[0] = (char)59;
						arr[1] = 0;
						break;
					case 36:
						arr[0] = (char)44;
						arr[1] = 0;
						break;
					case 38:
						arr[0] = (char)46;
						arr[1] = 0;
						break;
					case 40:
						arr[0] = (char)34;
						arr[1] = 0;
						break;
					case 42:
						arr[0] = (char)39;
						arr[1] = 0;
						break;
					case 44:
						arr[0] = (char)126;
						arr[1] = 0;
						break;
					case 46:
						arr[0] = (char)45;
						arr[1] = 0;
						break;
					case 48:
						arr[0] = (char)43;
						arr[1] = 0;
						break;
					case 50:
						arr[0] = (char)47;
						arr[1] = 0;
						break;
					case 52:
						arr[0] = (char)64;
						arr[1] = 0;
						break;
					case 54:
						arr[0] = (char)36;
						arr[1] = 0;
						break;
					case 56:
						arr[0] = (char)65;
						arr[1] = 0;
						break;
					case 58:
						arr[0] = (char)66;
						arr[1] = 0;
						break;
					case 60:
						arr[0] = (char)67;
						arr[1] = 0;
						break;
					case 62:
						arr[0] = (char)68;
						arr[1] = 0;
						break;
					case 64:
						arr[0] = (char)69;
						arr[1] = 0;
						break;
					case 66:
						arr[0] = (char)70;
						arr[1] = 0;
						break;
					case 68:
						arr[0] = (char)71;
						arr[1] = 0;
						break;
					case 70:
						arr[0] = (char)72;
						arr[1] = 0;
						break;
					case 72:
						arr[0] = (char)73;
						arr[1] = 0;
						break;
					case 74:
						arr[0] = (char)74;
						arr[1] = 0;
						break;
					case 76:
						arr[0] = (char)75;
						arr[1] = 0;
						break;
					case 78:
						arr[0] = (char)76;
						arr[1] = 0;
						break;
					case 80:
						arr[0] = (char)77;
						arr[1] = 0;
						break;
					case 82:
						arr[0] = (char)78;
						arr[1] = 0;
						break;
					case 84:
						arr[0] = (char)79;
						arr[1] = 0;
						break;
					case 86:
						arr[0] = (char)80;
						arr[1] = 0;
						break;
					case 88:
						arr[0] = (char)81;
						arr[1] = 0;
						break;
					case 90:
						arr[0] = (char)82;
						arr[1] = 0;
						break;
					case 92:
						arr[0] = (char)83;
						arr[1] = 0;
						break;
					case 94:
						arr[0] = (char)84;
						arr[1] = 0;
						break;
					case 96:
						arr[0] = (char)85;
						arr[1] = 0;
						break;
					case 98:
						arr[0] = (char)86;
						arr[1] = 0;
						break;
					case 100:
						arr[0] = (char)87;
						arr[1] = 0;
						break;
					case 102:
						arr[0] = (char)88;
						arr[1] = 0;
						break;
					case 104:
						arr[0] = (char)89;
						arr[1] = 0;
						break;
					case 106:
						arr[0] = (char)96;
						arr[1] = 0;
						break;
					case 108:
						arr[0] = (char)97;
						arr[1] = 0;
						break;
					case 110:
						arr[0] = (char)98;
						arr[1] = 0;
						break;
					case 112:
						arr[0] = (char)99;
						arr[1] = 0;
						break;
					case 114:
						arr[0] = (char)100;
						arr[1] = 0;
						break;
					case 116:
						arr[0] = (char)101;
						arr[1] = 0;
						break;
					case 118:
						arr[0] = (char)102;
						arr[1] = 0;
						break;
					case 120:
						arr[0] = (char)103;
						arr[1] = 0;
						break;
					case 122:
						arr[0] = (char)104;
						arr[1] = 0;
						break;
					case 124:
						arr[0] = (char)105;
						arr[1] = 0;
						break;
					case 126:
						arr[0] = (char)106;
						arr[1] = 0;
						break;
					case 128:
						arr[0] = (char)107;
						arr[1] = 0;
						break;
					case 130:
						arr[0] = (char)108;
						arr[1] = 0;
						break;
					case 132:
						arr[0] = (char)109;
						arr[1] = 0;
						break;
					case 134:
						arr[0] = (char)110;
						arr[1] = 0;
						break;
					case 136:
						arr[0] = (char)111;
						arr[1] = 0;
						break;
					case 138:
						arr[0] = (char)112;
						arr[1] = 0;
						break;
					case 140:
						arr[0] = (char)113;
						arr[1] = 0;
						break;
					case 142:
						arr[0] = (char)114;
						arr[1] = 0;
						break;
					case 144:
						arr[0] = (char)115;
						arr[1] = 0;
						break;
					case 146:
						arr[0] = (char)116;
						arr[1] = 0;
						break;
					case 148:
						arr[0] = (char)117;
						arr[1] = 0;
						break;
					case 150:
						arr[0] = (char)118;
						arr[1] = 0;
						break;
					case 152:
						arr[0] = (char)119;
						arr[1] = 0;
						break;
					case 154:
						arr[0] = (char)120;
						arr[1] = 0;
						break;
					case 156:
						arr[0] = (char)121;
						arr[1] = 0;
						break;
					case 158:
						arr[0] = (char)122;
						arr[1] = 0;
						break;
					case 160:
						arr[0] = (char)91;
						arr[1] = 0;
						break;
					case 162:
						arr[0] = (char)93;
						arr[1] = 0;
						break;
					case 164:
						arr[0] = (char)161;
						arr[1] = 0;
						break;
					case 166:
						arr[0] = (char)191;
						arr[1] = 0;
						break;
					case 168:
						arr[0] = (char)174;
						arr[1] = 0;
						break;
					case 170:
						arr[0] = (char)176;
						arr[1] = 0;
						break;
					case 172:
						arr[0] = (char)192;
						arr[1] = 0;
						break;
					case 174:
						arr[0] = (char)193;
						arr[1] = 0;
						break;
					case 176:
						arr[0] = (char)194;
						arr[1] = 0;
						break;
					case 178:
						arr[0] = (char)195;
						arr[1] = 0;
						break;
					case 180:
						arr[0] = (char)199;
						arr[1] = 0;
						break;
					case 182:
						arr[0] = (char)200;
						arr[1] = 0;
						break;
					case 184:
						arr[0] = (char)201;
						arr[1] = 0;
						break;
					case 186:
						arr[0] = (char)202;
						arr[1] = 0;
						break;
					case 188:
						arr[0] = (char)203;
						arr[1] = 0;
						break;
					case 190:
						arr[0] = (char)204;
						arr[1] = 0;
						break;
					case 192:
						arr[0] = (char)205;
						arr[1] = 0;
						break;
					case 194:
						arr[0] = (char)206;
						arr[1] = 0;
						break;
					case 196:
						arr[0] = (char)207;
						arr[1] = 0;
						break;
					case 198:
						arr[0] = (char)209;
						arr[1] = 0;
						break;
					case 200:
						arr[0] = (char)210;
						arr[1] = 0;
						break;
					case 202:
						arr[0] = (char)211;
						arr[1] = 0;
						break;
					case 204:
						arr[0] = (char)212;
						arr[1] = 0;
						break;
					case 206:
						arr[0] = (char)214;
						arr[1] = 0;
						break;
					case 208:
						arr[0] = (char)218;
						arr[1] = 0;
						break;
					case 210:
						arr[0] = (char)219;
						arr[1] = 0;
						break;
					case 212:
						arr[0] = (char)220;
						arr[1] = 0;
						break;
					case 214:
						arr[0] = (char)223;
						arr[1] = 0;
						break;
					case 216:
						arr[0] = (char)224;
						arr[1] = 0;
						break;
					case 218:
						arr[0] = (char)225;
						arr[1] = 0;
						break;
					case 220:
						arr[0] = (char)226;
						arr[1] = 0;
						break;
					case 222:
						arr[0] = (char)228;
						arr[1] = 0;
						break;
					case 224:
						arr[0] = (char)231;
						arr[1] = 0;
						break;
					case 226:
						arr[0] = (char)232;
						arr[1] = 0;
						break;
					case 228:
						arr[0] = (char)233;
						arr[1] = 0;
						break;
					case 230:
						arr[0] = (char)234;
						arr[1] = 0;
						break;
					case 232:
						arr[0] = (char)235;
						arr[1] = 0;
						break;
					case 234:
						arr[0] = (char)236;
						arr[1] = 0;
						break;
					case 236:
						arr[0] = (char)237;
						arr[1] = 0;
						break;
					case 238:
						arr[0] = (char)238;
						arr[1] = 0;
						break;
					case 240:
						arr[0] = (char)239;
						arr[1] = 0;
						break;
					case 242:
						arr[0] = (char)241;
						arr[1] = 0;
						break;
					case 244:
						arr[0] = (char)242;
						arr[1] = 0;
						break;
					case 246:
						arr[0] = (char)243;
						arr[1] = 0;
						break;
					case 248:
						arr[0] = (char)244;
						arr[1] = 0;
						break;
					case 250:
						arr[0] = (char)246;
						arr[1] = 0;
						break;
					case 252:
						arr[0] = (char)249;
						arr[1] = 0;
						break;
					case 254:
						arr[0] = (char)250;
						arr[1] = 0;
						break;
				}
				if(arr[0] != 0)
				{
					if(arr[0] == 1)
					{
						n = 164;
						txtFile << (char)n;
						n = 0;
						txtFile << (char)n;
					}else{
						txtFile << arr[0] << arr[1];
					}
				}
			}
		}
	}else{
		printf("File '%s' if not MSG2\n", fname);
		exit(1);
	}

	msgFile.close();
	txtFile.close();
}

void helpmsg(char* exec){
	printf("Usage: %s file.msg2 [option]\n\nOptions:\ne - extract\nr - repack\n", exec);
	exit(1);
}

int main(int argc, char** argv){
	if (argc != 3) helpmsg(argv[0]);
	char* fname = argv[1];

	switch (argv[2][0]) {
		case 'e':
			conv2text(fname);
			break;
		case 'r':
			conv2msg(fname);
			break;
		default:
			helpmsg(argv[0]);
			break;
	}	
	return 1;
}

