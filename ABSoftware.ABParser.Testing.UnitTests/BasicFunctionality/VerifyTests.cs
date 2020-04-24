using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace ABSoftware.ABParser.Testing.UnitTests.BasicFunctionality
{
    [TestClass]
    public class VerifyTests : UnitTestBase
    {


        #region They Parser, They Text

        [TestMethod]
        [DataRow("Leadings", new string[] { "A", "B", "C", "arD" }, "E")]
        [DataRow("Trailings", new string[] { "B", "C", "arD", "E" }, "")]
        [DataRow("PreviousTokens", new string[] { null, "the", "they", "they" }, "")]
        [DataRow("Tokens", new string[] { "the", "they", "they", "theyare" }, "")]
        [DataRow("NextTokens", new string[] { "they", "they", "theyare", null }, "")]
        public void AllMultiChar_AllVerifyTypes(string toTest, string[] expected, string leadingEndExpected) => RunThey("AtheBtheyCtheyarDtheyareE").Test(toTest, expected, leadingEndExpected);

        #endregion
    }
}
