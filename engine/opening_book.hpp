#pragma once

#include "../backend/chess.hpp"
#include "../backend/zobrist_hash.hpp"

#include <optional>
#include <mio/mmap.hpp>
#include <filesystem>

namespace engine {
    namespace fs = std::filesystem;

    class OpeningBook {
    public:
        OpeningBook() = default;
        virtual ~OpeningBook() = default;

        [[nodiscard]] virtual std::optional<chess::Move> query(const chess::Board& board) = 0;
    };

    struct PolyglotEntry {
        struct move_parts_t {
            std::uint16_t to_file: 3;
            std::uint16_t to_row: 3;
            std::uint16_t from_file: 3;
            std::uint16_t from_row: 3;
            std::uint16_t promotion_piece: 3;
        };

        std::uint64_t key;  // 8 bytes
        union {  // 2 bytes
            std::uint16_t move;
            move_parts_t move_parts;
        };
        std::uint16_t weight;  // 2 bytes
        std::uint32_t learn;   // 4 bytes
    };
    static_assert(sizeof(PolyglotEntry) == 16);

    class MMappedOpeningBook : public OpeningBook {
    public:
        explicit MMappedOpeningBook(const fs::path& path);
        ~MMappedOpeningBook() override = default;

        [[nodiscard]] std::optional<chess::Move> query(const chess::Board& board) override;
    private:
        using Index = std::size_t;
        using Key = chess::ZobristHasher::Hash;

        [[nodiscard]] size_t size() const;
        Index find_first_index(Key key);

        PolyglotEntry entry_at_key(Key key);
        PolyglotEntry entry_at_index(Index index);

        chess::Move move_at_key(const chess::Board& board, Key key);
        chess::Move move_at_index(const chess::Board& board, Index index);

        Key index_to_key(Index index);

        void for_each(Key key, const std::function<void(PolyglotEntry)>& callback);

        chess::Move entry_to_move(const chess::Board& board, const PolyglotEntry& entry);

        Key generate_key(const chess::Board& board);

        chess::ZobristHasher zobrist_hasher;
        mio::basic_mmap_source<std::byte> mmap;
    };

}