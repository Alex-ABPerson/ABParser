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
        [DataRow("Leadings", new string[] { "h ", "jk" }, "l o")]
        [DataRow("Trailings", new string[] { "jk", "l o" }, "")]
        [DataRow("Tokens", new string[] { "A", "C" }, "")]
        public void OneLevel(string toTest, string[] expected, string leadingEndExpected) => RunTriviaLimit("h Aj \tkCl o").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { "h ", "jk", "d", "lo" }, "l o")]
        [DataRow("Trailings", new string[] { "jk", "d", "lo", "l o" }, "")]
        [DataRow("Tokens", new string[] { "A", "B", "C", "C" }, "")]
        public void MultiLevel(string toTest, string[] expected, string leadingEndExpected) => RunTriviaLimit("h Aj \tkBadcCl oCl o").Test(toTest, expected, leadingEndExpected);
    }
}
