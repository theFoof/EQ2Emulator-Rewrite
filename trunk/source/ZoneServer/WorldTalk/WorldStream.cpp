#include "stdafx.h"

#include "WorldStream.h"
#include "../../common/EmuPacket.h"
#include "../../common/util.h"

void WorldStream::Process() {
	auto incoming = PopIncoming();

	for (auto& itr : incoming) {
		DumpBytes(itr->buffer, itr->Size);
	}

	auto outgoing = PopOutgoing();

	for (auto& itr : outgoing) {
		WritePacket(Sock, itr->buffer, itr->Size);
	}
}