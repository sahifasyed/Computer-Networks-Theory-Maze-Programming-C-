#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "wsock32.lib")


#define STUDENT_NUMBER    "23051901"

#define IP_ADDRESS_SERVER "127.0.0.1"

#define PORT_SERVER 0x1984 // We define a port that we are going to use.
#define PORT_CLIENT 0x1985 // We define a port that we are going to use.

#define WORD  unsigned short
#define DWORD unsigned long
#define BYTE  unsigned char

#define MAX_FILENAME_SIZE 500
#define MAX_BUFFER_SIZE   500

SOCKADDR_IN server_addr;
SOCKADDR_IN client_addr;

SOCKET sock;  // This is our socket, it is the handle to the IO address to read/write packets

WSADATA data;

char InputBuffer [MAX_BUFFER_SIZE];

char hex_file [MAX_BUFFER_SIZE];
char trc_file [MAX_BUFFER_SIZE];

//////////////////////////
//   Registers          //
//////////////////////////

#define FLAG_Z  0x80
#define FLAG_I  0x20
#define FLAG_N  0x08
#define FLAG_C  0x01

#define REGISTER_M  5
#define REGISTER_A	4
#define REGISTER_H	3
#define REGISTER_L	2
#define REGISTER_C	1
#define REGISTER_B	0

#define REGISTER_X 0
#define REGISTER_Y 1
BYTE Index_Registers[2];

BYTE Registers[6];
BYTE Flags;
WORD ProgramCounter;
WORD StackPointer;


////////////
// Memory //
////////////

#define MEMORY_SIZE	65536

BYTE Memory[MEMORY_SIZE];

#define TEST_ADDRESS_1  0x01FA
#define TEST_ADDRESS_2  0x01FB
#define TEST_ADDRESS_3  0x01FC
#define TEST_ADDRESS_4  0x01FD
#define TEST_ADDRESS_5  0x01FE
#define TEST_ADDRESS_6  0x01FF
#define TEST_ADDRESS_7  0x0200
#define TEST_ADDRESS_8  0x0201
#define TEST_ADDRESS_9  0x0202
#define TEST_ADDRESS_10  0x0203
#define TEST_ADDRESS_11  0x0204
#define TEST_ADDRESS_12  0x0205


///////////////////////
// Control variables //
///////////////////////

bool memory_in_range = true;
bool halt = false;


///////////////////////
// Disassembly table //
///////////////////////

char opcode_mneumonics[][14] =
{
"ILLEGAL     ", 
"ILLEGAL     ", 
"SWI impl     ", 
"RTI impl     ", 
"STO abs      ", 
"STOX abs     ", 
"STOY abs     ", 
"JMPR abs     ", 
"CCC abs      ", 
"CCS abs      ", 
"CNE abs      ", 
"CEQ abs      ", 
"CMI abs      ", 
"CPL abs      ", 
"ILLEGAL     ", 
"STOS abs     ", 

"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"STO abs,X    ", 
"STOX abs,X   ", 
"STOY abs,X   ", 
"NOP impl     ", 
"WAI impl     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ADI  #       ", 
"CPI  #       ", 
"ANI  #       ", 
"STOS abs,X   ", 

"LODS  #      ", 
"LDX  #       ", 
"LODY  #      ", 
"RT impl      ", 
"STO abs,Y    ", 
"STOX abs,Y   ", 
"STOY abs,Y   ", 
"MVR  #,B     ", 
"MVR  #,C     ", 
"MVR  #,L     ", 
"MVR  #,H     ", 
"TAY impl     ", 
"TYA impl     ", 
"MSA impl     ", 
"ILLEGAL     ", 
"STOS abs,Y   ", 

"LODS abs     ", 
"LDX abs      ", 
"LODY abs     ", 
"ILLEGAL     ", 
"STO abs,XY   ", 
"STOX abs,XY  ", 
"STOY abs,XY  ", 
"ILLEGAL     ", 
"JUMP abs     ", 
"JCC abs      ", 
"JCS abs      ", 
"JNE abs      ", 
"JEQ abs      ", 
"JMI abs      ", 
"JPL abs      ", 
"STOS abs,XY  ", 

"LODS abs,X   ", 
"LDX abs,X    ", 
"LODY abs,X   ", 
"LD  #        ", 
"STO zpg      ", 
"STOX zpg     ", 
"STOY zpg     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"DEX impl     ", 
"INX impl     ", 
"DEY impl     ", 
"INY impl     ", 
"ILLEGAL     ", 
"STOS zpg     ", 

"LODS abs,Y   ", 
"LDX abs,Y    ", 
"LODY abs,Y   ", 
"LD abs       ", 
"TEST abs     ", 
"INC abs      ", 
"DEC abs      ", 
"RR abs       ", 
"RCL abs      ", 
"SAL abs      ", 
"SHR abs      ", 
"COM abs      ", 
"NEG abs      ", 
"RAL abs      ", 
"ROR abs      ", 
"CLR abs      ", 

"LODS abs,XY  ", 
"LDX abs,XY   ", 
"LODY abs,XY  ", 
"LD abs,X     ", 
"TEST abs,X   ", 
"INC abs,X    ", 
"DEC abs,X    ", 
"RR abs,X     ", 
"RCL abs,X    ", 
"SAL abs,X    ", 
"SHR abs,X    ", 
"COM abs,X    ", 
"NEG abs,X    ", 
"RAL abs,X    ", 
"ROR abs,X    ", 
"CLR abs,X    ", 

"LODS zpg     ", 
"LDX zpg      ", 
"LODY zpg     ", 
"LD abs,Y     ", 
"TEST abs,Y   ", 
"INC abs,Y    ", 
"DEC abs,Y    ", 
"RR abs,Y     ", 
"RCL abs,Y    ", 
"SAL abs,Y    ", 
"SHR abs,Y    ", 
"COM abs,Y    ", 
"NEG abs,Y    ", 
"RAL abs,Y    ", 
"ROR abs,Y    ", 
"CLR abs,Y    ", 

"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"LD abs,XY    ", 
"TEST abs,XY  ", 
"INC abs,XY   ", 
"DEC abs,XY   ", 
"RR abs,XY    ", 
"RCL abs,XY   ", 
"SAL abs,XY   ", 
"SHR abs,XY   ", 
"COM abs,XY   ", 
"NEG abs,XY   ", 
"RAL abs,XY   ", 
"ROR abs,XY   ", 
"CLR abs,XY   ", 

"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"LD zpg       ", 
"TESTA A,A    ", 
"INCA A,A     ", 
"DECA A,A     ", 
"RRA A,A      ", 
"RCLA A,A     ", 
"SALA A,A     ", 
"SHRA A,A     ", 
"COMA A,A     ", 
"NEGA A,0     ", 
"RALA A,A     ", 
"RORA A,A     ", 
"CLRA A,0     ", 

"MV A,A       ", 
"MV B,A       ", 
"MV C,A       ", 
"MV L,A       ", 
"MV H,A       ", 
"MV M,A       ", 
"CLC impl     ", 
"SEC impl     ", 
"CLI impl     ", 
"SEI impl     ", 
"CMC impl     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"PUSH  ,A     ", 
"POP A,       ", 

"MV A,B       ", 
"MV B,B       ", 
"MV C,B       ", 
"MV L,B       ", 
"MV H,B       ", 
"MV M,B       ", 
"ADC A,B      ", 
"SBC A,B      ", 
"ADD A,B      ", 
"SUB A,B      ", 
"CMP A,B      ", 
"OR A,B       ", 
"AND A,B      ", 
"XOR A,B      ", 
"PUSH  ,s     ", 
"POP s,       ", 

"MV A,C       ", 
"MV B,C       ", 
"MV C,C       ", 
"MV L,C       ", 
"MV H,C       ", 
"MV M,C       ", 
"ADC A,C      ", 
"SBC A,C      ", 
"ADD A,C      ", 
"SUB A,C      ", 
"CMP A,C      ", 
"OR A,C       ", 
"AND A,C      ", 
"XOR A,C      ", 
"PUSH  ,B     ", 
"POP B,       ", 

"MV A,L       ", 
"MV B,L       ", 
"MV C,L       ", 
"MV L,L       ", 
"MV H,L       ", 
"MV M,L       ", 
"ADC A,L      ", 
"SBC A,L      ", 
"ADD A,L      ", 
"SUB A,L      ", 
"CMP A,L      ", 
"OR A,L       ", 
"AND A,L      ", 
"XOR A,L      ", 
"PUSH  ,C     ", 
"POP C,       ", 

"MV A,H       ", 
"MV B,H       ", 
"MV C,H       ", 
"MV L,H       ", 
"MV H,H       ", 
"MV M,H       ", 
"ADC A,H      ", 
"SBC A,H      ", 
"ADD A,H      ", 
"SUB A,H      ", 
"CMP A,H      ", 
"OR A,H       ", 
"AND A,H      ", 
"XOR A,H      ", 
"PUSH  ,L     ", 
"POP L,       ", 

"MV A,M       ", 
"MV B,M       ", 
"MV C,M       ", 
"MV L,M       ", 
"MV H,M       ", 
"MV -,-       ", 
"ADC A,M      ", 
"SBC A,M      ", 
"ADD A,M      ", 
"SUB A,M      ", 
"CMP A,M      ", 
"OR A,M       ", 
"AND A,M      ", 
"XOR A,M      ", 
"PUSH  ,H     ", 
"POP H,       ", 

}; 

