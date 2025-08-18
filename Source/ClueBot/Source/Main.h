#pragma once

// Your memory is in two parts A and B...
// 
// A: For each card you track the set of possible owners.  (If a singleton, you know the owner.)
//
// B: You also track all refutes.  (e.g., Sally disproved an accusation.  We're not told how, but we know she did.)
//
// Anytime the state of A changes, you can reduce B.
// Reducing B can change the state of A.
//
// When a failure to refute occurs, you can update A.
//
// A refute is reduced when we know that the disprover doesn't have a card in the disproved accusation.
// When a refute reduces to one card, then we know the disprover has that card.
//
// And of course, we can change A whenever we make an accusation and either receive a refute of it or someone fails to refute it.
//
// A note about updating A: We also know the total number of cards in everyone's hand, and that never changes.
// Therefore, if the total number of remaining unknown cards in a person's hand equals the total number of possible
// cards the person is assigned to, then we can deduce that they have those cards.
//
// ------------------
//
// The above accounts for everything out of our control.  Here we must consider the decisions
// we make.  For example, what room do we go to next?  What accusations to we make?  Note that
// making certain accusations where elements of it are known to be in our own hand is very,
// very useful, because it forces the types of refutes we can get from other players.
//
// Also, if we are asked to refute, and we can, what card do we show?  I think it best to never
// show a room card if we can help it, because these are the hardest to eliminate due to the
// need to travel to a room before it can be used in an accusation.  Also, never volunteer new
// information if we don't have to.  If the accuser can be shown the same card again, do it.