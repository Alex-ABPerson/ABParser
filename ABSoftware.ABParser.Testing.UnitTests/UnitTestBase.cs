using ABSoftware.ABParser.Testing.UnitTests.Parsers;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests
{
    public class UnitTestBase
    {
        public T InitializeAndRunParser<T>(ref T parser, string text)
            where T : TrackingParser
        {
            if (parser == null)
                parser = (T)Activator.CreateInstance(typeof(T));

            parser.SetText(new ABParserText(text));
            parser.Start();
            return parser;
        }

        static NoTokensParser RunNoTokensParser;
        static SingleTokenParser RunSingleTokenParser;
        static TheyParser RunTheyParser;
        static TheyMiddleParser RunTheyMiddleParser;
        static QuoteLimitParser RunQuoteLimitParser;
        static AngledLimitParser RunAngledLimitParser;

        public NoTokensParser RunNoTokens(string text) => InitializeAndRunParser(ref RunNoTokensParser, text);
        public SingleTokenParser RunSingleToken(string text) => InitializeAndRunParser(ref RunSingleTokenParser, text);
        public TheyParser RunThey(string text) => InitializeAndRunParser(ref RunTheyParser, text);
        public TheyMiddleParser RunTheyMiddle(string text) => InitializeAndRunParser(ref RunTheyMiddleParser, text);
        public QuoteLimitParser RunQuoteLimit(string text) => InitializeAndRunParser(ref RunQuoteLimitParser, text);
        public AngledLimitParser RunAngledLimit(string text) => InitializeAndRunParser(ref RunAngledLimitParser, text);

    }
}
