using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace ABSoftware.ABParser.Testing.UnitTests.CoreParsing
{
    [TestClass]
    public class VerifyTests : UnitTestBase
    {
        [TestMethod]
        [DataRow(new string[] { "A", "B", "C", "arD", "Et", "F" }, "Trivia")]
        [DataRow(new string[] { "the", "they", "they", "theyare", "h" }, "Tokens")]
        [DataRow(new int[] { 1, 5, 10, 17, 26 }, "TokenStarts")]
        [DataRow(new int[] { 3, 8, 13, 23, 26 }, "TokenEnds")]
        public void SomeSingleChar(object expected, string toTest) => RunTheyWithSingleChar("AtheBtheyCtheyarDtheyareEthF").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "A", "B", "C", "arD", "E" }, "Trivia")]
        [DataRow(new string[] { "the", "they", "they", "theyare" }, "Tokens")]
        [DataRow(new int[] { 1, 5, 10, 17 }, "TokenStarts")]
        [DataRow(new int[] { 3, 8, 13, 23 }, "TokenEnds")]
        public void AllMultiChar_AllVerifyTypes_TriggersStartsWithToken(object expected, string toTest) => RunThey("AtheBtheyCtheyarDtheyareE").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "a", "bA", "B", "C", "arD", "c" }, "Trivia")]
        [DataRow(new string[] { "AtheBtheyCtheyarDtheyareE", "the", "they", "they", "theyare" }, "Tokens")]
        [DataRow(new int[] { 1, 28, 32, 37, 44 }, "TokenStarts")]
        [DataRow(new int[] { 25, 30, 35, 40, 50 }, "TokenEnds")]
        public void AllMultiChar_MultipleVerifyTokens(object expected, string toTest) => RunTheyWithLongToken("aAtheBtheyCtheyarDtheyareEbAtheBtheyCtheyarDtheyarec").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "A", "BtheyC", "Dt", "E" }, "Trivia")]
        [DataRow(new string[] { "theya", "thata", "hat" }, "Tokens")]
        [DataRow(new int[] { 1, 12, 19 }, "TokenStarts")]
        [DataRow(new int[] { 5, 16, 21 }, "TokenEnds")]
        public void AllMultiChar_AllVerifyTypes_TriggersInMiddle(object expected, string toTest) => RunTheyMiddle("AtheyaBtheyCthataDthatE").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "", "a" }, "Trivia")]
        [DataRow(new string[] { "they" }, "Tokens")]
        [DataRow(new int[] { 0 }, "TokenStarts")]
        [DataRow(new int[] { 3 }, "TokenEnds")]
        public void ImpossibleSituation(object expected, string toTest) => RunImpossibleVerify("theya").Test(toTest, expected);
    }
}
