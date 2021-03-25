#pragma once
#include "../CommonDatabase.h"
#include <unordered_set>

class ParserDatabase : public CommonDatabase {
private:
	ParserDatabase();
public:
	~ParserDatabase() = default;

	bool Start();
	std::unordered_map<std::string, uint16_t> LoadOpcodesForVersion(uint32_t ver);
	std::unordered_set<int32_t> LoadExistingItems();
	std::unordered_map<std::string, uint32_t> LoadExistingItemSets();
	uint32_t CreateLogEntry(std::string name, uint32_t ver);
	uint32_t CreateItemSet(std::string name);
	uint32_t CreateItemSetBonus(uint32_t set_id, uint32_t index, uint32_t items_needed);

	static ParserDatabase* GetGlobal() { static ParserDatabase g_database; return &g_database; }
};