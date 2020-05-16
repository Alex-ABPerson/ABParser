﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    /// <summary>
    /// Has "the", "they" and "theyare" tokens - a good test for verification.
    /// </summary>
    public class ABParserTheyParser : TrackingParser
    {
        static readonly ABParserTokensArray ParserTokens = ABParserTokensArray.GenerateTokensArray(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("the")),
            new ABParserToken(new ABParserText("they")),
            new ABParserToken(new ABParserText("theyare")),
        });

        public ABParserTheyParser() : base(ParserTokens) { }
    }
}