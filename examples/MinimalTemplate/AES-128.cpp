/******************************************************************************************
* Copyright 2017 Ideetron B.V.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************************/
/****************************************************************************************
* File:     AES-128.cpp
* Author:   Gerben den Hartog
* Company:  Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         13-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#include <string.h>
#include <stdint.h>
#include "AES-128.h"


uint8_t S_Table[16][16] = {
	{0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76},
	{0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0},
	{0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15},
	{0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75},
	{0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84},
	{0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF},
	{0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8},
	{0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2},
	{0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73},
	{0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB},
	{0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79},
	{0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08},
	{0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A},
	{0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E},
	{0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF},
	{0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16}
};




/*
*****************************************************************************************
* Title         : AES_Encrypt
* Description	: 
*****************************************************************************************
*/
void AES_Encrypt(uint8_t *Data, uint8_t *Key)
{
	uint8_t Row, Column, Round = 0;
	uint8_t Round_Key[16];
    uint8_t State[4][4];

	//  Copy input to State arry
	for( Column = 0; Column < 4; Column++ )
	{
		for( Row = 0; Row < 4; Row++ )
		{
			State[Row][Column] = Data[Row + (Column << 2)];
		}
	}

	//  Copy key to round key
	memcpy( &Round_Key[0], &Key[0], 16 );

	//  Add round key
	AES_Add_Round_Key( Round_Key, State );

	//  Preform 9 full rounds with mixed collums
	for( Round = 1 ; Round < 10 ; Round++ )
	{
		//  Perform Byte substitution with S table
		for( Column = 0 ; Column < 4 ; Column++ )
		{
			for( Row = 0 ; Row < 4 ; Row++ )
			{
				State[Row][Column] = AES_Sub_Byte( State[Row][Column] );
			}
		}

		//  Perform Row Shift
		AES_Shift_Rows(State);

		//  Mix Collums
		AES_Mix_Collums(State);

		//  Calculate new round key
		AES_Calculate_Round_Key(Round, Round_Key);

        //  Add the round key to the Round_key
		AES_Add_Round_Key(Round_Key, State);
	}

	//  Perform Byte substitution with S table whitout mix collums
	for( Column = 0 ; Column < 4 ; Column++ )
	{
		for( Row = 0; Row < 4; Row++ )
		{
			State[Row][Column] = AES_Sub_Byte(State[Row][Column]);
		}
	}
    
	//  Shift rows
	AES_Shift_Rows(State);

	//  Calculate new round key
	AES_Calculate_Round_Key( Round, Round_Key );

    //  Add round key
	AES_Add_Round_Key( Round_Key, State );

	//  Copy the State into the data array
	for( Column = 0; Column < 4; Column++ )
	{
		for( Row = 0; Row < 4; Row++ )
		{
			Data[Row + (Column << 2)] = State[Row][Column];
		}
	}
} // AES_Encrypt


/*
*****************************************************************************************
* Title         : AES_Add_Round_Key
* Description	: 
*****************************************************************************************
*/
void AES_Add_Round_Key(uint8_t *Round_Key, uint8_t (*State)[4])
{
	uint8_t Row, Collum;

	for(Collum = 0; Collum < 4; Collum++)
	{
		for(Row = 0; Row < 4; Row++)
		{
			State[Row][Collum] ^= Round_Key[Row + (Collum << 2)];
		}
	}
} // AES_Add_Round_Key


/*
*****************************************************************************************
* Title         : AES_Sub_Byte
* Description	: 
*****************************************************************************************
*/
uint8_t AES_Sub_Byte(uint8_t Byte)
{
	return S_Table [ ((Byte >> 4) & 0x0F) ] [ ((Byte >> 0) & 0x0F) ];
} //    AES_Sub_Byte


/*
*****************************************************************************************
* Title         : AES_Shift_Rows
* Description	: 
*****************************************************************************************
*/
void AES_Shift_Rows(uint8_t (*State)[4])
{
	uint8_t Buffer;

	//Store firt byte in buffer
	Buffer      = State[1][0];
	//Shift all bytes
	State[1][0] = State[1][1];
	State[1][1] = State[1][2];
	State[1][2] = State[1][3];
	State[1][3] = Buffer;

	Buffer      = State[2][0];
	State[2][0] = State[2][2];
	State[2][2] = Buffer;
	Buffer      = State[2][1];
	State[2][1] = State[2][3];
	State[2][3] = Buffer;

	Buffer      = State[3][3];
	State[3][3] = State[3][2];
	State[3][2] = State[3][1];
	State[3][1] = State[3][0];
	State[3][0] = Buffer;
}   //  AES_Shift_Rows


/*
*****************************************************************************************
* Title         : AES_Mix_Collums
* Description	: 
*****************************************************************************************
*/
void AES_Mix_Collums(uint8_t (*State)[4])
{
	uint8_t Row,Collum;
	uint8_t a[4], b[4];
    
    
	for(Collum = 0; Collum < 4; Collum++)
	{
		for(Row = 0; Row < 4; Row++)
		{
			a[Row] =  State[Row][Collum];
			b[Row] = (State[Row][Collum] << 1);

			if((State[Row][Collum] & 0x80) == 0x80)
			{
				b[Row] ^= 0x1B;
			}
		}
        
		State[0][Collum] = b[0] ^ a[1] ^ b[1] ^ a[2] ^ a[3];
		State[1][Collum] = a[0] ^ b[1] ^ a[2] ^ b[2] ^ a[3];
		State[2][Collum] = a[0] ^ a[1] ^ b[2] ^ a[3] ^ b[3];
		State[3][Collum] = a[0] ^ b[0] ^ a[1] ^ a[2] ^ b[3];
	}
}   //  AES_Mix_Collums



/*
*****************************************************************************************
* Title         : AES_Calculate_Round_Key
* Description	: 
*****************************************************************************************
*/
void AES_Calculate_Round_Key(uint8_t Round, uint8_t *Round_Key)
{
	uint8_t i, j, b, Rcon;
	uint8_t Temp[4];

    
    //Calculate Rcon
	Rcon = 0x01;
	while(Round != 1)
	{
		b = Rcon & 0x80;
		Rcon = Rcon << 1;
        
		if(b == 0x80)
		{
			Rcon ^= 0x1b;
		}
		Round--;
	}
    
	//  Calculate first Temp
	//  Copy laste byte from previous key and subsitute the byte, but shift the array contents around by 1.
    Temp[0] = AES_Sub_Byte( Round_Key[12 + 1] );
    Temp[1] = AES_Sub_Byte( Round_Key[12 + 2] );
    Temp[2] = AES_Sub_Byte( Round_Key[12 + 3] );
    Temp[3] = AES_Sub_Byte( Round_Key[12 + 0] );

	//  XOR with Rcon
	Temp[0] ^= Rcon;

	//  Calculate new key
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			Round_Key[j + (i << 2)]  ^= Temp[j];
			Temp[j]                   = Round_Key[j + (i << 2)];
		}
	}
}   //  AES_Calculate_Round_Key




 
