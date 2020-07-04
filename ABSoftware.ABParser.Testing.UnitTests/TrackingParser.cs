using ABSoftware.ABParser.Events;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests
{
    public class TrackingParser : ABParser
    {
        public List<int> BeforeTokenProcessedTokenPreviousTokenStarts;
        public List<int> BeforeTokenProcessedTokenTokenStarts;
        public List<int> BeforeTokenProcessedTokenPreviousTokenEnds;
        public List<int> BeforeTokenProcessedTokenTokenEnds;
        public List<string> BeforeTokenProcessedLeadings;
        public List<string> BeforeTokenProcessedPreviousTokens;
        public List<string> BeforeTokenProcessedTokens;

        public List<int> OnTokenProcessedTokenPreviousTokenStarts;
        public List<int> OnTokenProcessedTokenTokenStarts;
        public List<int> OnTokenProcessedTokenNextTokenStarts;
        public List<int> OnTokenProcessedTokenPreviousTokenEnds;
        public List<int> OnTokenProcessedTokenTokenEnds;
        public List<int> OnTokenProcessedTokenNextTokenEnds;
        public List<string> OnTokenProcessedLeadings;
        public List<string> OnTokenProcessedTrailings;
        public List<string> OnTokenProcessedPreviousTokens;
        public List<string> OnTokenProcessedTokens;
        public List<string> OnTokenProcessedNextTokens;

        public int OnFirstUnlimitedCharProcessedPos;

        public string EndLeading;

        public TrackingParser(ABParserConfiguration config) : base(config) { }

        protected override void OnStart()
        {
            BeforeTokenProcessedLeadings = new List<string>();
            BeforeTokenProcessedPreviousTokens = new List<string>();
            BeforeTokenProcessedTokens = new List<string>();
            BeforeTokenProcessedTokenPreviousTokenStarts = new List<int>();
            BeforeTokenProcessedTokenTokenStarts = new List<int>();
            BeforeTokenProcessedTokenPreviousTokenEnds = new List<int>();
            BeforeTokenProcessedTokenTokenEnds = new List<int>();

            OnTokenProcessedTokenPreviousTokenStarts = new List<int>();
            OnTokenProcessedTokenTokenStarts = new List<int>();
            OnTokenProcessedTokenNextTokenStarts = new List<int>();
            OnTokenProcessedTokenPreviousTokenEnds = new List<int>();
            OnTokenProcessedTokenTokenEnds = new List<int>();
            OnTokenProcessedTokenNextTokenEnds = new List<int>();
            OnTokenProcessedLeadings = new List<string>();
            OnTokenProcessedTrailings = new List<string>();
            OnTokenProcessedPreviousTokens = new List<string>();
            OnTokenProcessedTokens = new List<string>();
            OnTokenProcessedNextTokens = new List<string>();

            EndLeading = null;

            OnFirstUnlimitedCharProcessedPos = 0;
        }

        protected override void OnFirstUnlimitedCharacterProcessed(int pos) => OnFirstUnlimitedCharProcessedPos = pos;

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            BeforeTokenProcessedPreviousTokens.Add(args.PreviousToken?.Token.Name);

            if (args.PreviousToken == null) {
                BeforeTokenProcessedTokenPreviousTokenStarts.Add(0);
                BeforeTokenProcessedTokenPreviousTokenEnds.Add(0);
            } else {
                BeforeTokenProcessedTokenPreviousTokenStarts.Add(args.PreviousToken.Start);
                BeforeTokenProcessedTokenPreviousTokenEnds.Add(args.PreviousToken.End);
            }

            BeforeTokenProcessedTokenTokenStarts.Add(args.CurrentToken.Start);
            BeforeTokenProcessedTokenTokenEnds.Add(args.CurrentToken.End);

            BeforeTokenProcessedLeadings.Add(args.GetLeadingAsString());
            BeforeTokenProcessedTokens.Add(args.CurrentToken.Token.Name);
        }

        protected override void OnTokenProcessed(OnTokenProcessedEventArgs args)
        {
            OnTokenProcessedPreviousTokens.Add(args.PreviousToken?.Token.Name);
            if (args.PreviousToken == null) {
                OnTokenProcessedTokenPreviousTokenStarts.Add(0);
                OnTokenProcessedTokenPreviousTokenEnds.Add(0);
            } else {
                OnTokenProcessedTokenPreviousTokenStarts.Add(args.PreviousToken.Start);
                OnTokenProcessedTokenPreviousTokenEnds.Add(args.PreviousToken.End);
            }

            OnTokenProcessedTokens.Add(args.CurrentToken.Token.Name);
            OnTokenProcessedTokenTokenStarts.Add(args.CurrentToken.Start);
            OnTokenProcessedTokenTokenEnds.Add(args.CurrentToken.End);

            OnTokenProcessedNextTokens.Add(args.NextToken?.Token.Name);
            if (args.NextToken == null) {
                OnTokenProcessedTokenNextTokenStarts.Add(0);
                OnTokenProcessedTokenNextTokenEnds.Add(0);
            } else {
                OnTokenProcessedTokenNextTokenStarts.Add(args.NextToken.Start);
                OnTokenProcessedTokenNextTokenEnds.Add(args.NextToken.End);
            }

            OnTokenProcessedLeadings.Add(args.GetLeadingAsString());
            OnTokenProcessedTrailings.Add(args.GetTrailingAsString());
        }

        protected override void OnEnd(OnEndEventArgs args)
        {
            EndLeading = args.GetLeadingAsString();
        }
        
        public TrackingParser Test(string toTest, object expected)
        {
            return toTest switch
            {
                "Trivia" => TestTrivia((string[])expected),
                "Tokens" => TestTokens((string[])expected),
                "TokenStarts" => TestStarts((int[])expected),
                "TokenEnds" => TestEnds((int[])expected),
                "OFUCP" => TestOFUCP((int)expected),
                _ => throw new Exception("INVALID TEST MODE")
            };
        }

        public TrackingParser TestTrivia(string[] expected)
        {
            var leadingEndExpected = expected[^1];

            var leadingExpected = new string[expected.Length - 1];
            var trailingExpected = new string[expected.Length - 1];

            Array.Copy(expected, leadingExpected, expected.Length - 1);
            Array.Copy(expected, 1, trailingExpected, 0, expected.Length - 1);

            CollectionAssert.AreEqual(BeforeTokenProcessedLeadings, leadingExpected);
            CollectionAssert.AreEqual(OnTokenProcessedLeadings, leadingExpected);
            CollectionAssert.AreEqual(OnTokenProcessedTrailings, trailingExpected);

            Assert.AreEqual(EndLeading, leadingEndExpected);
            return this;
        }

        public TrackingParser TestTokens(string[] expected)
        {
            var previousTokensExpected = new string[expected.Length];
            var tokensExpected = expected;
            var nextTokensExpected = new string[expected.Length];

            Array.Copy(expected, 0, previousTokensExpected, 1, expected.Length - 1);
            Array.Copy(expected, 1, nextTokensExpected, 0, expected.Length - 1);

            CollectionAssert.AreEqual(BeforeTokenProcessedPreviousTokens, previousTokensExpected);
            CollectionAssert.AreEqual(BeforeTokenProcessedTokens, tokensExpected);

            CollectionAssert.AreEqual(OnTokenProcessedPreviousTokens, previousTokensExpected);
            CollectionAssert.AreEqual(OnTokenProcessedTokens, tokensExpected);
            CollectionAssert.AreEqual(OnTokenProcessedNextTokens, nextTokensExpected);
            return this;
        }

        public TrackingParser TestStarts(int[] expected)
        {
            var previousTokensStarts = new int[expected.Length];
            var tokenStarts = expected;
            var nextTokensStarts = new int[expected.Length];

            Array.Copy(expected, 0, previousTokensStarts, 1, expected.Length - 1);
            Array.Copy(expected, 1, nextTokensStarts, 0, expected.Length - 1);

            CollectionAssert.AreEqual(OnTokenProcessedTokenPreviousTokenStarts, previousTokensStarts);
            CollectionAssert.AreEqual(OnTokenProcessedTokenTokenStarts, tokenStarts);
            CollectionAssert.AreEqual(OnTokenProcessedTokenNextTokenStarts, nextTokensStarts);
            return this;
        }

        public TrackingParser TestEnds(int[] expected)
        {
            var previousTokensEnds = new int[expected.Length];
            var tokenEnds = expected;
            var nextTokensEnds = new int[expected.Length];

            Array.Copy(expected, 0, previousTokensEnds, 1, expected.Length - 1);
            Array.Copy(expected, 1, nextTokensEnds, 0, expected.Length - 1);

            CollectionAssert.AreEqual(OnTokenProcessedTokenPreviousTokenEnds, previousTokensEnds);
            CollectionAssert.AreEqual(OnTokenProcessedTokenTokenEnds, tokenEnds);
            CollectionAssert.AreEqual(OnTokenProcessedTokenNextTokenEnds, nextTokensEnds);
            return this;
        }

        public TrackingParser TestOFUCP(int expected)
        {
            Assert.AreEqual(expected, OnFirstUnlimitedCharProcessedPos);
            return this;
        }

    }
}