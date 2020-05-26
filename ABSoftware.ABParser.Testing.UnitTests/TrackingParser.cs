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
        public List<string> BeforeTokenProcessedLeadings = new List<string>();
        public List<string> BeforeTokenProcessedPreviousTokens = new List<string>();
        public List<string> BeforeTokenProcessedTokens = new List<string>();

        public List<string> OnTokenProcessedLeadings = new List<string>();
        public List<string> OnTokenProcessedTrailings = new List<string>();
        public List<string> OnTokenProcessedPreviousTokens = new List<string>();
        public List<string> OnTokenProcessedTokens = new List<string>();
        public List<string> OnTokenProcessedNextTokens = new List<string>();

        public string EndLeading;

        protected override void OnStart()
        {
            BeforeTokenProcessedLeadings = new List<string>();
            BeforeTokenProcessedPreviousTokens = new List<string>();
            BeforeTokenProcessedTokens = new List<string>();

            OnTokenProcessedLeadings = new List<string>();
            OnTokenProcessedTrailings = new List<string>();
            OnTokenProcessedPreviousTokens = new List<string>();
            OnTokenProcessedTokens = new List<string>();
            OnTokenProcessedNextTokens = new List<string>();

            EndLeading = null;
        }

        public TrackingParser(ABParserConfiguration tokensArray) : base(tokensArray) { }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            BeforeTokenProcessedLeadings.Add(args.Leading.AsString());
            BeforeTokenProcessedPreviousTokens.Add(args.PreviousToken?.TokenName.AsString());
            BeforeTokenProcessedTokens.Add(args.Token.TokenName.AsString());
        }

        protected override void OnTokenProcessed(OnTokenProcessedEventArgs args)
        {
            OnTokenProcessedLeadings.Add(args.Leading.AsString());
            OnTokenProcessedTrailings.Add(args.Trailing.AsString());
            OnTokenProcessedPreviousTokens.Add(args.PreviousToken?.TokenName.AsString());
            OnTokenProcessedTokens.Add(args.Token.TokenName.AsString());
            OnTokenProcessedNextTokens.Add(args.NextToken?.TokenName.AsString());
        }

        protected override void OnEnd(ABParserText leading)
        {
            EndLeading = leading.AsString();
        }
        
        public TrackingParser Test(string toTest, string[] expected, string leadingEndExpected)
        {
            switch (toTest)
            {
                case "Leadings": return TestLeadings(expected, leadingEndExpected);
                case "Trailings": return TestTrailings(expected);
                case "PreviousTokens": return TestPreviousTokens(expected);
                case "Tokens": return TestTokens(expected);
                case "NextTokens": return TestNextTokens(expected);
                default: throw new Exception("INVALID TEST MODE");
            }
        }

        public TrackingParser TestLeadings(string[] expected, string endExpected) => TestLeadings(expected, expected, endExpected);

        public TrackingParser TestLeadings(string[] beforeExpected, string[] onExpected, string endExpected)
        {
            CollectionAssert.AreEqual(BeforeTokenProcessedLeadings, beforeExpected);
            CollectionAssert.AreEqual(OnTokenProcessedLeadings, onExpected);
            Assert.AreEqual(EndLeading, endExpected);
            return this;
        }

        public TrackingParser TestTrailings(string[] expected)
        {
            CollectionAssert.AreEqual(OnTokenProcessedTrailings, expected);
            return this;
        }

        public TrackingParser TestPreviousTokens(string[] expected) => TestPreviousTokens(expected, expected);
        public TrackingParser TestPreviousTokens(string[] beforeExpected, string[] onExpected)
        {
            CollectionAssert.AreEqual(BeforeTokenProcessedPreviousTokens, beforeExpected);
            CollectionAssert.AreEqual(OnTokenProcessedPreviousTokens, onExpected);
            return this;
        }

        public TrackingParser TestTokens(string[] expected) => TestTokens(expected, expected);
        public TrackingParser TestTokens(string[] beforeExpected, string[] onExpected)
        {
            CollectionAssert.AreEqual(BeforeTokenProcessedTokens, beforeExpected);
            CollectionAssert.AreEqual(OnTokenProcessedTokens, onExpected);
            return this;
        }

        public TrackingParser TestNextTokens(string[] expected)
        {
            CollectionAssert.AreEqual(OnTokenProcessedNextTokens, expected);
            return this;
        }

    }
}