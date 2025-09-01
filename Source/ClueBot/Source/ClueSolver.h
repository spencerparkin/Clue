#pragma once

#include "Card.h"
#include <vector>
#include <list>
#include <memory>
#include <optional>

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
	friend class HasNoneOfHint;
	friend class HasAtLeastOneOfHint;

public:
	ClueSolver();
	virtual ~ClueSolver();

	enum KnowledgeElement
	{
		OWNED,
		NOT_OWNED,
		OWNER_UNKNOWN
	};

	struct CardHolder
	{
		union
		{
			Clue::Character character;
			int32_t value;
		};
		uint32_t numCards;
	};

	class Hint
	{
	public:
		Hint(Clue::Character character);

		/**
		 * Apply the hint to the knowledge matrix.  Return true if and only if
		 * the hint can now be discarded.  Also, return the number of changes
		 * made to the knowledge matrix.
		 */
		virtual bool Apply(ClueSolver* clueSolver, uint32_t& numChanges) = 0;

		Clue::Character character;
		std::vector<Clue::Card> cardArray;
	};

	class HasNoneOfHint : public Hint
	{
	public:
		HasNoneOfHint(Clue::Character character);

		virtual bool Apply(ClueSolver* clueSolver, uint32_t& numChanges) override;
	};

	class HasAtLeastOneOfHint : public Hint
	{
	public:
		HasAtLeastOneOfHint(Clue::Character character);

		virtual bool Apply(ClueSolver* clueSolver, uint32_t& numChanges) override;
	};

	void Clear();
	void Reset(const std::vector<CardHolder>& givenCardHolderArray);
	void AddHint(std::shared_ptr<Hint> hint);
	void Reduce();
	bool Solve(Clue::Accusation& correctAccusation) const;
	Clue::Accusation GetRecommendedAccusation() const;

	int GetColumnForCard(const Clue::Card& card) const;
	int GetRowForCharacter(Clue::Character character) const;

private:

	void ReduceMatrix();

	// The rows are card holders.  The columns are cards.
	KnowledgeElement** knowledgeMatrix;

	std::vector<Clue::Card> cardArray;
	std::vector<CardHolder> cardHolderArray;
	std::list<std::shared_ptr<Hint>> hintList;
};