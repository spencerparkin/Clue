#pragma once

#include "Defines.h"
#include <stdint.h>

namespace Clue
{
	/**
	 *
	 */
	class CLUE_LIBRARY_API RingBuffer
	{
	public:
		RingBuffer(uint32_t size);
		virtual ~RingBuffer();

		bool WriteBytes(const uint8_t* byteArray, uint32_t byteArraySize);
		bool PeakBytes(uint8_t* byteArray, uint32_t byteArraySize);
		bool ReadBytes(uint8_t* byteArray, uint32_t byteArraySize);
		bool DeleteBytes(uint32_t numBytes);

		uint32_t GetNumFreeBytes() const;
		uint32_t GetNumStoredBytes() const;

	private:
		uint32_t startOffset;
		uint32_t endOffset;
		uint8_t* buffer;
		uint32_t bufferSize;
		uint32_t numFreeBytes;
	};
}