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
    public class ABParserTokensArray
    {

        // This prevents tokens from being processed/sorted into single-character and multi-character multiple times.
        internal IntPtr SingleCharTokensPointer;
        internal int SingleCharTokensLength;

        internal IntPtr MultiCharTokensPointer;
        internal int MultiCharTokensLength;

        public ABParserToken[] Tokens;

        public int Length => Tokens.Length;

        internal ABParserTokensArray() { }

        public static ABParserTokensArray GenerateTokensArray(ABParserToken[] tokens)
        {
            // Create the new "tokensArray", with the simplest field - "Tokens" set.
            var tokensArray = new ABParserTokensArray()
            {
                Tokens = tokens
            };

            // Organize those tokens into two arrays - a length array and a string array.
            var arrayOfTokenData = new string[tokens.Length];
            var arrayOfTokenLengths = new int[tokens.Length];

            for (int i = 0; i < tokens.Length; i++)
            {
                arrayOfTokenData[i] = tokens[i].TokenData.AsString();
                arrayOfTokenLengths[i] = tokens[i].TokenData.GetLength();
            }

            // Then, get this data processed.
            NativeMethods.InitializeTokens(arrayOfTokenData, arrayOfTokenLengths, tokens.Length, ref tokensArray.SingleCharTokensPointer, ref tokensArray.MultiCharTokensPointer, ref tokensArray.SingleCharTokensLength, ref tokensArray.MultiCharTokensLength);

            // Finally, return our newly generated "tokensArray".
            return tokensArray;
        }
    }
}
