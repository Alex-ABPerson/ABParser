using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Events
{
    public class OnTokenProcessedEventArgs : TokenProcessedEventArgs
    {
        internal bool HasNextToken;
        internal ushort NextTokenIndex;

        /// <summary>
        /// The text that is in-between this token and the next.
        /// </summary>
        public ABParserText Trailing { get; internal set; }

        /// <summary>
        /// The token that comes after this token.
        /// </summary>
        public ABParserToken NextToken => HasNextToken ? _parser.Tokens[NextTokenIndex] : null;
        public int NextTokenStart { get; internal set; }
        public int NextTokenEnd => HasNextToken ? NextTokenStart + NextToken.TokenName.GetLength() : 0;

        internal OnTokenProcessedEventArgs(ABParser parser) : base(parser) { }
    }
}
