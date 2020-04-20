#include "stdafx.h"

#include "Client.h"
#include "../Packets/OP_LoginByNumRequestMsg_Packet.h"
#include "../../common/EQ2Stream.h"

#include "../../common/Packets/EQ2Packets/OP_LoginReplyMsg_Packet.h"
#include "../Controllers/PlayerController.h"
#include "../Spawns/Spawn.h"
#include "../ZoneServer/ZoneServer.h"

Client::Client(unsigned int ip, unsigned short port) : EQ2Stream(ip, port), m_nextSpawnIndex(1) {
	account_id = 0;
	character_id = 0;
	m_controller = std::make_shared<PlayerController>();
}

void Client::Process() {
	while (EQ2Packet* p = PopPacket()) {
		if (NetDebugEnabled()) {
			LogError(LOG_PACKET, 0, "ZoneServer client packet dump");
			DumpBytes(p->buffer, p->Size);
		}
		p->HandlePacket(std::static_pointer_cast<Client>(shared_from_this()));
		delete p;
	}
}

void Client::ReadVersionPacket(const unsigned char* data, uint32_t size, uint32_t offset, uint16_t opcode) {
	//Choose the struct version depending on the size of the data
	uint32_t remaining_size = size - offset;
	uint32_t pver;
	if (remaining_size == 30) {
		pver = 283;
	}
	else if (remaining_size > 34) {
		pver = 1212;
	}
	else {
		pver = 284;
	}
	OP_LoginByNumRequestMsg_Packet p(pver);
	p.Read(data, offset, size);
	SetVersion(static_cast<uint32_t>(p.version));
	p.HandlePacket(std::static_pointer_cast<Client>(shared_from_this()));
}

void Client::SendLoginReply(uint8_t reply) {
	// Login error, 0 = accepted, 1 = invalid password, 2 = currently playing, 6 = bad version, every thing else = unknown reason
	LogDebug(LOG_CLIENT, 0, "SendLoginReply called with %u", reply);

	OP_LoginReplyMsg_Packet* r = new OP_LoginReplyMsg_Packet(GetVersion());
	r->Response = reply;
	/*if (reply == 0) {
		r->AccountID = AccountID;
		r->ResetAppearance = 0;
		r->DoNotForceSoga = 1;
		r->RaceUnknown = 63;
		r->Unknown11 = 7;
		r->SubscriptionLevel = 2;
		r->RaceFlag = GetServer()->GetAllowedRaces();	//0x001FFFFF;
		r->ClassFlag = GetServer()->GetAllowedClasses();	//0x07FFFFFE;

		// Unknown5 and Unknown7 set to DoV values
		r->Unknown5 = 1148;
		r->Unknown7 = 2145009599;

		r->Unknown10 = 1;
		r->NumClassItems = 0;
		r->UnknownArraySize = 0;
	}
	else {
		r->AccountID = 0xFFFFFFFF;
		r->ParentalControlTimer = 0xFFFFFFFF;
	}*/
	QueuePacket(r);
}

void Client::Disconnected() {
	std::shared_ptr<ZoneServer> zone = m_zone.lock();
	if (zone) {
		zone->RemoveClient(std::dynamic_pointer_cast<Client>(shared_from_this()));
	}
}

bool Client::WasSentSpawn(const std::shared_ptr<Spawn>& spawn) {
	return m_spawnIndexMap.count(spawn) != 0;
}

uint16_t Client::AddSpawnToIndexMap(const std::shared_ptr<Spawn>& spawn) {
	uint16_t index = m_nextSpawnIndex.fetch_add(1);
	m_spawnIndexMap[spawn] = index;
	m_spawnLookupMap[index] = spawn;
	return index;
}

uint16_t Client::GetIndexForSpawn(std::shared_ptr<Spawn> spawn) {
	uint16_t index = 0;

	std::map<std::weak_ptr<Spawn>, uint16_t>::iterator itr = m_spawnIndexMap.find(spawn);
	if (itr != m_spawnIndexMap.end())
		index = itr->second;

	return index;
}

std::shared_ptr<Spawn> Client::GetSpawnByIndex(uint16_t spawn_index) {
	auto itr = m_spawnLookupMap.find(spawn_index);
	if (itr != m_spawnLookupMap.end()) {
		return itr->second.lock();
	}

	return std::shared_ptr<Spawn>();
}

std::shared_ptr<PlayerController> Client::GetController() {
	return m_controller;
}