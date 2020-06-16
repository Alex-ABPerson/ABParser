using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Features
{
    [TestClass]
    public class DetectionLimitTests : UnitTestBase
    {
        [TestMethod]
        [DataRow("Leadings", new string[] { "abcd", "efg" }, "hij")]
        [DataRow("Tokens", new string[] { "ABC", "DEF" }, "")]
        public void NormalUsage(string toTest, string[] expected, string leadingEndExpected) => RunDetectionLimit("abcdABCefgDEFhij").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { "abcd", "efg" }, "hij")]
        [DataRow("Tokens", new string[] { "ABC", "DEF" }, "")]
        public void CharactersWithinToken(string toTest, string[] expected, string leadingEndExpected) => RunDetectionLimit("abcdAdBCefgD\tE \r Fhij").Test(toTest, expected, leadingEndExpected);
    }
}
