#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../CppChessLib/chess.h"

using namespace chess;


namespace tests
{		
	TEST_CLASS(TestMove)
	{
	public:
		
		TEST_METHOD(TestConstructor)
		{
			auto from = PosIndex::from_uci("E2"), to = PosIndex::from_uci("E4");
			auto flags = Move::D_P_PUSH;
			auto m = Move(from, to, flags);
			Assert::AreEqual(from, m.from());
			Assert::AreEqual(to, m.to());
			Assert::AreEqual(flags, m.flags());
		}

	};
}