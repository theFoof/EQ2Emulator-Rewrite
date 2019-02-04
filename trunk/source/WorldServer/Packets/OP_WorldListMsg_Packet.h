#pragma once

#include "../../common/Packets/EQ2Packet.h"
#include "../../common/Packets/EQ2Packets/OpcodeManager.h"
#include "../../common/Packets/PacketElements/PacketElements.h"

class OP_WorldListMsg_Packet : public EQ2Packet {
public:
	OP_WorldListMsg_Packet(uint16_t version)
		: EQ2Packet(version) {
		RegisterElements();

		NumWorlds = 1;
		ID = 1;
		Name = "Rewrite Test Server";
		Name2 = "Rewrite Test Server";
		Tag = 1; // 0 = used to show down servers, server should never be down so set to 1
		Locked = 0;
		Hidden = 0;
		Unknown = 0;
		NumPlayers = 0;
		Load = 0;	// valid values seem to be 0, 1, and 2
		NumOnlineFlag = 1;
		FeatureSet = 0;

		if (version < 1096)
			AllowedRaces = 0x0007FFFF; // without Freeblood and Aerakyn race
		else if (version < 60006)
			AllowedRaces = 0x000FFFFF; // with Freeblood
		else
			AllowedRaces = 0x001FFFFF;	// with Aerakyn

		Unknown2 = 0;
	}

	
	// Following should be in an arry but as there will only ever be
	// 1 world (this world) listed in the packet we will skip that
	uint32_t ID;
	std::string Name;
	std::string Name2;
	uint8_t Tag;
	uint8_t Locked;
	uint8_t Hidden;
	uint8_t Unknown;
	uint16_t NumPlayers;
	uint8_t Load;
	uint8_t NumOnlineFlag;
	uint8_t FeatureSet;
	uint32_t AllowedRaces;
	// End of Array elements

	uint8_t Unknown2; // 60100

private:
	// Normally this would be the array size, but it should only ever be set to 1 now
	uint8_t NumWorlds;

	void RegisterElements() {
		RegisterUInt8(NumWorlds);
		RegisterUInt32(ID);
		Register16String(Name);
		Register16String(Name2);
		RegisterUInt8(Tag);
		RegisterUInt8(Locked);
		RegisterUInt8(Hidden);
		RegisterUInt8(Unknown);
		RegisterUInt16(NumPlayers);
		RegisterUInt8(Load);
		RegisterUInt8(NumOnlineFlag);
		RegisterUInt8(FeatureSet);
		RegisterUInt32(AllowedRaces);
		if (GetVersion() > 60100)
			RegisterUInt8(Unknown2);
	}

};

RegisterWorldStruct("OP_WorldListMsg", OP_WorldListMsg_Packet, 1, 1);

/*
<Struct Name="LS_WorldList" ClientVersion="1" OpcodeName="OP_WorldListMsg">
<Data ElementName="num_worlds" Type="int8" />
<Data ElementName="world_list" Type="Array" ArraySizeVariable="num_worlds">
  <Data ElementName="id" Type="int32" Size="1" />
  <Data ElementName="name" Type="EQ2_16BitString" />
  <Data ElementName="name2" Type="EQ2_16BitString" />
  <Data ElementName="tag" Type="int8" Size="1" />
  <Data ElementName="locked" Type="int8" Size="1" />
  <Data ElementName="hidden" Type="int8" Size="1" />
  <Data ElementName="unknown" Type="int8" Size="1" />
  <Data ElementName="num_players" Type="int16" Size="1" />
  <Data ElementName="load" Type="int8" Size="1" />
  <Data ElementName="number_online_flag" Type="int8" Size="1" />
  <Data ElementName="feature_set" Type="int8" Size="1" />
  <Data ElementName="allowed_races" Type="int32" Size="1" />
</Data>
</Struct>
<Struct Name="LS_WorldList" ClientVersion="60100" OpcodeName="OP_WorldListMsg">
<Data ElementName="num_worlds" Type="int8" />
<Data ElementName="world_list" Type="Array" ArraySizeVariable="num_worlds">
  <Data ElementName="id" Type="int32" Size="1" />
  <Data ElementName="name" Type="EQ2_16BitString" />
  <Data ElementName="name2" Type="EQ2_16BitString" />
  <Data ElementName="tag" Type="int8" Size="1" />
  <Data ElementName="locked" Type="int8" Size="1" />
  <Data ElementName="hidden" Type="int8" Size="1" />
  <Data ElementName="unknown" Type="int8" Size="1" />
  <Data ElementName="num_players" Type="int16" Size="1" />
  <Data ElementName="load" Type="int8" Size="1" />
  <Data ElementName="number_online_flag" Type="int8" Size="1" />
  <Data ElementName="feature_set" Type="int8" Size="2" />
  <Data ElementName="allowed_races" Type="int32" Size="1" />
</Data>
<Data ElementName="unknown2" Type="int8" />
</Struct>
*/