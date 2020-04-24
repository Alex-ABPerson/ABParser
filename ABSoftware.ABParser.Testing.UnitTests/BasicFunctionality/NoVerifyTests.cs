using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace ABSoftware.ABParser.Testing.UnitTests.BasicFunctionality
{
    [TestClass]
    public class NoVerifyTests : UnitTestBase
    {

        [TestMethod]
        [DataRow("Leadings", new string[] { "a" }, "b")]
        [DataRow("Trailings", new string[] { "b" }, "")]
        [DataRow("PreviousTokens", new string[] { null }, "")]
        [DataRow("Tokens", new string[] { "A" }, "")]
        [DataRow("NextTokens", new string[] { null }, "")]
        public void SingleCharToken(string toTest, string[] expected, string leadingEndExpected) => RunSingleToken("aAb").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { "a" }, "b")]
        [DataRow("Trailings", new string[] { "b" }, "")]
        [DataRow("PreviousTokens", new string[] { null }, "")]
        [DataRow("Tokens", new string[] { "BCD" }, "")]
        [DataRow("NextTokens", new string[] { null }, "")]
        public void MultiCharToken(string toTest, string[] expected, string leadingEndExpected) => RunSingleToken("aBCDb").Test(toTest, expected, leadingEndExpected);
       
    }
}
