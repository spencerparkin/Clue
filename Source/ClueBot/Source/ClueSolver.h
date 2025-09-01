#pragma once

#include "Card.h"
#include <vector>
#include <list>

/**
 * Note that this class does not solve the game of clue entirely on its own.  It must
 * be fed the best possible hints.  A good clue player will go to the best rooms
 * and make the best possible accusations to produce the best possible hints.
 * The goal of this class is to just make sure that every given hint is accounted
 * for and used to its fullest extent to solve the mystery.  This ensures that we're
 * able to solve the mystery as soon as possible based on all available information
 * to-date.  Note that hints don't just come from interactions a player has with other
 * players, but interactions between any two players at any time.
 * 
 * One stratagy for making accusations is to first identify a card whose location
 * you would like to know.  Then, when you formulate the accusation, ensure that
 * no player can refute that accusation in any way other than to reveal whether
 * or not they have the card.  To do that, make sure that the accusation includes
 * only cards that either you or the server owns, except for the card in question.
 */
class ClueSolver
{
public:
	ClueSolver();
	virtual ~ClueSolver();

	struct CardHolder
	{
		union
		{
			Clue::Character character;
			int32_t value;
		};
		uint32_t numCards;
	};

	struct Hint
	{
		enum Type
		{
			HAS_NONE_OF,
			HAS_AT_LEAST_ONE_OF
		};

		uint32_t cardHolder;
		std::vector<int> cardArray;
	};

	void Clear();
	void Reset(const std::vector<CardHolder>& givenCardHolderArray);
	void AddHint(const Hint& hint);
	void Reduce();
	bool Solve(Clue::Accusation& correctAccusation) const;

private:

	enum KnowledgeElement
	{
		OWNED,
		NOT_OWNED,
		OWNER_UNKNOWN
	};

	// The rows are card holders.  The columns are cards.
	KnowledgeElement** knowledgeMatrix;

	std::vector<Clue::Card> cardArray;
	std::vector<CardHolder> cardHolderArray;
	std::list<Hint> hintList;
};