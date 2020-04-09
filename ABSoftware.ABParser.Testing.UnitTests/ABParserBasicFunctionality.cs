using System;
using ABSoftware.ABParser.Testing.UnitTests.BasicFunctionality;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace ABSoftware.ABParser.Testing.UnitTests
{
    [TestClass]
    public class ABParserBasicFunctionality
    {
        ABParserSingleTokenParser RunSingleTokenParser = new ABParserSingleTokenParser();

        public ABParserSingleTokenParser RunSingleToken(string text)
        {
            RunSingleTokenParser.SetText(new ABParserText(text));
            RunSingleTokenParser.Start();
            return RunSingleTokenParser;
        }

        public ABParserSingleTokenParser RunSingleToken_NoText() => RunSingleToken("");
        public ABParserSingleTokenParser RunSingleToken_NoTokensInText() => RunSingleToken("aBCfDgh");
        public ABParserSingleTokenParser RunSingleToken_SingleChar() => RunSingleToken("aAb");
        public ABParserSingleTokenParser RunSingleToken_MultiChar() => RunSingleToken("aBCDb");

        #region Single Char
        [TestMethod]
        public void SingleToken_SingleChar_ValidLeadings() => RunSingleToken_SingleChar().TestLeadings(new string[] { "a" }, "b").Dispose();

        [TestMethod]
        public void SingleToken_SingleChar_ValidTrailings() => RunSingleToken_SingleChar().TestTrailings(new string[] { "b" }).Dispose();

        [TestMethod]
        public void SingleToken_SingleChar_ValidPreviousTokens() => RunSingleToken_SingleChar().TestPreviousTokens(new string[] { null }).Dispose();

        [TestMethod]
        public void SingleToken_SingleChar_ValidTokens() => RunSingleToken_SingleChar().TestTokens(new string[] { "A" }).Dispose();

        [TestMethod]
        public void SingleToken_SingleChar_ValidNextTokens() => RunSingleToken_SingleChar().TestNextTokens(new string[] { null }).Dispose();

        #endregion

        #region Multi Char

        [TestMethod]
        public void SingleToken_MultiChar_ValidLeadings() => RunSingleToken_MultiChar().TestLeadings(new string[] { "a" }, "b").Dispose();

        [TestMethod]
        public void SingleToken_MultiChar_ValidTrailings() => RunSingleToken_MultiChar().TestTrailings(new string[] { "b" }).Dispose();

        [TestMethod]
        public void SingleToken_MultiChar_ValidPreviousTokens() => RunSingleToken_MultiChar().TestPreviousTokens(new string[] { null }).Dispose();

        [TestMethod]
        public void SingleToken_MultiChar_ValidTokens() => RunSingleToken_MultiChar().TestTokens(new string[] { "BCD" }).Dispose();

        [TestMethod]
        public void SingleToken_MultiChar_ValidNextTokens() => RunSingleToken_MultiChar().TestNextTokens(new string[] { null }).Dispose();

        #endregion

        #region No Text

        [TestMethod]
        public void NoText_ValidLeadings() => RunSingleToken_NoText().TestLeadings(new string[] { }, "").Dispose();

        [TestMethod]
        public void NoText_ValidTrailings() => RunSingleToken_NoText().TestTrailings(new string[] { }).Dispose();

        [TestMethod]
        public void NoText_ValidPreviousTokens() => RunSingleToken_NoText().TestPreviousTokens(new string[] { }).Dispose();

        [TestMethod]
        public void NoText_ValidTokens() => RunSingleToken_NoText().TestTokens(new string[] { }).Dispose();

        [TestMethod]
        public void NoText_ValidNextTokens() => RunSingleToken_NoText().TestNextTokens(new string[] { }).Dispose();

        #endregion

        #region No Tokens In Text

        [TestMethod]
        public void NoTokensInText_ValidLeadings() => RunSingleToken_NoTokensInText().TestLeadings(new string[] { }, "aBCfDgh").Dispose();

        [TestMethod]
        public void NoTokensInText_ValidTrailings() => RunSingleToken_NoTokensInText().TestTrailings(new string[] { }).Dispose();

        [TestMethod]
        public void NoTokensInText_ValidPreviousTokens() => RunSingleToken_NoTokensInText().TestPreviousTokens(new string[] { }).Dispose();

        [TestMethod]
        public void NoTokensInText_ValidTokens() => RunSingleToken_NoTokensInText().TestTokens(new string[] { }).Dispose();

        [TestMethod]
        public void NoTokensInText_ValidNextTokens() => RunSingleToken_NoTokensInText().TestNextTokens(new string[] { }).Dispose();

        #endregion
    }
}
