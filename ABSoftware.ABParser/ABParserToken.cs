using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser
{
    public sealed class ABParserToken
    {
        internal List<string> TokenLimitsToAddTo = new List<string>();

        /// <summary>
        /// The name this token can be given to identify it.
        /// </summary>
        public ABParserText TokenName;

        /// <summary>
        /// The data this token is made up of.
        /// </summary>
        public ABParserText TokenData;

        /// <summary>
        /// If you want to attach extra data alongside the token, you can use this to do that.
        /// </summary>
        public object TokenTag;

        public ABParserToken(ABParserText tokenName, ABParserText tokenData, object tag = null)
        {
            TokenName = tokenName;
            TokenData = tokenData;
            TokenTag = tag;
        }

        public ABParserToken(ABParserText nameAndData, object tag = null)
        {
            TokenName = nameAndData;
            TokenData = nameAndData;
            TokenTag = tag;
        }

        public ABParserToken AddToLimit(params string[] limits)
        {
            TokenLimitsToAddTo.AddRange(limits);
            return this;
        }
    }
}
