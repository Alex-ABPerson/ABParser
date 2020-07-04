using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Text;

namespace ABSoftware.ABParser.Testing.UnitTests.Features
{
    [TestClass]
    public class OnFirstUnlimitedCharacterProcessedTests : UnitTestBase
    {
        [TestMethod]
        [DataRow(0, "OFUCP")]
        public void WithoutLimit(object expected, string toTest) => RunThey("AtheB").Test(toTest, expected);

        [TestMethod]
        [DataRow(2, "OFUCP")]
        public void WithLimit(object expected, string toTest) => RunBlacklistTriviaLimit("  AtheB").Test(toTest, expected);
    }
}
