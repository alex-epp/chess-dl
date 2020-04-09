#include <chess/backend/bitboard.hpp>
#include <chess/backend/square.hpp>


namespace chess {
	BitBoardBitScanIterator::BitBoardBitScanIterator(const BitBoard* bb) : bb(bb), square(bitscan::first(bb->bb)) {}
	BitBoardBitScanIterator& BitBoardBitScanIterator::operator ++ () {
		this->square = bitscan::next(this->square.get(), this->bb->bb);
		return *this;
	}
	bool BitBoardBitScanIterator::operator != (unsigned int rhs) const {
		return this->square.get() != rhs;
	}
	Square BitBoardBitScanIterator::operator * () const {
		return this->square;
	}
}
