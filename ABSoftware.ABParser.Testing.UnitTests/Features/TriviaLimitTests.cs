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
        [DataRow(new string[] { "h ", "jk", "l o" }, "Trivia")]
        [DataRow(new string[] { "A", "C" }, "Tokens")]
        [DataRow(new int[] { 2, 7 }, "TokenStarts")]
        [DataRow(new int[] { 2, 7 }, "TokenEnds")]
        public void OneLevel(object expected, string toTest) => RunTriviaLimit("h Aj \tkCl o").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "h ", "jk", "d", "lo", "l o" }, "Trivia")]
        [DataRow(new string[] { "A", "B", "C", "C" }, "Tokens")]
        [DataRow(new int[] { 2, 7, 11, 15 }, "TokenStarts")]
        [DataRow(new int[] { 2, 7, 11, 15 }, "TokenEnds")]
        public void MultiLevel(object expected, string toTest) => RunTriviaLimit("h Aj \tkBadcCl oCl o").Test(toTest, expected);
    }
}