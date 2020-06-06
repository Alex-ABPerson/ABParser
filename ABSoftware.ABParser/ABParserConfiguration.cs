using ABSoftware.ABParser.Exceptions;
using ABSoftware.ABParser.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.XPath;

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

        public static ABParserConfiguration Create(ABParserToken[] tokens)
        {
            if (tokens.Length > ushort.MaxValue) throw new ABParserTooManyTokens();

            var tokensArray = new ABParserConfiguration()
            {
                Tokens = tokens
            };

            var tokenData = new string[tokens.Length];
            var tokenDataLengths = new ushort[tokens.Length];
            var limitsPerToken = new ushort[tokens.Length];
            var limitNames = new List<string>();

            for (int i = 0; i < tokens.Length; i++)
            {
                tokenData[i] = tokens[i].TokenData.AsString();
                tokenDataLengths[i] = (ushort)tokens[i].TokenData.GetLength();

                limitNames.AddRange(tokens[i].TokenLimitsToAddTo);
                limitsPerToken[i] = (ushort)tokens[i].TokenLimitsToAddTo.Count;
            }

            var limitNameSizes = new byte[limitNames.Count];
            for (int i = 0; i < limitNames.Count; i++)
                limitNameSizes[i] = (byte)limitNames[i].Length;

            tokensArray.TokensStorage = NativeMethods.InitializeTokens(tokenData, tokenDataLengths, (ushort)tokens.Length, limitNames.ToArray(), limitNameSizes, limitsPerToken);
            return tokensArray;
        }

        //static void ProcessTokenLimit(ABParserInternalToken token, int tokenIndex, ABParserConfiguration config)
        //{
        //    var tokenLimitsSize = token.TokenLimitsToAddTo.Count;
        //    if (tokenLimitsSize == 0) return;

        //    for (int i = 0; i < tokenLimitsSize; i++)
        //    {
        //        var thisItem = token.TokenLimitsToAddTo[i];
        //        var limitIndex = config.TokenLimits.FindIndex(l => l.Name == thisItem);

        //        if (limitIndex == -1)
        //        {
        //            config.TokenLimits.Add(new ABParserConfigurationTokenLimit(thisItem));
        //            limitIndex = config.TokenLimits.Count;
        //        }

        //        config.TokenLimits[limitIndex].Tokens.Add(tokenIndex);
        //    }
        //}
    }
}
