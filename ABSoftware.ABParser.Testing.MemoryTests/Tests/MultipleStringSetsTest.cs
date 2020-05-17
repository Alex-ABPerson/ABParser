using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.MemoryTests.Tests
{
    /// <summary>
    /// A test that change the ABParser's text multiple times - use this to ensure that everytime new text is given, old data is deleted.
    /// </summary>
    public class MultipleStringSetsTest : MemoryTest
    {
        // Keep the parser alive in this variable.
        public static TestParser KeepAlive;

        public override void Run()
        {
            KeepAlive = new TestParser();
            for (int i = 0; i < 100; i++)
            {
                KeepAlive.SetText(new ABParserText(new string('*', i)));
                KeepAlive.Start();
            }
            
        }
    }
}
