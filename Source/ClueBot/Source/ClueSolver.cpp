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
	if (this->knowledgeMatrix)
	{
		int numRows = (int)this->cardHolderArray.size();
		for (int row = 0; row < numRows; row++)
			delete[] this->knowledgeMatrix[row];
		delete[] this->knowledgeMatrix;
		this->knowledgeMatrix = nullptr;
	}

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

	int numCols = (int)this->cardArray.size();
	int numRows = (int)this->cardHolderArray.size();

	this->knowledgeMatrix = new KnowledgeElement*[numRows];
	for (int row = 0; row < numRows; row++)
	{
		this->knowledgeMatrix[row] = new KnowledgeElement[numCols];
		for (int col = 0; col < numCols; col++)
		{
			this->knowledgeMatrix[row][col] = KnowledgeElement::OWNER_UNKNOWN;
		}
	}
}

void ClueSolver::AddHint(std::shared_ptr<Hint> hint)
{
	this->hintList.push_back(hint);
}

void ClueSolver::Reduce()
{
	// TODO: We must blow up here or assert or something if we get contradictory information.
	//       We should check the sanity of the knowledge matrix after every hint application.

	std::list<std::shared_ptr<Hint>>::iterator iter = this->hintList.begin();
	while (iter != this->hintList.end())
	{
		std::list<std::shared_ptr<Hint>>::iterator nextIter = iter;
		nextIter++;

		std::shared_ptr<Hint> hint = *iter;
			
		uint32_t numChanges = 0;
		if (hint->Apply(this, numChanges))
			this->hintList.erase(iter);

		if (numChanges > 0)
		{
			this->ReduceMatrix();
			nextIter = this->hintList.begin();
		}

		iter = nextIter;
	}
}

void ClueSolver::ReduceMatrix()
{
	int numCols = (int)this->cardArray.size();
	int numRows = (int)this->cardHolderArray.size();
	int numChanges = 0;

	do
	{
		numChanges = 0;

		// In a column, if we find an owned element, then all other elements in the collumn are not owned.
		for (int col = 0; col < numCols; col++)
		{
			int rowOwned = -1;
			for (int row = 0; row < numRows && rowOwned == -1; row++)
				if (this->knowledgeMatrix[row][col] == KnowledgeElement::OWNED)
					rowOwned = row;

			if (rowOwned != -1)
			{
				for (int row = 0; row < numRows; row++)
				{
					if (row != rowOwned && this->knowledgeMatrix[row][col] != KnowledgeElement::NOT_OWNED)
					{
						this->knowledgeMatrix[row][col] = KnowledgeElement::NOT_OWNED;
						numChanges++;
					}
				}
			}
		}

		// In a column, if all elements are not owned except for one being unknown, then that unknown becomes owned.
		for (int col = 0; col < numCols; col++)
		{
			int rowUnknown = -1;
			int numNotOwned = 0;

			for (int row = 0; row < numRows; row++)
			{
				if (this->knowledgeMatrix[row][col] == KnowledgeElement::NOT_OWNED)
					numNotOwned++;

				if (this->knowledgeMatrix[row][col] == KnowledgeElement::OWNER_UNKNOWN)
					rowUnknown = row;
			}

			if (rowUnknown != -1 && numNotOwned == numRows - 1)
			{
				this->knowledgeMatrix[rowUnknown][col] = KnowledgeElement::OWNED;
				numChanges++;
			}
		}

		// In a row, if the number of unknown elements is the number of remaining unknown cards for a card holder, then they all become owned.
		for (int row = 0; row < numRows; row++)
		{
			int numOwned = 0;
			int numUnknown = 0;
			for (int col = 0; col < numCols; col++)
			{
				if (this->knowledgeMatrix[row][col] == KnowledgeElement::OWNED)
					numOwned++;

				if (this->knowledgeMatrix[row][col] == KnowledgeElement::OWNER_UNKNOWN)
					numUnknown++;
			}

			if (numUnknown == this->cardHolderArray[row].numCards - numOwned)
			{
				for (int col = 0; col < numCols; col++)
				{
					if (this->knowledgeMatrix[row][col] == KnowledgeElement::OWNER_UNKNOWN)
					{
						this->knowledgeMatrix[row][col] = KnowledgeElement::OWNED;
						numChanges++;
					}
				}
			}
		}

		// In a row, if the number of owned elements is the number of cards the holder has, then the rest become not owned.
		for (int row = 0; row < numRows; row++)
		{
			int numOwned = 0;
			for (int col = 0; col < numCols; col++)
				if (this->knowledgeMatrix[row][col] == KnowledgeElement::OWNED)
					numOwned++;

			if (numOwned == this->cardHolderArray[row].numCards)
			{
				for (int col = 0; col < numCols; col++)
				{
					if (this->knowledgeMatrix[row][col] == KnowledgeElement::OWNER_UNKNOWN)
					{
						this->knowledgeMatrix[row][col] = KnowledgeElement::NOT_OWNED;
						numChanges++;
					}
				}
			}
		}
	} while (numChanges > 0);
}

