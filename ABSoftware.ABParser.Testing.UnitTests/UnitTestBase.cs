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
        static ABParserNoTokensParser RunNoTokensParser = new ABParserNoTokensParser();
        static ABParserSingleTokenParser RunSingleTokenParser = new ABParserSingleTokenParser();
        static ABParserTheyParser RunTheyParser = new ABParserTheyParser();

        public ABParserNoTokensParser RunNoTokens(string text) => RunParser(ref RunNoTokensParser, text);
        public ABParserSingleTokenParser RunSingleToken(string text) => RunParser(ref RunSingleTokenParser, text);
        public ABParserTheyParser RunThey(string text) => RunParser(ref RunTheyParser, text);

    }
}
