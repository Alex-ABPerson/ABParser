using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Features
{
    [TestClass]
    public class TriviaLimitTests : UnitTestBase
    {
        [TestMethod]
        [DataRow(new string[] { "", "h ", "jk", "l o" }, "Trivia")]
        [DataRow(new string[] { "C", "A", "C" }, "Tokens")]
        [DataRow(new int[] { 0, 3, 8 }, "TokenStarts")]
        [DataRow(new int[] { 0, 3, 8 }, "TokenEnds")]
        public void Blacklist_OneLevel(object expected, string toTest) => RunBlacklistTriviaLimit("Ch Aj \tkCl o").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "", "h ", "jk", "d", "lo", "l o" }, "Trivia")]
        [DataRow(new string[] { "C", "A", "B", "C", "C" }, "Tokens")]
        [DataRow(new int[] { 0, 3, 8, 12, 16 }, "TokenStarts")]
        [DataRow(new int[] { 0, 3, 8, 12, 16 }, "TokenEnds")]
        public void Blacklist_MultiLevel(object expected, string toTest) => RunBlacklistTriviaLimit("Ch Aj \tkBadcCl oCl o").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "h ", "cba", "l o" }, "Trivia")]
        [DataRow(new string[] { "A", "C" }, "Tokens")]
        [DataRow(new int[] { 2, 8 }, "TokenStarts")]
        [DataRow(new int[] { 2, 8 }, "TokenEnds")]
        public void Whitelist(object expected, string toTest) => RunWhitelistTriviaLimit("h AcjbkaCl o").Test(toTest, expected);
    }
}