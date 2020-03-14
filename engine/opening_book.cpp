#include "opening_book.hpp"

namespace engine {

    MMappedOpeningBook::MMappedOpeningBook(const fs::path& path) : mmap(path.string()) {}

    std::optional<chess::Move> MMappedOpeningBook::query(const chess::Board& board) {
        std::optional<chess::Move> best_move = std::nullopt;
        decltype(PolyglotEntry::weight) best_weight = 0;
        this->for_each(this->generate_key(board), [&](PolyglotEntry entry) {
            if (!best_move.has_value() || entry.weight > best_weight) {
                best_move = this->entry_to_move(board, entry);
                best_weight = entry.weight;
            }
        });
        return best_move;
    }

    size_t MMappedOpeningBook::size() const {
        return this->mmap.size() / sizeof(PolyglotEntry);
    }

    MMappedOpeningBook::Index MMappedOpeningBook::find_first_index(Key key) {
        Index lo = 0, hi = this->size();

        while (lo < hi) {
            Index mid = (lo + hi) / 2;  // TODO: replace with std::midpoint (C++ 20)
            Key mid_key = this->entry_at_index(mid).key;
            if (mid_key < key) lo = mid + 1;
            else hi = mid;
        }

        return lo;
    }


    PolyglotEntry MMappedOpeningBook::entry_at_index(MMappedOpeningBook::Index index) {
        return this->entry_at_key(this->index_to_key(index));
    }

    PolyglotEntry MMappedOpeningBook::entry_at_key(MMappedOpeningBook::Key key) {
        PolyglotEntry entry;
        entry.key = (std::uint64_t(this->mmap[key + 0]) << 56u) +
                    (std::uint64_t(this->mmap[key + 1]) << 48u) +
                    (std::uint64_t(this->mmap[key + 2]) << 40u) +
                    (std::uint64_t(this->mmap[key + 3]) << 32u) +
                    (std::uint64_t(this->mmap[key + 4]) << 24u) +
                    (std::uint64_t(this->mmap[key + 5]) << 16u) +
                    (std::uint64_t(this->mmap[key + 6]) <<  8u) +
                    (std::uint64_t(this->mmap[key + 7]) <<  0u);

        entry.move = (std::uint64_t(this->mmap[key + 8]) << 8u) +
                     (std::uint64_t(this->mmap[key + 9]) << 0u);

        entry.weight = (std::uint64_t(this->mmap[key + 10]) <<  8u) +
                       (std::uint64_t(this->mmap[key + 11]) <<  0u);

        entry.weight = (std::uint64_t(this->mmap[key + 12]) << 24u) +
                       (std::uint64_t(this->mmap[key + 13]) << 16u) +
                       (std::uint64_t(this->mmap[key + 14]) <<  8u) +
                       (std::uint64_t(this->mmap[key + 15]) <<  0u);

        return entry;
    }

    chess::Move MMappedOpeningBook::move_at_key(const chess::Board& board, Key key) {
        return this->entry_to_move(board, this->entry_at_key(key));
    }

    chess::Move MMappedOpeningBook::move_at_index(const chess::Board& board, Index index) {
        return this->move_at_key(board, this->index_to_key(index));
    }

    void MMappedOpeningBook::for_each(Key key, const std::function<void(PolyglotEntry)>& callback) {
        for (auto index = this->find_first_index(key); index < this->size(); ++index) {
            auto entry = this->entry_at_index(index);
            if (entry.key != key) break;
            callback(entry);
        }
    }

    chess::Move MMappedOpeningBook::entry_to_move(const chess::Board& board, const PolyglotEntry& entry) {
        chess::Square to_square(static_cast<chess::File>(entry.move_parts.to_file),
                                static_cast<chess::Rank>(entry.move_parts.to_row));
        chess::Square from_square(static_cast<chess::File>(entry.move_parts.from_file),
                                  static_cast<chess::Rank>(entry.move_parts.from_row));
        auto promotion = static_cast<chess::Piece::Type>(entry.move_parts.promotion_piece  + chess::Piece::PAWN);
        if (promotion == chess::Piece::PAWN) promotion = chess::Piece::NO_TYPE;
        auto flags = chess::Move::QUIET;

        // Castling is different
        if (from_square == chess::S::E1 && to_square == chess::S::H1 &&
                board.get_piece_at(from_square).type() == chess::Piece::KING) {
            to_square = chess::S::G1;
        } else if (from_square == chess::S::E1 && to_square == chess::S::A1 &&
                   board.get_piece_at(from_square).type() == chess::Piece::KING) {
            to_square = chess::S::C1;
        } else if (from_square == chess::S::E8 && to_square == chess::S::H8 &&
                   board.get_piece_at(from_square).type() == chess::Piece::KING) {
            to_square = chess::S::G8;
        } else if (from_square == chess::S::E8 && to_square == chess::S::A8 &&
                   board.get_piece_at(from_square).type() == chess::Piece::KING) {
            to_square = chess::S::C8;
        }

        return board.parse_move(from_square, to_square, promotion);
    }

    MMappedOpeningBook::Key MMappedOpeningBook::index_to_key(Index index) {
        return index * sizeof(PolyglotEntry);
    }

    MMappedOpeningBook::Key MMappedOpeningBook::generate_key(const chess::Board& board) {
        return this->zobrist_hasher(board);
    }


}