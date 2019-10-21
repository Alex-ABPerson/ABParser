using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Events
{
    public class TokenProcessedEventArgs : EventArgs
    {
        protected readonly ABParser _parser;

        #region Internal Data
        internal ushort PreviousTokenIndex;
        internal bool HasPreviousToken;
        internal ushort TokenIndex;
        #endregion

        #region Public Properties

        /// <summary>
        /// The text that is in-between this token and the previous.
        /// </summary>
        public ABParserText Leading { get; internal set; }
        
        /// <summary>
        /// The token that was encountered before this token.
        /// </summary>
        public ABParserToken PreviousToken => HasPreviousToken ? _parser.Tokens[PreviousTokenIndex] : null;
        public int PreviousTokenStart { get; internal set; }
        public int PreviousTokenEnd => HasPreviousToken ? PreviousTokenStart + PreviousToken.TokenData.GetLength() : 0;

        /// <summary>
        /// The token that was encountered.
        /// </summary>
        public ABParserToken Token => _parser.Tokens[TokenIndex];
        public int TokenStart { get; internal set; }
        public int TokenEnd => TokenStart + Token.TokenData.GetLength();
        #endregion

        internal TokenProcessedEventArgs(ABParser parser) => _parser = parser;
    }
}
