#include "stdafx.h"

#include "CharacterSheet.h"
#include "../Packets/OP_UpdateCharacterSheetMsg_Packet.h"

void UpdateCharacterSheetMsgData::WriteElement(unsigned char* outbuf, uint32_t& offset) {
	//Convert data that is a different format in the packet vs what we use
	advExp = experience.currentAdvExp;
	advExpNextLevel = experience.nextAdvLevelExp;
	advExpDebt = experience.advExpDebt;
	advVitality = experience.advVitality;
	tsExp = experience.currentTsExp;
	tsExpNextLevel = experience.nextTsLevelExp;
	tsVitality = experience.tsVitality;
	hp = attributes->hp.currentValue;

	auto stringToChar = [](const std::string& str, char* out, size_t outsize) {
		if (str.empty()) {
			out[0] = '\0';
			return;
		}

		size_t count = std::min<>(str.length(), outsize - 1);
		memcpy(out, str.c_str(), count);
		out[count] = '\0';
	};

	stringToChar(namingInfo->name, character_name, sizeof(character_name));
	stringToChar(namingInfo->last_name, last_name, sizeof(last_name));

	PacketEncodedData::WriteElement(outbuf, offset);
}