#include "bitboard.hpp"
#include "position_index.hpp"


namespace chess {
	BitBoardBitScanIterator::BitBoardBitScanIterator(BitBoard* bb) : bb(bb), position(bitscan::first(bb->bb)) {}
	BitBoardBitScanIterator& BitBoardBitScanIterator::operator ++ () {
		this->position = bitscan::next(this->position.position_index, this->bb->bb);
		return *this;
	}
	bool BitBoardBitScanIterator::operator != (const unsigned int rhs) const {
		return this->position.position_index != rhs;
	}
	PositionIndex BitBoardBitScanIterator::operator * () const {
		return this->position;
	}
}
