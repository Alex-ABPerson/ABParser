using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Exceptions
{
    public class ABParserTooManyTriviaLimits : Exception
    {
        public ABParserTooManyTriviaLimits() : base("The number of trivia limits provided exceed the number of trivia limits stated. If you're using trivia limits, you must say how many trivia limits you need as the last parameter on the 'ABParserConfiguration'.") { }
    }
}
