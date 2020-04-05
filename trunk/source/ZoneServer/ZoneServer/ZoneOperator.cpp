#include "stdafx.h"

#include "ZoneOperator.h"
#include "Client.h"
#include "ZoneServer.h"

#include "../Packets/OP_LoginByNumRequestMsg_Packet.h"
#include "../Packets/OP_ZoneInfoMsg_Packet.h"
#include "../Packets/OP_SetRemoteCmdsMsg_Packet.h"

ZoneOperator::ZoneOperator() {
	commands_packet = nullptr;
}

ZoneOperator::~ZoneOperator() {
	delete commands_packet;
	for (std::pair<uint32_t, Command*> kvp : commands)
		delete kvp.second;
	commands.clear();
}

bool ZoneOperator::Process() {
	return ProcessClientWrite() && ProcessClients();
}

void ZoneOperator::AddStream(std::shared_ptr<Stream> stream, std::string key) {
	UDPServer::AddStream(stream, key);

	clients.insert(std::static_pointer_cast<Client>(stream));
}

bool ZoneOperator::ProcessClientWrite() {
	ReadLocker lock(streamLock);

	std::map<std::string, std::shared_ptr<Stream> >::iterator stream_itr;
	for (stream_itr = Streams.begin(); stream_itr != Streams.end(); stream_itr++) {
		std::static_pointer_cast<Client>(stream_itr->second)->Write();
	}

	return true;
}

bool ZoneOperator::ProcessClients() {
	/*ReadLocker lock(streamLock);

	std::map<std::string, Stream*>::iterator stream_itr;
	for (stream_itr = Streams.begin(); stream_itr != Streams.end(); stream_itr++) {
		((Client*)stream_itr->second)->Process();
	}

	return true;*/

	std::set<std::weak_ptr<Client>, std::owner_less<std::weak_ptr<Client> > > itr_copy = clients;
	for (auto& itr : itr_copy) {
		if (std::shared_ptr<Client> c = itr.lock()) {
			c->Process();		
		}
		else {
			clients.erase(itr);
		}
	}

	return true;
}

std::shared_ptr<Stream> ZoneOperator::GetNewStream(unsigned int ip, unsigned short port) {
	return std::make_shared<Client>(ip, port);
}

void ZoneOperator::ClientLogIn(std::shared_ptr<Client> client, OP_LoginByNumRequestMsg_Packet* packet) {

	client->SetAccountID(packet->account_id);
	std::map<uint32_t, PendingClient>::iterator itr = pending_clients.find(packet->account_id);
	if (itr != pending_clients.end()) {
		if (itr->second.access_code != packet->access_code) {
			client->SendLoginReply(1);
			return;
		}

		ZoneServer* z = GetZone(itr->second.zone_id, itr->second.instance_id);
		if (!z) {
			client->SendLoginReply(3);
			return;
		}
		
		// Login error, 0 = accepted, 1 = invalid password, 2 = currently playing, 6 = bad version, every thing else = unknown reason
		client->SendLoginReply(0);
		client->SetCharacterID(itr->second.character_id);
		clients.erase(client);
		z->AddClient(client);
	}
	else {
		client->SendLoginReply(3);
	}
}

void ZoneOperator::AddPendingClient(uint32_t account_id, PendingClient pending_client) {
	pending_clients[account_id] = pending_client;
}

ZoneServer* ZoneOperator::AddNewZone(uint32_t zone_id, uint32_t instance_id) {
	ZoneServer* ret = nullptr;

	ret = GetZone(zone_id, instance_id);
	if (ret) {
		LogWarn(LOG_ZONE, 0, "Attempted to start a zone (%u) that was already running", zone_id);
		return ret;
	}

	// TODO: Make an instance
	ZoneServer* z = new ZoneServer(zone_id);
	if (z->Init()) {
		std::pair<uint32_t, uint32_t> key = std::make_pair(zone_id, instance_id);
		zones[key] = z;
		ret = z;
	}
	else {
		LogError(LOG_ZONE, 0, "Failed to start zone %u", zone_id);
		delete z;
	}

	return ret;
}

ZoneServer* ZoneOperator::GetZone(uint32_t zone_id, uint32_t instance_id) {
	std::pair<uint32_t, uint32_t> key = std::make_pair(zone_id, instance_id);
	std::map<std::pair<uint32_t, uint32_t>, ZoneServer*>::iterator itr = zones.find(key);
	if (itr != zones.end())
		return itr->second;

	return nullptr;
}

void ZoneOperator::AddCommand(uint32_t id, std::string name) {
	Command* cmd = new Command();
	cmd->name = name;
	commands[id] = cmd;
}

void ZoneOperator::AddSubCommand(std::string parent_cmd, uint32_t id, std::string name) {
	Command* cmd = nullptr;

	for (std::pair<uint32_t, Command*> kvp : commands) {
		if (kvp.second->name == parent_cmd) {
			cmd = kvp.second;
			break;
		}
	}

	if (cmd) {
		cmd->subcommands[id] = name;
	}
	else
		LogError(LOG_COMMAND, 0, "Error trying to add subcommand %s (%u) to the parent command %s", name.c_str(), id, parent_cmd.c_str());
}

OP_SetRemoteCmdsMsg_Packet* ZoneOperator::GetCommandsPacket(uint32_t version) {
	OP_SetRemoteCmdsMsg_Packet* p = new OP_SetRemoteCmdsMsg_Packet(version);

	uint32_t index = 1;
	for (std::pair<uint32_t, Command*> kvp : commands) {

		while (kvp.first > index) {
			OP_SetRemoteCmdsMsg_Packet::Command command;
			p->commands_array.push_back(command);
			index++;
		}

		OP_SetRemoteCmdsMsg_Packet::Command command;
		command.name = kvp.second->name;
		p->commands_array.push_back(command);
		index++;
	}
	
	return p;
}