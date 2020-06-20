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
        [DataRow(new string[] { "abcd", "efg", "hij" }, "Trivia")]
        [DataRow(new string[] { "ABC", "DEF" }, "Tokens")]
        [DataRow(new int[] { 4, 10 }, "TokenStarts")]
        [DataRow(new int[] { 6, 12 }, "TokenEnds")]
        public void NormalUsage(object expected, string toTest) => RunDetectionLimit("abcdABCefgDEFhij").Test(toTest, expected);

        [TestMethod]
        [DataRow(new string[] { "abcd", "efg", "hij" }, "Trivia")]
        [DataRow(new string[] { "ABC", "DEF" }, "Tokens")]
        [DataRow(new int[] { 4, 11 }, "TokenStarts")]
        [DataRow(new int[] { 7, 17 }, "TokenEnds")]
        public void CharactersWithinToken(object expected, string toTest) => RunDetectionLimit("abcdAdBCefgD\tE \r Fhij").Test(toTest, expected);
        
    }
}
