using ABSoftware.ABParser.Exceptions;
using ABSoftware.ABParser.Internal;
using System;
using System.Collections.Generic;
namespace ABSoftware.ABParser
{
    public struct ABParserConfigurationTriviaLimit
    {
        public string Name;
        public char[] ToIgnore;

        public void Init(string name, char[] toIgnore)
        {
            Name = name;
            ToIgnore = toIgnore;
        }
    }

    /// <summary>
    /// Represents an array of ABParser Tokens.
    /// </summary>
    public class ABParserConfiguration : IDisposable
    {
        public ABParserConfigurationTriviaLimit[] TriviaLimits;
        int _triviaLimitsProvided;

        // The storage space for tokens on the C++ side, this is passed onto any instance of the parser.
        internal IntPtr TokensStorage;

        public ABParserToken[] Tokens;

        public unsafe ABParserConfiguration(ABParserToken[] tokens, int numberOfTriviaTokens = 0)
        {
            if (tokens.Length > ushort.MaxValue) throw new ABParserTooManyTokens();
            Tokens = tokens;

            var tokenData = new string[tokens.Length];
            var tokenDataLengths = stackalloc ushort[tokens.Length];
            var limitsPerToken = stackalloc ushort[tokens.Length];
            var limitNames = new List<string>();
            var tokenDetectionLimits = new string[tokens.Length];
            var tokenDetectionSizes = stackalloc ushort[tokens.Length];

            for (int i = 0; i < tokens.Length; i++)
            {
                tokenData[i] = tokens[i].Data;
                tokenDataLengths[i] = (ushort)tokens[i].Data.Length;

                if (tokens[i].TokenLimits == null)
                    limitsPerToken[i] = 0;
                else
                {
                    limitNames.AddRange(tokens[i].TokenLimits);
                    limitsPerToken[i] = (ushort)tokens[i].TokenLimits.Length;
                }

                if (tokens[i].DetectionLimits == null)
                    tokenDetectionSizes[i] = 0;
                else
                {
                    tokenDetectionLimits[i] = new string(tokens[i].DetectionLimits);
                    tokenDetectionSizes[i] = (ushort)tokenDetectionLimits[i].Length;
                }
            }

            var limitNameSizes = stackalloc byte[limitNames.Count];
            for (int i = 0; i < limitNames.Count; i++)
                limitNameSizes[i] = (byte)limitNames[i].Length;

            TokensStorage = NativeMethods.InitializeConfiguration(tokenData, tokenDataLengths, (ushort)tokens.Length, limitNames.ToArray(), limitNameSizes, limitsPerToken, tokenDetectionLimits, tokenDetectionSizes);
            TriviaLimits = new ABParserConfigurationTriviaLimit[numberOfTriviaTokens];
        }

        public ABParserConfiguration AddTriviaLimit(string name, params char[] toIgnore)
        {
            if (name.Length > 255) throw new ABParserNameTooLong();

            if (TriviaLimits.Length == _triviaLimitsProvided) throw new ABParserTooManyTriviaLimits();
            else
            {
                TriviaLimits[_triviaLimitsProvided++].Init(name, toIgnore);

                if (TriviaLimits.Length == _triviaLimitsProvided)
                    FlushTriviaLimits();
            }

            return this;
        }

        private unsafe void FlushTriviaLimits()
        {
            var limitNames = new string[TriviaLimits.Length];
            var limitLengths = stackalloc byte[TriviaLimits.Length];

            var limitContents = new string[TriviaLimits.Length];
            var limitContentLengths = stackalloc ushort[TriviaLimits.Length];
            
            for (int i = 0; i < TriviaLimits.Length; i++)
            {
                limitNames[i] = TriviaLimits[i].Name;
                limitLengths[i] = (byte)TriviaLimits[i].Name.Length;

                limitContents[i] = new string(TriviaLimits[i].ToIgnore);
                limitContentLengths[i] = (ushort)TriviaLimits[i].ToIgnore.Length;
            }

            NativeMethods.ConfigSetTriviaLimits(TokensStorage, limitNames, limitLengths, limitContents, limitContentLengths, TriviaLimits.Length);
        }

        public void Dispose()
        {
            NativeMethods.DeleteConfiguration(TokensStorage);
        }
    }
}
