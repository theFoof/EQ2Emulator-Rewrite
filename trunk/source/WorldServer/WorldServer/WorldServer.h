#pragma once

#include "../../common/UDPServer.h"

class WorldServer : public UDPServer {
public:
	WorldServer();

	bool Process() override;
	bool ProcessClientWrite();
	bool ProcessClients();

	void SetName(std::string name) { Name = name; }
	std::string GetName() { return Name; }
	void SetMaxCharactersPerAccount(uint8_t num) { MaxCharactersPerAccount = num; }
	uint8_t GetMaxCharactersPerAccount() { return MaxCharactersPerAccount; }
	void SetAllowedRaces(uint32_t races) { AllowedRaces = races; }
	uint32_t GetAllowedRaces() { return AllowedRaces; }
	void SetAllowedClasses(uint32_t classes) { AllowedClasses = classes; }
	uint32_t GetAllowedClasses() { return AllowedClasses; }
	void SetMaxLevel(uint8_t level) { MaxLevel = level; }
	uint8_t GetMaxLevel() { return MaxLevel; }

protected:
	Stream* GetNewStream(unsigned int ip, unsigned short port) override;

private:
	std::string Name;
	uint8_t MaxCharactersPerAccount;
	uint32_t AllowedRaces;
	uint32_t AllowedClasses;
	uint8_t MaxLevel;

};