////////////////////////////////////////////////////////////////////////////////
//                           Simulator/Emulator (Start)                       //
////////////////////////////////////////////////////////////////////////////////

BYTE fetch()
{
	BYTE byte = 0;

	if ((ProgramCounter >= 0) && (ProgramCounter <= MEMORY_SIZE))
	{
		memory_in_range = true;
		byte = Memory[ProgramCounter];
		ProgramCounter++;
	}
	else
	{
		memory_in_range = false;
	}
	return byte;
}

void push(BYTE reg) {

    Memory[StackPointer] = reg;

    StackPointer--;

}

void set_flag_n(BYTE inReg) 
{
	BYTE reg; 
	reg = inReg; 

	if ((reg & 0x80) != 0) // msbit set 
	{ 
		Flags = Flags | FLAG_N;
	}
	else 
	{ 
		Flags = Flags & (0xFF - FLAG_N);
	}
}

void set_flag_n(WORD inReg)

{
    WORD reg;
    reg = inReg;

    if ((reg & 0x8000) != 0) // msbit set

    {
        Flags = Flags | FLAG_N;

    }

    else

    {
        Flags = Flags & (0xFF - FLAG_N);

    }

}

void set_flag_z(BYTE inReg) {
    BYTE reg;
    reg = inReg;
    if (reg == 0) // zero set
    {
        Flags = Flags | FLAG_Z;
    }
    else
    {
        Flags = Flags & (0xFF - FLAG_Z);
    }
}

void set_flag_Z(WORD inReg)

{
    WORD reg;
    reg = inReg;

    if (reg == 0) // zero

    {
        Flags = Flags | FLAG_Z;

    }

    else

    {
        Flags = Flags & (0xFF - FLAG_Z);

    }

}


