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
        public ABParserText TokenName;

        /// <summary>
        /// The data this token is made up of.
        /// </summary>
        public ABParserText TokenData;

        public ABParserToken(ABParserText tokenName, ABParserText tokenData)
        {
            TokenName = tokenName;
            TokenData = tokenData;
        }
    }
}
