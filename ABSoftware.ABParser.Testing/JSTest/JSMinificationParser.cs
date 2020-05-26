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
        static readonly ABParserConfiguration JSParserConfig = ABParserConfiguration.Create(JSMinificationTokenProvider.ABParserConfig);

        public JSMinificationParser() : base(JSParserConfig) { }

        protected override void OnStart()
        {
            //Console.WriteLine("Parser started!");
        }

        protected override void OnEnd(ABParserText leading)
        {
            //Console.WriteLine("Parser finished!");
        }

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
