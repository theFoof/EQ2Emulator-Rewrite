#pragma once

#include "../PacketElement.h"

struct EQ2Color {
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
};

class PacketEQ2Color : public PacketElement {
public:
	PacketEQ2Color(EQ2Color& in_int) : element(&in_int) {

	}

	PacketEQ2Color(uint8_t r, uint8_t g, uint8_t b) {
		element = new EQ2Color;
		element->Red = r;
		element->Green = g;
		element->Blue = b;
	}

	~PacketEQ2Color() = default;

	bool ReadElement(const unsigned char* srcbuf, uint32_t& offset, uint32_t bufsize) {
		if (!BoundsCheckRead(offset, bufsize)) {
			return false;
		}

		memcpy(element, srcbuf + offset, GetSize());
		offset += GetSize();

		return true;
	}

	void WriteElement(unsigned char* outbuf, uint32_t& offset) {
		memcpy(outbuf + offset, element, GetSize());
		offset += GetSize();
	}

	uint32_t GetSize() {
		return sizeof(EQ2Color) * count;
	}

private:
	EQ2Color* element;

};