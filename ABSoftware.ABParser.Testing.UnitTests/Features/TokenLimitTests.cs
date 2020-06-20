using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Features
{
    [TestClass]
    public class TokenLimitTests : UnitTestBase
    {
        [TestMethod]
        [DataRow(new string[] { "A", "aBdc", "", "" }, "Trivia")]
        [DataRow(new string[] { "DOUBLE_QUOTE", "DOUBLE_QUOTE", "CAPITAL_B" }, "Tokens")]
        [DataRow(new int[] { 1, 6, 7 }, "TokenStarts")]
        [DataRow(new int[] { 1, 6, 7 }, "TokenEnds")]
        public void OneLevel_SingleStart_SingleEnd(object expected, string toTest) => RunQuoteLimit("A\"aBdc\"B").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "A!", "abc", "d", "", "deep", "est", "out", "", "g", "", " ", "?", "B" }, "Trivia")]
        [DataRow(new string[] { "<", "<<", "?", "<<<", "?", ">", ">>", "<<", "<<<", "!", "<", ">" }, "Tokens")]
        [DataRow(new int[] { 2, 6, 9, 10, 17, 21, 25, 27, 30, 33, 35, 37 }, "TokenStarts")]
        [DataRow(new int[] { 2, 7, 9, 12, 17, 21, 26, 28, 32, 33, 35, 37 }, "TokenEnds")]
        public void MultiLevel_VariedStartAndEnd(object expected, string toTest) => RunAngledLimit("A!<abc<<d?<<<deep?est>out>><<g<<<! <?>B").Test(toTest, expected);
    }
}
