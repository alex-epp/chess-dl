#include "stdafx.h"
#include "CppUnitTest.h"

#include <cstdint>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../CppChessLib/mailbox.hpp"

using namespace chess;

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			inline std::wstring ToString(const chess::Mailbox& mailbox) {
				std::wostringstream oss;
				for (int ri = 7; ri >= 0; --ri) {
					for (int fi = 0; fi < 8; ++fi) {
						auto r = static_cast<Rank>(ri);
						auto f = static_cast<File>(fi);
						auto piece = mailbox.get(PositionIndex(f, r));

						oss << piece_repr(piece);
					}
					oss << '\n';
				}
				return oss.str();
			}
		}
	}
}

namespace tests
{
	TEST_CLASS(TestMailbox)
	{
	public:
		TEST_METHOD(TestFlipVertical)
		{
			Mailbox mb;
			mb.set(PI::B3, Piece(Piece::ROOK, Piece::BLACK));

			Mailbox mb_flip_2 = mb;
			mb_flip_2.flip();
			mb_flip_2.flip();
			Assert::AreEqual(mb, mb_flip_2);

			Mailbox mb_flip = mb;
			mb_flip.flip();
			Mailbox mb_flip_expected;
			mb_flip_expected.set(PI::B6, Piece(Piece::ROOK, Piece::WHITE));

			Assert::AreEqual(mb_flip_expected, mb_flip);
		}
	};
}