using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.MemPerfTests.Tests
{
    /// <summary>
    /// A test that change the ABParser's text multiple times - use this to ensure that everytime new text is given, old data is deleted.
    /// </summary>
    public class MultipleStringSetsTest : Test
    {
        TestParser _parser;
        ABParserText[] _texts;

        protected override int NumberOfIterations => 1000;
        protected override string TestName => "MultipleStringSetsTest";
        protected override void Prepare()
        {
            _parser = new TestParser();

            _texts = new ABParserText[NumberOfIterations];
            for (int i = 0; i < NumberOfIterations; i++)
                _texts[i] = new ABParserText(new string('*', i));
        }

        protected override void DoIteration(int i)
        {
            _parser.SetText(_texts[i]);
        }

        protected override void Finish()
        {
            
        }
    }
}
