using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ABSoftware.ABParser.Events;

namespace ABSoftware.ABParser.Testing.JSTest
{
    public class JSMinificationParser : ABParser
    {
        static readonly ABParserTokensArray JSParserTokens = ABParserTokensArray.GenerateTokensArray(JSMinificationTokenProvider.ABParserTokens);

        public JSMinificationParser() : base(JSParserTokens) { }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            Console.WriteLine("BeforeTokenProcessed hit!");
        }

        protected override void OnTokenProcessed(OnTokenProcessedEventArgs args)
        {
            Console.WriteLine("OnTokenProcessed hit!");
        }
    }
}
