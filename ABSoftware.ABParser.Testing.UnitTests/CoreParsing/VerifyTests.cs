using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace ABSoftware.ABParser.Testing.UnitTests.CoreParsing
{
    [TestClass]
    public class VerifyTests : UnitTestBase
    {
        [TestMethod]
        [DataRow("Leadings", new string[] { "A", "B", "C", "arD", "Et" }, "F")]
        [DataRow("Tokens", new string[] { "the", "they", "they", "theyare", "h" }, "")]
        public void SomeSingleChar(string toTest, string[] expected, string leadingEndExpected) => RunTheyWithSingleChar("AtheBtheyCtheyarDtheyareEthF").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { "A", "B", "C", "arD" }, "E")]
        [DataRow("Tokens", new string[] { "the", "they", "they", "theyare" }, "")]
        public void AllMultiChar_AllVerifyTypes_TriggersStartsWithToken(string toTest, string[] expected, string leadingEndExpected) => RunThey("AtheBtheyCtheyarDtheyareE").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { "a", "bA", "B", "C", "arD" }, "c")]
        [DataRow("Tokens", new string[] { "AtheBtheyCtheyarDtheyareE", "the", "they", "they", "theyare" }, "")]
        public void AllMultiChar_MultipleVerifyTokens(string toTest, string[] expected, string leadingEndExpected) => RunTheyWithLongToken("aAtheBtheyCtheyarDtheyareEbAtheBtheyCtheyarDtheyarec").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { "A", "BtheyC", "Dt" }, "E")]
        [DataRow("Tokens", new string[] { "theya", "thata", "hat" }, "")]
        public void AllMultiChar_AllVerifyTypes_TriggersInMiddle(string toTest, string[] expected, string leadingEndExpected) => RunTheyMiddle("AtheyaBtheyCthataDthatE").Test(toTest, expected, leadingEndExpected);
    }
}
