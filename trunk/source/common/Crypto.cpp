/*  
    EQ2Emulator:  Everquest II Server Emulator
    Copyright (C) 2007  EQ2EMulator Development Team (http://www.eq2emulator.net)

    This file is part of EQ2Emulator.

    EQ2Emulator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EQ2Emulator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EQ2Emulator.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "Crypto.h"

uint64_t Crypto::RSADecrypt(unsigned char* text, uint16_t size){
    return ntohll(*reinterpret_cast<uint64_t*>(text));
}

void Crypto::RC4Decrypt(unsigned char* text, uint32_t size){
	client->Cypher(text, size);
}

void Crypto::RC4Encrypt(unsigned char* text, uint32_t size){
	server->Cypher(text, size);
}