bool ClueSolver::Solve(Clue::Accusation& correctAccusation) const
{
	// Return the correct accusation if we finally know what it is.  Note that we don't have to know where every card is to do this.

	return false;
}

Clue::Accusation ClueSolver::GetRecommendedAccusation() const
{
	Clue::Accusation accusation{};

	return accusation;
}

int ClueSolver::GetColumnForCard(const Clue::Card& card) const
{
	int numCols = (int)this->cardArray.size();
	for (int col = 0; col < numCols; col++)
		if (this->cardArray[col] == card)
			return col;

	return -1;
}

int ClueSolver::GetRowForCharacter(Clue::Character character) const
{
	int numRows = (int)this->cardHolderArray.size();
	for (int row = 0; row < numRows; row++)
		if (this->cardHolderArray[row].character == character)
			return row;

	return -1;
}

//-------------------------------------------- ClueSolver::Hint --------------------------------------------

ClueSolver::Hint::Hint(Character character)
{
	this->character = character;
}

//-------------------------------------------- ClueSolver::HasNoneOfHint --------------------------------------------

ClueSolver::HasNoneOfHint::HasNoneOfHint(Character character) : Hint(character)
{
}

/*virtual*/ bool ClueSolver::HasNoneOfHint::Apply(ClueSolver* clueSolver, uint32_t& numChanges)
{
	numChanges = 0;

	int row = clueSolver->GetRowForCharacter(this->character);

	for (const Card& card : this->cardArray)
	{
		int col = clueSolver->GetColumnForCard(card);

		if (clueSolver->knowledgeMatrix[row][col] != KnowledgeElement::NOT_OWNED)
		{
			clueSolver->knowledgeMatrix[row][col] = KnowledgeElement::NOT_OWNED;
			numChanges++;
		}
	}

	return true;
}

//-------------------------------------------- ClueSolver::HasAtLeastOneOfHint --------------------------------------------

ClueSolver::HasAtLeastOneOfHint::HasAtLeastOneOfHint(Character character) : Hint(character)
{
}

/*virtual*/ bool ClueSolver::HasAtLeastOneOfHint::Apply(ClueSolver* clueSolver, uint32_t& numChanges)
{
	int row = clueSolver->GetRowForCharacter(this->character);

	uint32_t numUnknowns = 0;
	uint32_t numOwnedByOthers = 0;
	uint32_t numOwnedByMe = 0;

	for (const Clue::Card& card : this->cardArray)
	{
		int col = clueSolver->GetColumnForCard(card);

		switch (clueSolver->knowledgeMatrix[row][col])
		{
			case KnowledgeElement::OWNED:			numOwnedByMe++;		break;
			case KnowledgeElement::NOT_OWNED:		numOwnedByOthers++;	break;
			case KnowledgeElement::OWNER_UNKNOWN:	numUnknowns++;		break;
		}
	}

	if (numUnknowns == 1 && numOwnedByOthers == int(this->cardArray.size()) - 1)
	{
		for (const Clue::Card& card : this->cardArray)
		{
			int col = clueSolver->GetColumnForCard(card);

			if (clueSolver->knowledgeMatrix[row][col] == KnowledgeElement::OWNER_UNKNOWN)
			{
				clueSolver->knowledgeMatrix[row][col] = KnowledgeElement::OWNED;
				numChanges++;
				break;
			}
		}

		numOwnedByMe++;
	}

	return numOwnedByMe > 0;
}