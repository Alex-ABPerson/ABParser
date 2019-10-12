using ABSoftware.ABParser.Internal;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser
{
    /// <summary>
    /// Represents an ABParser.
    /// </summary>
    public class ABParser : IDisposable
    {
        // NOTE: The core part of ABParser is written in C++, that is most likely where you want to look.
        // Check ABSoftware.ABParser.Core and the ABSoftware Docs for more info.

        #region Public Data
        
        public int TextLength;

        #endregion

        #region Internal Data

        /// <summary>
        /// A pointer to the actual parser in C++, this pointer can be used to get or set information.
        /// </summary>
        IntPtr _baseParser;

        #endregion

        #region Internal Management

        internal void InitializeABParser(ABParserToken[] tokens)
        {
            // Put all of the tokens down into an array of an array of token data.
            var arrayOfTokenData = new string[tokens.Length];
            var arrayOfTokenLengths = new int[tokens.Length];

            // Initialize the tokens.
            InitializeTokens(tokens, arrayOfTokenData, arrayOfTokenLengths);

        }

        internal void InitializeTokens(ABParserToken[] tokens, string[] arrayOfTokenData, int[] arrayOfTokenLengths)
        {
            // Go through all of the tokens and put their token data into this array.
            for (int i = 0; i < tokens.Length; i++)
            {
                arrayOfTokenData[i] = tokens[i].TokenData.AsString();
                arrayOfTokenLengths[i] = tokens[i].TokenData.GetLength();
            }

            // Finally, give this data to the base parser - to initialize it.
            _baseParser = NativeMethods.CreateBaseParser(arrayOfTokenData, arrayOfTokenLengths, tokens.Length);
        }

        internal void SetBaseParserText(ABParserText text)
        {
            TextLength = text.GetLength();
            NativeMethods.SetText(_baseParser, text.AsString(), TextLength);
        }

        #endregion

        #region Main Execution

        internal void Execute()
        {
            // This is how execution works on this side.
            // Quite simply, we will run "ContinueExecution", and that will do all of the work in C++.
            // Then, whenever the C++ code wants us to do something - like calling "OnTokenProcessed", it will return a result, and we will act on that.
            // After we've done that, we'll then just get it to continue execution.
            var result = ContinueExecutionResult.None;
            var data = new byte[5];

            // Just keep on executing until we hit the "Stop" result.
            while (result != ContinueExecutionResult.Stop)
            {
                result = NativeMethods.ContinueExecution(_baseParser, data);

                // Do whatever the result said to do.
                switch (result)
                {
                    case ContinueExecutionResult.BeforeTokenProcessed:
                        Console.WriteLine("C#: BEFORETOKENPROCESSED RECIEVED!");
                        break;
                    case ContinueExecutionResult.OnAndBeforeTokenProcessed:
                        Console.WriteLine("C#: BEFORETOKENPROCESSED + ONTOKENPROCESSED RECIEVED!");
                        break;
                }
            }
        }

        #endregion

        #region Public Methods

        public void Start(string text)
        {
            SetBaseParserText(new ABParserText(text));
            Execute();
        }

        public void Start(char[] text)
        {
            // Set the text for the parser.
            SetBaseParserText(new ABParserText(text));
            Execute();
        }

        public void Start(StringBuilder text)
        {
            // Set the text for the parser.
            SetBaseParserText(new ABParserText(text));
            Execute();
        }

        public void Start(ABParserText text)
        {
            // Set the text for the parser.
            SetBaseParserText(text);
            Execute();
        }

        #endregion

        #region Constructor / Dispose

        public ABParser(ABParserToken[] tokens) => InitializeABParser(tokens);
        public void Dispose() => NativeMethods.DeleteBaseParser(_baseParser);

        #endregion
    }
}
