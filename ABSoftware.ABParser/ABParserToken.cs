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

        public ABParserToken(char[] tokenName, string tokenData)
        {
            TokenName = tokenName;
            TokenData = new StringBuilder();
            TokenData.Append(tokenData);
        }

        public ABParserToken(char[] tokenName, StringBuilder tokenData)
        {
            TokenName = tokenName;
            TokenData = tokenData;
        }

        public ABParserToken(char[] tokenName, char[] tokenData)
        {
            TokenName = tokenName;
            TokenData = new StringBuilder();
            TokenData.Append(tokenData);
        }

        public ABParserToken(string tokenName, string tokenData)
        {
            TokenName = tokenName.ToCharArray();
            TokenData = new StringBuilder();
            TokenData.Append(tokenData);
        }

        public ABParserToken(string tokenName, char[] tokenData)
        {
            TokenName = tokenName.ToCharArray();
            TokenData = new StringBuilder();
            TokenData.Append(tokenData);
        }

        public ABParserToken(string tokenName, StringBuilder tokenData)
        {
            TokenName = tokenName.ToCharArray();
            TokenData = tokenData;
        }
    }
}
