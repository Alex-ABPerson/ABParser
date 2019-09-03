using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser
{
    public sealed class ABParserToken
    {

        /// <summary>
        /// The name this token can be given to identify it.
        /// </summary>
        public char[] TokenName;

        /// <summary>
        /// The data this token is made up of.
        /// </summary>
        public StringBuilder TokenData;

    }
}
