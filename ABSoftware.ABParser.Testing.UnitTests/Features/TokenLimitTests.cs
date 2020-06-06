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
        [DataRow("Leadings", new string[] { "A", "aBdc", "" }, "")]
        [DataRow("Trailings", new string[] { "aBdc", "", "" }, "")]
        [DataRow("Tokens", new string[] { "DOUBLE_QUOTE", "DOUBLE_QUOTE", "CAPITAL_B" }, "")]
        public void OneLevel_SingleStart_SingleEnd(string toTest, string[] expected, string leadingEndExpected) => RunQuoteLimit("A\"aBdc\"B").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { "A!", "abc", "d", "", "deep", "est", "out", "", "g", "", " ", "?" }, "B")]
        [DataRow("Trailings", new string[] { "abc", "d", "", "deep", "est", "out", "", "g", "", " ", "?", "B" }, "")]
        [DataRow("Tokens", new string[] { "<", "<<", "?", "<<<", "?", ">", ">>", "<<", "<<<", "!", "<", ">" }, "")]
        public void MultiLevel_VariedStartAndEnd(string toTest, string[] expected, string leadingEndExpected) => RunAngledLimit("A!<abc<<d?<<<deep?est>out>><<g<<<! <?>B").Test(toTest, expected, leadingEndExpected);
    }
}
