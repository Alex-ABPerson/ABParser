using ABSoftware.ABParser.Testing.UnitTests.Parsers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.MemPerfTests.Tests
{
    /// <summary>
    /// A test that reuses an ABParser multiple times - use this to ensure that everytime you start a new parse specific data from the previous parse is deleted.
    /// </summary>
    public class MultipleReuseTest : Test
    {
        SingleTokenParser _parser;

        protected override int NumberOfIterations => 10000;
        protected override string TestName => "MultipleReuseTest";

        protected override void Prepare()
        {
            _parser = new SingleTokenParser();
            _parser.SetText(new ABParserText("AbbbbbbbbbbbbbBCDashBCDosh[peaAahlahayBCDkaoph;asA"));
        }

        protected override void DoIteration(int i)
        {
            _parser.Start();
        }

        protected override void Finish()
        {
            _parser.Dispose();
        }
    }
}
