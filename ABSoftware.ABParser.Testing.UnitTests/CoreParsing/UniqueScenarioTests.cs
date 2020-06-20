using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.CoreParsing
{
    [TestClass]
    public class UniqueScenarioTests : UnitTestBase
    {
        [TestMethod]
        public void NoText()
        {
            var parser = RunSingleToken("");
            Assert.AreEqual(parser.OnTokenProcessedLeadings.Count, 0);
        }
    }
}
