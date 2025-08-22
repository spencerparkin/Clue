#pragma once

#include "Packet.h"

class Player;

/**
 * 
 */
class PacketHandler
{
public:
	virtual bool HandlePacket(const std::shared_ptr<Clue::Packet> packet, Player* player) = 0;
};

/**
 * 
 */
class CharAndCardsPacketHandler : public PacketHandler
{
public:
	virtual bool HandlePacket(const std::shared_ptr<Clue::Packet> packet, Player* player) override;
};

/**
 * 
 */
class PlayerIntroPacketHandler : public PacketHandler
{
public:
	virtual bool HandlePacket(const std::shared_ptr<Clue::Packet> packet, Player* player) override;
};

/**
 *
 */
class DiceRollPacketHandler : public PacketHandler
{
public:
	virtual bool HandlePacket(const std::shared_ptr<Clue::Packet> packet, Player* player) override;
};