void Group_1(BYTE opcode)
{
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	WORD data = 0;
    WORD temp_word;
    BYTE saved_flags;


	switch(opcode) 
	{
    case 0x43: //LD \#
        data = fetch();
        Registers[REGISTER_A] = data;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x53: //LD abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE) {
            Registers[REGISTER_A] = Memory[address];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x63: //LD abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE) {
            Registers[REGISTER_A] = Memory[address];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x73: //LD abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE) {
            Registers[REGISTER_A] = Memory[address];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x83: //LD abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8)
            + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE) {
            Registers[REGISTER_A] = Memory[address];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x93: //LD zpg
        address += 0x0000 | (WORD)fetch();
        if (address >= 0 && address < MEMORY_SIZE) {
            Registers[REGISTER_A] = Memory[address];
        }
        set_flag_n((BYTE)Memory);
        set_flag_z((BYTE)Memory);
        break;

    case 0x04: //STO abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE) {
            Memory[address] = Registers[REGISTER_A];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x14: //STO abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE) {
            Memory[address] = Registers[REGISTER_A];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x24: //STO abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE) {
            Memory[address] = Registers[REGISTER_A];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x34: //STO abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8)
            + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE) {
            Memory[address] = Registers[REGISTER_A];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x44: //STO zpg
        address += 0x0000 | (WORD)fetch();
        if (address >= 0 && address < MEMORY_SIZE) {
            Memory[address] = Registers[REGISTER_A];
        }
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x27: //MVRB
        data = fetch();
        Registers[REGISTER_B] = data;
        set_flag_Z(Registers[REGISTER_B]);
        set_flag_n(Registers[REGISTER_B]);
        break;

    case 0x28: //MVRC
        data = fetch();
        Registers[REGISTER_C] = data;
        set_flag_Z(Registers[REGISTER_C]);
        set_flag_n(Registers[REGISTER_C]);
        break;

    case 0x29: //MVRL
        data = fetch();
        Registers[REGISTER_L] = data;
        set_flag_Z(Registers[REGISTER_L]);
        set_flag_n(Registers[REGISTER_L]);
        break;

    case 0x2A: //MVRH
        data = fetch();
        Registers[REGISTER_H] = data;
        set_flag_Z(Registers[REGISTER_H]);
        set_flag_n(Registers[REGISTER_H]);
        break;

    case 0x20: //LODS \#
        data = fetch();
        StackPointer = data; StackPointer += (WORD)fetch() << 8;
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x30: //LODS abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            StackPointer = Memory[address];
            StackPointer += (WORD)Memory[address + 1] << 8;
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x40: //LODS abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            StackPointer = Memory[address];
            StackPointer += (WORD)Memory[address + 1] << 8;
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x50: //LODS abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            StackPointer = Memory[address];
            StackPointer += (WORD)Memory[address + 1] << 8;
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x60: //LODS abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            StackPointer = Memory[address];
            StackPointer += (WORD)Memory[address + 1] << 8;
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x70: //LODS zpg
        address += 0x0000 | (WORD)fetch();
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            StackPointer = Memory[address];
            StackPointer += (WORD)Memory[address + 1] << 8;
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0xB6: //ADC A,B
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_B];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word++;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xC6: // ADC A,C
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_C];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word++;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xD6: //ADC A,L
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_L];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word++;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xE6: // ADC A,H
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_H];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word++;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xF6: //ADC A,M
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Registers[REGISTER_M] = Memory[address];
        }
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_M];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word++;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xB8: //ADD A,B
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_B];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xC8: //ADD A,C
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_C];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xD8: //ADD A,L
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_L];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xE8: //ADD A,H
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_H];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xF8: //ADD A,M
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Registers[REGISTER_M] = Memory[address];
        }
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_M];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xB9: //SUB A,B
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_B];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xC9: //SUB A,C
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_C];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xD9: //SUB A,L
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_L];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xE9: //SUB A,H
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_H];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xF9: //SUB A,M
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Registers[REGISTER_M] = Memory[address];
        }
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_M];
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x21:   //LDX \#
        data = fetch();
        Index_Registers[REGISTER_X] = data;
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x31:   //LDX abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_X] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x41:   //LDX abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_X] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x51:   //LDX abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_X] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x61: //LDX abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_X] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x71: //LDX zpg
        address += 0x0000 | (WORD)fetch();
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_X] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x05:   //STOX abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_X];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x15:   //STOX abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_X];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x25:   //STOX abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_X];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x35:   //STOX abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_X];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x45: //STOX zpg
        address += 0x0000 | (WORD)fetch();
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_X];
        }
        set_flag_Z(Index_Registers[REGISTER_X]);
        set_flag_n(Index_Registers[REGISTER_X]);
        break;

    case 0x22:   //LODY \#
        data = fetch();
        Registers[REGISTER_Y] = data;
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x32:   //LODY abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_Y] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x42:   //LODY abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_Y] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x52:   //LODY abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_Y] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x62: //LODY abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_Y] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x72: //LODY zpg
        address += 0x0000 | (WORD)fetch();
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Index_Registers[REGISTER_Y] = Memory[address];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x06:   //STOY abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_Y];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x16:   //STOY abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_Y];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x26:   //STOY abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_Y];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x36:   //STOY abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_Y];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x46: //STOY zpg
        address += 0x0000 | (WORD)fetch();
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Memory[address] = Index_Registers[REGISTER_Y];
        }
        set_flag_Z(Index_Registers[REGISTER_Y]);
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x0F: //STOS abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            Memory[address] = (BYTE)StackPointer;
            Memory[address + 1] = (BYTE)(StackPointer >> 8);
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x1F: //STOS abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            Memory[address] = (BYTE)(StackPointer);
            Memory[address + 1] = (BYTE)(StackPointer >> 8);
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x2F: //STOS abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            Memory[address] = (BYTE)(StackPointer);
            Memory[address + 1] = (BYTE)(StackPointer >> 8);
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x3F: //STOS abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            Memory[address] = (BYTE)(StackPointer);
            Memory[address + 1] = (BYTE)(StackPointer >> 8);
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x4F: //STOS zpg
        address += 0x0000 | (WORD)fetch();
        if (address >= 0 && address < MEMORY_SIZE - 1)
        {
            Memory[address] = (BYTE)(StackPointer);
            Memory[address + 1] = (BYTE)(StackPointer >> 8);
        }
        set_flag_Z(StackPointer);
        set_flag_n(StackPointer);
        break;

    case 0x2B: // TAY impl
        Index_Registers[REGISTER_Y] = Registers[REGISTER_A];
        set_flag_n(Index_Registers[REGISTER_Y]);
        break;

    case 0x2C: // TYA impl
        Registers[REGISTER_A] = Index_Registers[REGISTER_Y];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x2D: // MSA impl
        Registers[REGISTER_A] = Flags;
        break;

    case 0xA6: // CLC impl
        Flags = Flags & (0xFF - FLAG_C);
        break;

    case 0xA7: // SEC impl
        Flags = Flags | FLAG_C;
        break;

    case 0xA8: // CLI impl
        Flags = Flags & (0xFF - FLAG_I);
        break;

    case 0xA9: // SEI impl
        Flags = Flags | FLAG_I;
        break;

    case 0xAA: //CMC impl
        Flags = Flags ^ 0xFF;
        break;

    case 0xBA: //CMP A,B
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_B];
        if (temp_word >= 0x100) {
            Flags = Flags | FLAG_C; // set carry flag
        }
        else {
            Flags = Flags & (0xFF - FLAG_C); // clear carry flag
        }
        set_flag_n((BYTE)temp_word);
        set_flag_Z((BYTE)temp_word);
        break;

    case 0xCA: //CMP A,C
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_C];
        if (temp_word >= 0x100) {
            Flags = Flags | FLAG_C; // set carry flag
        }
        else {
            Flags = Flags & (0xFF - FLAG_C); // clear carry flag
        }
        set_flag_n((BYTE)temp_word);
        set_flag_Z((BYTE)temp_word);
        break;

    case 0xDA: //CMP A,L
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_L];
        if (temp_word >= 0x100) {
            Flags = Flags | FLAG_C; // set carry flag
        }
        else {
            Flags = Flags & (0xFF - FLAG_C); // clear carry flag
        }
        set_flag_n((BYTE)temp_word);
        set_flag_Z((BYTE)temp_word);
        break;

    case 0xEA: //CMP A,H
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_H];
        if (temp_word >= 0x100) {
            Flags = Flags | FLAG_C; // set carry flag
        }
        else {
            Flags = Flags & (0xFF - FLAG_C); // clear carry flag
        }
        set_flag_n((BYTE)temp_word);
        set_flag_Z((BYTE)temp_word);
        break;

    case 0xFA: //CMP A,M
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Registers[REGISTER_M] = Memory[address];
        }
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_M];
        if (temp_word >= 0x100) {
            Flags = Flags | FLAG_C; // set carry flag
        }
        else {
            Flags = Flags & (0xFF - FLAG_C); // clear carry flag
        }
        set_flag_n((BYTE)temp_word);
        set_flag_Z((BYTE)temp_word);
        break;

    case 0xAE: //PUSH A
        if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)) {
            StackPointer--;
            Memory[StackPointer] = Registers[REGISTER_A];
        }
        break;

    case 0xBE: //PUSH FL
        if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)) {
            StackPointer--;
            Memory[StackPointer] = Flags;
        }
        break;

    case 0xCE: //PUSH B
        if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)) {
            StackPointer--;
            Memory[StackPointer] = Registers[REGISTER_B];
        }
        break;

    case 0xDE: //PUSH C
        if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)) {
            StackPointer--;
            Memory[StackPointer] = Registers[REGISTER_C];
        }
        break;

    case 0xEE: //PUSH L
        if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)) {
            StackPointer--;
            Memory[StackPointer] = Registers[REGISTER_L];
        }
        break;

    case 0xFE: //PUSH H
        if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)) {
            StackPointer--;
            Memory[StackPointer] = Registers[REGISTER_H];
        }
        break;

    case 0xAF: // POP A
        if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
            Registers[REGISTER_A] = Memory[StackPointer];
            StackPointer++;
        }
        break;

    case 0xBF: // POP FL
        if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
            Flags = Memory[StackPointer];
            StackPointer++;
        }
        break;

    case 0xCF: // POP B
        if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
            Registers[REGISTER_B] = Memory[StackPointer];
            StackPointer++;
        }
        break;

    case 0xDF: // POP C
        if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
            Registers[REGISTER_C] = Memory[StackPointer];
            StackPointer++;
        }
        break;

    case 0xEF: // POP L
        if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
            Registers[REGISTER_L] = Memory[StackPointer];
            StackPointer++;
        }
        break;

    case 0xFF: // POP H
        if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
            Registers[REGISTER_H] = Memory[StackPointer];
            StackPointer++;
        }
        break;

    case 0x38: // JUMP abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        ProgramCounter = address;
        break;

    case 0x07: // JMPR abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((StackPointer >= 2) && (StackPointer < MEMORY_SIZE)) {
            StackPointer--;
            Memory[StackPointer] = (BYTE)((ProgramCounter >> 8) & 0xFF);
            StackPointer--;
            Memory[StackPointer] = (BYTE)(ProgramCounter & 0xFF);
        }
        ProgramCounter = address;
        break;

    case 0x23: // RT impl
        if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 2)) {
            LB = Memory[StackPointer];
            StackPointer++;
            HB = Memory[StackPointer];
            StackPointer++;
            ProgramCounter = ((WORD)HB << 8) + (WORD)LB;
        }
        break;

    case 0x39: // JCC abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_C) == 0) {
            ProgramCounter = address;
        }
        break;

    case 0x3A: // JCS abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_C) == 1) {
            ProgramCounter = address;
        }
        break;

    case 0x3B: // JNE abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_Z) == 0) {
            ProgramCounter = address;
        }
        break;

    case 0x3C: // JEQ abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_Z) == 1) {
            ProgramCounter = address;
        }
        break;

    case 0x3D: // JMI abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_N) == 0) {
            ProgramCounter = address;
        }
        break;

    case 0x3E: // JPL abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_N) == 1) {
            ProgramCounter = address;
        }
        break;

    case 0x08: // CCC abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_C) == 0) {
            push(ProgramCounter);
            ProgramCounter = address;
        }
        break;

    case 0x09: // CCS abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_C) == 1) {
            push(ProgramCounter);
            ProgramCounter = address;
        }
        break;

    case 0x0A: // CNE abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_Z) == 0) {
            push(ProgramCounter);
            ProgramCounter = address;
        }
        break;

    case 0x0B: // CEQ abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_Z) == 1) {
            push(ProgramCounter);
            ProgramCounter = address;
        }
        break;

    case 0x0C: // CMI abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_N) == 0) {
            push(ProgramCounter);
            ProgramCounter = address;
        }
        break;

    case 0x0D: // CPL abs
        LB = fetch();
        HB = fetch();
        address = ((WORD)HB << 8) + (WORD)LB;
        if ((Flags & FLAG_N) == 1) {
            push(ProgramCounter);
            ProgramCounter = address;
        }
        break;

    case 0x95: //INCA A
        ++Registers[REGISTER_A];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x96: // DECA A
        --Registers[REGISTER_A];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x4B: // INX impl
        ++Index_Registers[REGISTER_X];
        set_flag_Z(Index_Registers[REGISTER_X]);
        break;

    case 0x4D: // INY impl
        ++Index_Registers[REGISTER_Y];
        set_flag_Z(Index_Registers[REGISTER_Y]);
        break;

    case 0x4A: // DEX impl
        --Index_Registers[REGISTER_X];
        set_flag_Z(Index_Registers[REGISTER_X]);
        break;

    case 0x4C: // DEY impl
        --Index_Registers[REGISTER_Y];
        set_flag_Z(Index_Registers[REGISTER_Y]);
        break;

    case 0x55: // INC abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        ++Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x65: // INC abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        ++Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x75: // INC abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        ++Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x85: // INC abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        ++Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x56: // DEC abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        --Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x66: // DEC abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        --Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x76: // DEC abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        --Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x86: // DEC abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        --Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0xBC: // AND A,B
        Registers[REGISTER_A] = Registers[REGISTER_A] & Registers[REGISTER_B];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xCC: // AND A,C
        Registers[REGISTER_A] = Registers[REGISTER_A] & Registers[REGISTER_C];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xDC: // AND A,L
        Registers[REGISTER_A] = Registers[REGISTER_A] & Registers[REGISTER_L];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xEC: // AND A,H
        Registers[REGISTER_A] = Registers[REGISTER_A] & Registers[REGISTER_H];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xFC: // AND A,M
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Registers[REGISTER_M] = Memory[address];
        }
        Registers[REGISTER_A] = Registers[REGISTER_A] & Registers[REGISTER_M];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xBB: // OR A,B
        Registers[REGISTER_A] = Registers[REGISTER_A] | Registers[REGISTER_B];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xCB: // OR A,C
        Registers[REGISTER_A] = Registers[REGISTER_A] | Registers[REGISTER_C];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xDB: // OR A,L
        Registers[REGISTER_A] = Registers[REGISTER_A] | Registers[REGISTER_L];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xEB: // OR A,H
        Registers[REGISTER_A] = Registers[REGISTER_A] | Registers[REGISTER_H];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xFB: // OR A,M
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Registers[REGISTER_M] = Memory[address];
        }
        Registers[REGISTER_A] = Registers[REGISTER_A] | Registers[REGISTER_M];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xBD: // XOR A,B
        Registers[REGISTER_A] = Registers[REGISTER_A] ^ Registers[REGISTER_B];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xCD: // XOR A,C
        Registers[REGISTER_A] = Registers[REGISTER_A] ^ Registers[REGISTER_C];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xDD: // XOR A,L
        Registers[REGISTER_A] = Registers[REGISTER_A] ^ Registers[REGISTER_L];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xED: // XOR A,H
        Registers[REGISTER_A] = Registers[REGISTER_A] ^ Registers[REGISTER_H];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0xFD: // XOR A,M
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Registers[REGISTER_M] = Memory[address];
        }
        Registers[REGISTER_A] = Registers[REGISTER_A] ^ Registers[REGISTER_M];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x9F: // CLRA A
        Registers[REGISTER_A] = 0;
        Flags = Flags | FLAG_Z;
        Flags = Flags & (0xFF - (FLAG_N + FLAG_C));
        break;

    case 0x5F: // CLR abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = 0;
        Flags = Flags | FLAG_Z;
        Flags = Flags & (0xFF - (FLAG_N + FLAG_C));
        break;

    case 0x6F: // CLR abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = 0;
        Flags = Flags | FLAG_Z;
        Flags = Flags & (0xFF - (FLAG_N + FLAG_C));
        break;

    case 0x7F: // CLR abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = 0;
        Flags = Flags | FLAG_Z;
        Flags = Flags & (0xFF - (FLAG_N + FLAG_C));
        break;

    case 0x8F: // CLR abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = 0;
        Flags = Flags | FLAG_Z;
        Flags = Flags & (0xFF - (FLAG_N + FLAG_C));
        break;

    case 0x18: // WAI impl
        halt = true;
        break;

    case 0xB7: // SBC A-B
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_B];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word--;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xC7: // SBC A-C
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_C];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word--;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xD7: // SBC A-L
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_L];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word--;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xE7: // SBC A-H
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_H];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word--;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xF7: // SBC A-M
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address < MEMORY_SIZE)
        {
            Registers[REGISTER_M] = Memory[address];
        }
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)Registers[REGISTER_M];
        if ((Flags & FLAG_C) != 0)
        {
            temp_word--;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0x9B: // COMA
        Registers[REGISTER_A] = Registers[REGISTER_A] ^ 0xFF;
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x5B: // COM abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = Memory[address] ^ 0xFF;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x6B: // COM abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = Memory[address] ^ 0xFF;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x7B: // COM abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = Memory[address] ^ 0xFF;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x8B: // COM abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = Memory[address] ^ 0xFF;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x98: // RCLA
        saved_flags = Flags;
        if ((Registers[REGISTER_A] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (Registers[REGISTER_A] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
        }
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x58: // RCL abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        saved_flags = Flags;
        if ((Memory[address] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Memory[address] = Memory[address] | 0x01;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x68: // RCL abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        saved_flags = Flags;
        if ((Memory[address] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Memory[address] = Memory[address] | 0x01;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x78: // RCL abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        saved_flags = Flags;
        if ((Memory[address] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        break;

    case 0x88: // RCL abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        saved_flags = Flags;
        if ((Memory[address] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Memory[address] = Memory[address] | 0x01;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x9D: // RALA A
        saved_flags = Flags;
        Registers[REGISTER_A] = (Registers[REGISTER_A] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
        }
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x5D: // RAL abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        saved_flags = Flags;
        Memory[address] = (Memory[address] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Memory[address] = Memory[address] | 0x01;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x6D: // RAL abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        saved_flags = Flags;
        Memory[address] = (Memory[address] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Memory[address] = Memory[address] | 0x01;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x7D: // RAL abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        saved_flags = Flags;
        Memory[address] = (Memory[address] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Memory[address] = Memory[address] | 0x01;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x8D: // RAL abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        saved_flags = Flags;
        Memory[address] = (Memory[address] << 1) & 0xFF;
        if ((saved_flags & FLAG_C) == FLAG_C) {
            Memory[address] = Memory[address] | 0x01;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x99: // SALA
        if ((Registers[REGISTER_A] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (Registers[REGISTER_A] << 1) & 0xFF;
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x59: // SAL abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if ((Memory[address] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] << 1) & 0xFF;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x69: // SAL abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if ((Memory[address] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] << 1) & 0xFF;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x79: // SAL abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if ((Memory[address] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] << 1) & 0xFF;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x89: // SAL abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if ((Memory[address] & 0x80) == 0x80) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] << 1) & 0xFF;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x9A: // SHRA
        if ((Registers[REGISTER_A] & 0x01) == 0x01) {
            Flags = Flags | FLAG_C;
        }
        else {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (Registers[REGISTER_A] >> 1) & 0x7F;
        if ((Flags & FLAG_N) == FLAG_N) {
            Registers[REGISTER_A] = Registers[REGISTER_A] | 0x80;
        }
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x5A: // SHR abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if ((Memory[address] & 0x01) == 0x01) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] >> 1) & 0x7F;
        if ((Flags & FLAG_N) == FLAG_N) {
            Memory[address] = Memory[address] | 0x80;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x6A: //SHR abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if ((Memory[address] & 0x01) == 0x01) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] >> 1) & 0x7F;
        if ((Flags & FLAG_N) == FLAG_N) {
            Memory[address] = Memory[address] | 0x80;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x7A: //SHR abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if ((Memory[address] & 0x01) == 0x01) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] >> 1) & 0x7F;
        if ((Flags & FLAG_N) == FLAG_N) {
            Memory[address] = Memory[address] | 0x80;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x8A: //SHR abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        if ((Memory[address] & 0x01) == 0x01) {
            Flags = Flags | FLAG_C;
        }
        else
        {
            Flags = Flags & (0xFF - FLAG_C);
        }
        Memory[address] = (Memory[address] >> 1) & 0x7F;
        if ((Flags & FLAG_N) == FLAG_N) {
            Memory[address] = Memory[address] | 0x80;
        }
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x1C: // ADI \#
        data = fetch();
        temp_word = (WORD)Registers[REGISTER_A] + (WORD)data;
        if ((Flags & FLAG_C) != 0)
        {
            temp_word++;
        }
        if (temp_word >= 0x100)
        {
            //set carry falg
            Flags = Flags | FLAG_C;
        }
        else
        {
            // clear carry flag
            Flags = Flags & (0xFF - FLAG_C);
        }
        Registers[REGISTER_A] = (BYTE)temp_word;
        set_flag_Z(Registers[REGISTER_A]);
        set_flag_n(Registers[REGISTER_A]);
        break;

    case 0xD1: // CPI \#
        temp_word = (WORD)Registers[REGISTER_A] - (WORD)data;
        if (temp_word >= 0x100) {
            Flags = Flags | FLAG_C; // set carry flag
        }
        else {
            Flags = Flags & (0xFF - FLAG_C); // clear carry flag
        }
        set_flag_n((BYTE)temp_word);
        set_flag_Z((BYTE)temp_word);
        break;

    case 0xE1: // ANI \#
        Registers[REGISTER_A] = Registers[REGISTER_A] & data;
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x94: // TESTA A
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x54: // TEST abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        data = Memory[address] - 0;
        if ((BYTE)data == 0x00) {
            Flags = Flags | FLAG_Z;
        }
        else {
            Flags = Flags & (~FLAG_Z);
        }
        if (((BYTE)data == 0x80) == 0x80) {
            Flags = Flags | FLAG_N;
        }
        else {
            Flags = Flags & (~FLAG_N);
        }

        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);

        break;

    case 0x64: // TEST abs,X
        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x74: // TEST abs,Y
        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x84: // TEST abs,XY
        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x9C: // NEGA A
        Registers[REGISTER_A] = -Registers[REGISTER_A];
        set_flag_n(Registers[REGISTER_A]);
        set_flag_Z(Registers[REGISTER_A]);
        break;

    case 0x5C: // NEG abs
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = -Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x6C: // NEG abs,X

        address += Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = -Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x7C: // NEG abs,Y

        address += Index_Registers[REGISTER_Y];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = -Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x8C: // NEG abs,XY

        address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
        LB = fetch();
        HB = fetch();
        address += (WORD)((WORD)HB << 8) + LB;
        Memory[address] = -Memory[address];
        set_flag_n(Memory[address]);
        set_flag_Z(Memory[address]);
        break;

    case 0x02:    //SWI impl

        --StackPointer;
        Memory[StackPointer] = Registers[REGISTER_A];
        --StackPointer;
        Memory[StackPointer] = ProgramCounter;
        --StackPointer;
        Memory[StackPointer] = ProgramCounter >> 8;
        --StackPointer;
        Memory[StackPointer] = Flags;
        --StackPointer;
        Memory[StackPointer] = Registers[REGISTER_B];
        --StackPointer;
        Memory[StackPointer] = Registers[REGISTER_C];
        --StackPointer;
        Memory[StackPointer] = Registers[REGISTER_L];
        --StackPointer;
        Memory[StackPointer] = Registers[REGISTER_H];
        Flags = Flags | FLAG_I;

        break;

    case 0x03: // RTI impl

        Registers[REGISTER_H] = Memory[StackPointer];
        StackPointer++;
        Registers[REGISTER_L] = Memory[StackPointer];
        StackPointer++;
        Registers[REGISTER_C] = Memory[StackPointer];
        StackPointer++;
        Registers[REGISTER_B] = Memory[StackPointer];
        StackPointer++;
        Flags = Memory[StackPointer];
        StackPointer++;
        ProgramCounter = Memory[StackPointer];
        StackPointer++;
        ProgramCounter = Memory[StackPointer] << 8;
        StackPointer++;
        Registers[REGISTER_A] = Memory[StackPointer];
        StackPointer++;

        break;

    default:
        break;
    }
}
	

void Group_2_Move(BYTE opcode)
{
	BYTE source = opcode >> 4;
	BYTE destination = opcode & 0x0F;
	int destReg = 0;
	int sourceReg = 0;
	WORD address = 0;

    switch (destination) {
    case 0x01:
        destReg = REGISTER_A;
        break;
    case 0x02:
        destReg = REGISTER_B;
        break;
    case 0x03:
        destReg = REGISTER_C;
        break;
    case 0x04:
        destReg = REGISTER_L;
        break;
    case 0x05:
        destReg = REGISTER_H;
        break;
    case 0x06:
        destReg = REGISTER_M;
        break;
    default:

        break;
    }

    switch (source) {
    case 0x0A:
        sourceReg = REGISTER_A;
        break;
    case 0x0B:
        sourceReg = REGISTER_B;
        break;
    case 0x0C:
        sourceReg = REGISTER_C;
        break;
    case 0x0D:
        sourceReg = REGISTER_L;
        break;
    case 0x0E:
        sourceReg = REGISTER_H;
        break;
    case 0x0F:
        sourceReg = REGISTER_M;
        break;
    default:
        break;
    }
    if (destReg == REGISTER_M) {
        address = Registers[REGISTER_L];
        address += (WORD)Registers[REGISTER_H] << 8;
        if (address >= 0 && address <= MEMORY_SIZE) {
            Memory[address] = Registers[sourceReg];
        }
    }
    else
        if (sourceReg == REGISTER_M) {
            address = Registers[REGISTER_L];
            address += (WORD)Registers[REGISTER_H] << 8;
            if (address >= 0 && address <= MEMORY_SIZE) {
                Memory[address] = Registers[sourceReg];
            }
        }
        else {
            Registers[destReg] = Registers[sourceReg];
        }
}

	


void execute(BYTE opcode)
{	
	BYTE source = opcode >> 4;
	BYTE destination = opcode & 0x0F;

	if (((source >= 0x0A) && (source <= 0x0F)) && ((destination >= 0x00) && (destination <= 0x05)))
	{
		Group_2_Move(opcode);
	}
	else
	{
		Group_1(opcode);
	}
}

void emulate()
{
	BYTE opcode;
	int sanity;

	ProgramCounter = 0;
	halt = false;
	memory_in_range = true;
	sanity = 0;

	printf("                    A  B  C  L  H  X  Y  SP\n");

	while ((!halt) && (memory_in_range)) 
	{
		sanity++;
		if (sanity > 200) halt = true;

		printf("%04X ", ProgramCounter);           // Print current address
		opcode = fetch();
		execute(opcode);

		printf("%s  ", opcode_mneumonics[opcode]);  // Print current opcode

		printf("%02X ", Registers[REGISTER_A]);
		printf("%02X ", Registers[REGISTER_B]);
		printf("%02X ", Registers[REGISTER_C]);
		printf("%02X ", Registers[REGISTER_L]);
		printf("%02X ", Registers[REGISTER_H]);
		printf("%02X ", Index_Registers[REGISTER_X]);
		printf("%02X ", Index_Registers[REGISTER_Y]);
		printf("%04X ", StackPointer);              // Print Stack Pointer

		if ((Flags & FLAG_Z) == FLAG_Z)	
		{
			printf("Z=1 ");
		}
		else
		{
			printf("Z=0 ");
		}
		if ((Flags & FLAG_I) == FLAG_I)	
		{
			printf("I=1 ");
		}
		else
		{
			printf("I=0 ");
		}
		if ((Flags & FLAG_N) == FLAG_N)	
		{
			printf("N=1 ");
		}
		else
		{
			printf("N=0 ");
		}
		if ((Flags & FLAG_C) == FLAG_C)	
		{
			printf("C=1 ");
		}
		else
		{
			printf("C=0 ");
		}

		printf("\n");  // New line
		sanity++;
	}

	printf("\n");  // New line
}


////////////////////////////////////////////////////////////////////////////////
//                            Simulator/Emulator (End)                        //
////////////////////////////////////////////////////////////////////////////////


void initialise_filenames() {
	int i;

	for (i=0; i<MAX_FILENAME_SIZE; i++) {
		hex_file [i] = '\0';
		trc_file [i] = '\0';
	}
}




int find_dot_position(char *filename) {
	int  dot_position;
	int  i;
	char chr;

	dot_position = 0;
	i = 0;
	chr = filename[i];

	while (chr != '\0') {
		if (chr == '.') {
			dot_position = i;
		}
		i++;
		chr = filename[i];
	}

	return (dot_position);
}


int find_end_position(char *filename) {
	int  end_position;
	int  i;
	char chr;

	end_position = 0;
	i = 0;
	chr = filename[i];

	while (chr != '\0') {
		end_position = i;
		i++;
		chr = filename[i];
	}

	return (end_position);
}


bool file_exists(char *filename) {
	bool exists;
	FILE *ifp;

	exists = false;

	if ( ( ifp = fopen( filename, "r" ) ) != NULL ) {
		exists = true;

		fclose(ifp);
	}

	return (exists);
}



void create_file(char *filename) {
	FILE *ofp;

	if ( ( ofp = fopen( filename, "w" ) ) != NULL ) {
		fclose(ofp);
	}
}



bool getline(FILE *fp, char *buffer) {
	bool rc;
	bool collect;
	char c;
	int  i;

	rc = false;
	collect = true;

	i = 0;
	while (collect) {
		c = getc(fp);

		switch (c) {
		case EOF:
			if (i > 0) {
				rc = true;
			}
			collect = false;
			break;

		case '\n':
			if (i > 0) {
				rc = true;
				collect = false;
				buffer[i] = '\0';
			}
			break;

		default:
			buffer[i] = c;
			i++;
			break;
		}
	}

	return (rc);
}






void load_and_run(int args,char** argv) {
	char chr;
	int  ln;
	int  dot_position;
	int  end_position;
	long i;
	FILE *ifp;
	long address;
	long load_at;
	int  code;

	// Prompt for the .hex file

	printf("\n");
	printf("Enter the hex filename (.hex): ");

	if(args == 2){
		ln = 0;
		chr = argv[1][ln];
		while (chr != '\0')
		{
			if (ln < MAX_FILENAME_SIZE)
			{
				hex_file [ln] = chr;
				trc_file [ln] = chr;
				ln++;
			}
			chr = argv[1][ln];
		}
	} else {
		ln = 0;
		chr = '\0';
		while (chr != '\n') {
			chr = getchar();

			switch(chr) {
			case '\n':
				break;
			default:
				if (ln < MAX_FILENAME_SIZE)	{
					hex_file [ln] = chr;
					trc_file [ln] = chr;
					ln++;
				}
				break;
			}
		}

	}
	// Tidy up the file names

	dot_position = find_dot_position(hex_file);
	if (dot_position == 0) {
		end_position = find_end_position(hex_file);

		hex_file[end_position + 1] = '.';
		hex_file[end_position + 2] = 'h';
		hex_file[end_position + 3] = 'e';
		hex_file[end_position + 4] = 'x';
		hex_file[end_position + 5] = '\0';
	} else {
		hex_file[dot_position + 0] = '.';
		hex_file[dot_position + 1] = 'h';
		hex_file[dot_position + 2] = 'e';
		hex_file[dot_position + 3] = 'x';
		hex_file[dot_position + 4] = '\0';
	}

	dot_position = find_dot_position(trc_file);
	if (dot_position == 0) {
		end_position = find_end_position(trc_file);

		trc_file[end_position + 1] = '.';
		trc_file[end_position + 2] = 't';
		trc_file[end_position + 3] = 'r';
		trc_file[end_position + 4] = 'c';
		trc_file[end_position + 5] = '\0';
	} else {
		trc_file[dot_position + 0] = '.';
		trc_file[dot_position + 1] = 't';
		trc_file[dot_position + 2] = 'r';
		trc_file[dot_position + 3] = 'c';
		trc_file[dot_position + 4] = '\0';
	}

	if (file_exists(hex_file)) {
		// Clear Registers and Memory

		Registers[REGISTER_A] = 0;
		Registers[REGISTER_B] = 0;
		Registers[REGISTER_C] = 0;
		Registers[REGISTER_L] = 0;
		Registers[REGISTER_H] = 0;
		Index_Registers[REGISTER_X] = 0;
		Index_Registers[REGISTER_Y] = 0;
		Flags = 0;
		ProgramCounter = 0;
		StackPointer = 0;

		for (i=0; i<MEMORY_SIZE; i++) {
			Memory[i] = 0x00;
		}

		// Load hex file

		if ( ( ifp = fopen( hex_file, "r" ) ) != NULL ) {
			printf("Loading file...\n\n");

			load_at = 0;

			while (getline(ifp, InputBuffer)) {
				if (sscanf(InputBuffer, "L=%x", &address) == 1) {
					load_at = address;
				} else if (sscanf(InputBuffer, "%x", &code) == 1) {
					if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
						Memory[load_at] = (BYTE)code;
					}
					load_at++;
				} else {
					printf("ERROR> Failed to load instruction: %s \n", InputBuffer);
				}
			}

			fclose(ifp);
		}

		// Emulate

		emulate();
	} else {
		printf("\n");
		printf("ERROR> Input file %s does not exist!\n", hex_file);
		printf("\n");
	}
}

void building(int args,char** argv){
	char buffer[1024];
	load_and_run(args,argv);
	sprintf(buffer, "0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X", 
		Memory[TEST_ADDRESS_1],
		Memory[TEST_ADDRESS_2],
		Memory[TEST_ADDRESS_3],
		Memory[TEST_ADDRESS_4], 
		Memory[TEST_ADDRESS_5],
		Memory[TEST_ADDRESS_6], 
		Memory[TEST_ADDRESS_7],
		Memory[TEST_ADDRESS_8], 
		Memory[TEST_ADDRESS_9], 
		Memory[TEST_ADDRESS_10],
		Memory[TEST_ADDRESS_11],
		Memory[TEST_ADDRESS_12]
		);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
}

void show_memory() {
	//ADDED By Mohammad
	unsigned int mem_add = 0;
	printf("\n");
	printf("Enter mem address: ");
	scanf("%X", &mem_add);


	for (int i = 0; i < 16; i++) {
		printf("%X \t", mem_add + i );
		printf("%X \n", Memory[mem_add + i ]);
	}
}

void test_and_mark() {
	char buffer[1024];
	bool testing_complete;
	int  len = sizeof(SOCKADDR);
	char chr;
	int  i;
	int  j;
	bool end_of_program;
	long address;
	long load_at;
	int  code;
	int  mark;
	int  passed;

	printf("\n");
	printf("Automatic Testing and Marking\n");
	printf("\n");

	testing_complete = false;

	sprintf(buffer, "Test Student %s", STUDENT_NUMBER);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));

	while (!testing_complete) {
		memset(buffer, '\0', sizeof(buffer));

		if (recvfrom(sock, buffer, sizeof(buffer)-1, 0, (SOCKADDR *)&client_addr, &len) != SOCKET_ERROR) {
			printf("Incoming Data: %s \n", buffer);

			//if (strcmp(buffer, "Testing complete") == 1)
			if (sscanf(buffer, "Testing complete %d", &mark) == 1) {
				testing_complete = true;
				printf("Current mark = %d\n", mark);

			}else if (sscanf(buffer, "Tests passed %d", &passed) == 1) {
				//testing_complete = true;
				printf("Passed = %d\n", passed);

			} else if (strcmp(buffer, "Error") == 0) {
				printf("ERROR> Testing abnormally terminated\n");
				testing_complete = true;
			} else {
				// Clear Registers and Memory

		Registers[REGISTER_A] = 0;
		Registers[REGISTER_B] = 0;
		Registers[REGISTER_C] = 0;
		Registers[REGISTER_L] = 0;
		Registers[REGISTER_H] = 0;
		Index_Registers[REGISTER_X] = 0;
		Index_Registers[REGISTER_Y] = 0;
				Flags = 0;
				ProgramCounter = 0;
				StackPointer = 0;
				for (i=0; i<MEMORY_SIZE; i++) {
					Memory[i] = 0;
				}

				// Load hex file

				i = 0;
				j = 0;
				load_at = 0;
				end_of_program = false;
				FILE *ofp;
				fopen_s(&ofp ,"branch.txt", "a");

				while (!end_of_program) {
					chr = buffer[i];
					switch (chr) {
					case '\0':
						end_of_program = true;

					case ',':
						if (sscanf(InputBuffer, "L=%x", &address) == 1) {
							load_at = address;
						} else if (sscanf(InputBuffer, "%x", &code) == 1) {
							if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
								Memory[load_at] = (BYTE)code;
								fprintf(ofp, "%02X\n", (BYTE)code);
							}
							load_at++;
						} else {
							printf("ERROR> Failed to load instruction: %s \n", InputBuffer);
						}
						j = 0;
						break;

					default:
						InputBuffer[j] = chr;
						j++;
						break;
					}
					i++;
				}
				fclose(ofp);
				// Emulate

				if (load_at > 1) {
					emulate();
					// Send and store results
					sprintf(buffer, "%02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X", 
						Memory[TEST_ADDRESS_1],
						Memory[TEST_ADDRESS_2],
						Memory[TEST_ADDRESS_3],
						Memory[TEST_ADDRESS_4], 
						Memory[TEST_ADDRESS_5],
						Memory[TEST_ADDRESS_6], 
						Memory[TEST_ADDRESS_7],
						Memory[TEST_ADDRESS_8], 
						Memory[TEST_ADDRESS_9], 
						Memory[TEST_ADDRESS_10],
						Memory[TEST_ADDRESS_11],
						Memory[TEST_ADDRESS_12]
						);
					sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
				}
			}
		}
	}
}



int main(int argc, char* argv[])
{
	char chr;
	char dummy;

	printf("\n");
	printf("Microprocessor Emulator\n");
	printf("UWE Computer and Network Systems Assignment 1\n");
	printf("\n");

	initialise_filenames();

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return(0);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!sock) {	
		// Creation failed! 
	}

	memset(&server_addr, 0, sizeof(SOCKADDR_IN));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	server_addr.sin_port = htons(PORT_SERVER);

	memset(&client_addr, 0, sizeof(SOCKADDR_IN));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	client_addr.sin_port = htons(PORT_CLIENT);

	chr = '\0';
	while ((chr != 'e') && (chr != 'E'))
	{
		printf("\n");
		printf("Please select option\n");
		printf("L - Load and run a hex file\n");
		printf("T - Have the server test and mark your emulator\n");
		printf("M - Show memory\n");  //ADDED By Mohammad
		printf("E - Exit\n");
		if(argc == 2){ building(argc,argv); exit(0);}
		printf("Enter option: ");
		chr = getchar();
		if (chr != 0x0A)
		{
			dummy = getchar();  // read in the <CR>
		}
		printf("\n");

		switch (chr)
		{
		case 'L':
		case 'l':
			load_and_run(argc,argv);
			break;

		case 'T':
		case 't':
			test_and_mark();
			break;
		case 'M':   //ADDED By Mohammad
		case 'm':
			show_memory();
			break;
		default:
			break;
		}
	}

	closesocket(sock);
	WSACleanup();


	return 0;
}


