using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.MemoryTests.Tests
{
    /// <summary>
    /// A test that reuses an ABParser multiple times - use this to ensure that everytime you start a new parse specific data from the previous parse is deleted.
    /// </summary>
    public class MultipleReuseMemoryTest : MemoryTest
    {
        public override void Run()
        {
            using (var parser = new TestParser())
            {
                for (int i = 0; i < 1000; i++)
                    parser.Start(new ABParserText("AtheBtheyCtheyarDtheyareE"));
            }
        }
    }
}
