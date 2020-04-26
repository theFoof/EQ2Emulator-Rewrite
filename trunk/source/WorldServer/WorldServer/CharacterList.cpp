#include "stdafx.h"

#include "CharacterList.h"
#include "Character.h"

void CharacterList::AddCharacter(const std::shared_ptr<Character>& c) {
	charMap[c->basicInfo.characterID] = c;
}

std::shared_ptr<Character> CharacterList::GetCharacterByID(uint32_t id) {
	auto itr = charMap.find(id);
	if (itr != charMap.end()) {
		return itr->second;
	}
	return std::shared_ptr<Character>();
}

std::shared_ptr<Character> CharacterList::GetCharacterBySessionID(uint32_t session) {
	for (auto& itr : charMap) {
		if (itr.second->GetSessionID() == session) {
			return itr.second;
		}
	}
	return std::shared_ptr<Character>();
}