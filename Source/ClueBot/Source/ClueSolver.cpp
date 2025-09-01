#include "ClueSolver.h"

using namespace Clue;

ClueSolver::ClueSolver()
{
	this->knowledgeMatrix = nullptr;
}

/*virtual*/ ClueSolver::~ClueSolver()
{
	this->Clear();
}

void ClueSolver::Clear()
{
	// TODO: Delete knowledge matrix here.

	this->cardHolderArray.clear();
	this->cardArray.clear();
}

void ClueSolver::Reset(const std::vector<CardHolder>& givenCardHolderArray)
{
	this->Clear();

	for (const CardHolder& cardHolder : givenCardHolderArray)
		this->cardHolderArray.push_back(cardHolder);
	
	CardHolder server;
	server.numCards = 3;
	server.value = -1;
	this->cardHolderArray.push_back(server);

	Card::GenerateDeck(this->cardArray);

	// TODO: Make knowledge matrix here.
}

void ClueSolver::AddHint(const Hint& hint)
{
	this->hintList.push_back(hint);
}

void ClueSolver::Reduce()
{
	// Visit each hint we have until we find one that we can integrate with the knowledge matrix.
	// If found, remove it, integrate it, then repeat.  (If the knowledge matrix changes, then we
	// may be able to integreate a hint that we could not have before.)

	// When examining a hint, we compare it with the knowledge matrix in an effort to reduce the hint.
	// If the hint is fully reduced, then we can integerate it with the matrix.
	// A hint of type AT_LEAST_ONE_OF can be reduced.  If it is reduced to one card, then it can be integreated.

	// When a hint is integrated with the matrix, one or more elements are changed.  We then try to reduce
	// all rows and columns.  We must repeat this until no row or column can be reduced.

	// If an element is changed to OWNED, then all other elements in the column must be NOT_OWNED.
	// If all elements in a column are NOT_OWNED except that one is UNKNOWN, then that UNKNOWN becomes OWNED.
	// If the number of UNKNOWN elements in a row equals the number of cards a card holder has minus the number of cards we know they have, then they become OWNED.

	// TODO: We must blow up here or assert or something if we get contradictory information.
}

bool ClueSolver::Solve(Clue::Accusation& correctAccusation) const
{
	// Return the correct accusation if we finally know what it is.  Note that we don't have to know where every card is to do this.

	return false;
}