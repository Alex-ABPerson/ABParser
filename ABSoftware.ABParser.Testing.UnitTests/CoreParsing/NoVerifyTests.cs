using System;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace ABSoftware.ABParser.Testing.UnitTests.CoreParsing
{
    [TestClass]
    public class NoVerifyTests : UnitTestBase
    {

        [TestMethod]
        [DataRow(new string[] { "a", "b" }, "Trivia")]
        [DataRow(new string[] { "A" }, "Tokens")]
        [DataRow(new int[] { 1 }, "TokenStarts")]
        [DataRow(new int[] { 1 }, "TokenEnds")]
        public void SingleCharToken(object expected, string toTest) => RunSingleToken("aAb").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "a", "b" }, "Trivia")]
        [DataRow(new string[] { "BCD" }, "Tokens")]
        [DataRow(new int[] { 1 }, "TokenStarts")]
        [DataRow(new int[] { 3 }, "TokenEnds")]
        public void MultiCharToken(object expected, string toTest) => RunSingleToken("aBCDb").Test(toTest, expected);
       
    }
}
