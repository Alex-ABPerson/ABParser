using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.CoreParsing
{
    [TestClass]
    public class UniqueScenarioTests : UnitTestBase
    {
        [TestMethod]
        [DataRow("Leadings", new string[] { }, "")]
        [DataRow("Trailings", new string[] { }, "")]
        [DataRow("PreviousTokens", new string[] { }, "")]
        [DataRow("Tokens", new string[] { }, "")]
        [DataRow("NextTokens", new string[] { }, "")]
        public void NoText(string toTest, string[] expected, string leadingEndExpected) => RunSingleToken("").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { }, "aBCfDgh")]
        [DataRow("Trailings", new string[] { }, "")]
        [DataRow("PreviousTokens", new string[] { }, "")]
        [DataRow("Tokens", new string[] { }, "")]
        [DataRow("NextTokens", new string[] { }, "")]
        public void NoTokensInText(string toTest, string[] expected, string leadingEndExpected) => RunSingleToken("aBCfDgh").Test(toTest, expected, leadingEndExpected);

        [TestMethod]
        [DataRow("Leadings", new string[] { }, "ABDOKah\\alh26-0")]
        [DataRow("Trailings", new string[] { }, "")]
        [DataRow("PreviousTokens", new string[] { }, "")]
        [DataRow("Tokens", new string[] { }, "")]
        [DataRow("NextTokens", new string[] { }, "")]
        public void NoTokens_RandomText(string toTest, string[] expected, string leadingEndExpected) => RunNoTokens("ABDOKah\\alh26-0").Test(toTest, expected, leadingEndExpected);

        #region No Tokens, Random Text

        #endregion
    }
}
