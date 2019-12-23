#pragma once


#include "../CppChessLib/chess.h"

#include <string>
#include <sstream>


//namespace Microsoft {
//	namespace VisualStudio {
//		namespace CppUnitTestFramework {
//			template <>
//			inline std::wstring ToString(const chess::BitBoard& bb) {
//				std::wostringstream oss;
//				chess::print_bitboard(oss, bb);
//				return oss.str();
//			}
//
//			template<>
//			inline std::wstring ToString(const chess::Piece::Type& piece) {
//				return piece_repr(piece).data();
//			}
//
//			template<>
//			inline std::wstring ToString(const chess::PositionIndex& position) {
//				return ToString(position.get());
//			}
//
//			template<>
//			inline std::wstring ToString(const chess::Mailbox& mailbox) {
//				std::wostringstream oss;
//				for (int ri = 7; ri >= 0; --ri) {
//					for (int fi = 0; fi < 8; ++fi) {
//						auto r = static_cast<chess::Rank>(ri);
//						auto f = static_cast<chess::File>(fi);
//						auto piece = mailbox.get(chess::PositionIndex::from_file_rank(f, r));
//						oss << piece_repr(piece);
//					}
//					oss << '\n';
//				}
//				return oss.str();
//			}
//		}
//	}
//}