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
        public T RunParser<T>(ref T parser, string text)
            where T : TrackingParser
        {
            parser.SetText(new ABParserText(text));
            parser.Start();
            return parser;
        }

        // PARSERS:
        static NoTokensParser RunNoTokensParser = new NoTokensParser();
        static SingleTokenParser RunSingleTokenParser = new SingleTokenParser();
        static TheyParser RunTheyParser = new TheyParser();
        static TheyMiddleParser RunTheyMiddleParser = new TheyMiddleParser();

        public NoTokensParser RunNoTokens(string text) => RunParser(ref RunNoTokensParser, text);
        public SingleTokenParser RunSingleToken(string text) => RunParser(ref RunSingleTokenParser, text);
        public TheyParser RunThey(string text) => RunParser(ref RunTheyParser, text);
        public TheyMiddleParser RunTheyMiddle(string text) => RunParser(ref RunTheyMiddleParser, text);

    }
}
