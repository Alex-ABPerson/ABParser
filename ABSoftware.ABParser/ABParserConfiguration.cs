using ABSoftware.ABParser.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser
{
    /// <summary>
    /// Represents an array of ABParser Tokens.
    /// </summary>
    public class ABParserConfiguration
    {
        // The storage space for tokens on the C++ side, this is passed onto any instance of the parser.
        internal IntPtr TokensStorage;

        public ABParserToken[] Tokens;
        public int Length => Tokens.Length;

        internal ABParserConfiguration() { }

        public static ABParserConfiguration Create(ABParserToken[] tokens)
        {
            var tokensArray = new ABParserConfiguration()
            {
                Tokens = tokens
            };

            var arrayOfTokenData = new string[tokens.Length];
            var arrayOfTokenLengths = new int[tokens.Length];

            for (int i = 0; i < tokens.Length; i++)
            {
                arrayOfTokenData[i] = tokens[i].TokenData.AsString();
                arrayOfTokenLengths[i] = tokens[i].TokenData.GetLength();
            }

            tokensArray.TokensStorage = NativeMethods.InitializeTokens(arrayOfTokenData, arrayOfTokenLengths, tokens.Length);
            return tokensArray;
        }
    }
}
