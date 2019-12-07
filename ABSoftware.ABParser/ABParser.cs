using ABSoftware.ABParser.Events;
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
    public abstract class ABParser : IDisposable
    {
        // NOTE: The core part of ABParser is written in C++, that is most likely where you want to look.
        // Check ABSoftware.ABParser.Core and the ABSoftware Docs for more info.

        #region Public Data

        public ABParserText Text;
        public int TextLength;
        public ABParserToken[] Tokens;

        #endregion

        #region Internal Data

        /// <summary>
        /// A pointer to the actual parser in C++, this pointer can be used to get or set information.
        /// </summary>
        IntPtr _baseParser;

        /// <summary>
        /// Data that is transferred alongside each result.
        /// </summary>
        ushort[] Data;

        bool FirstBeforeTokenProcessed = true;
        bool FirstOnTokenProcessed = true;

        BeforeTokenProcessedEventArgs BeforeTokenProcessedArgs;
        OnTokenProcessedEventArgs OnTokenProcessedArgs;

        #endregion

        #region Internal Management

        internal void InitializeABParser(ABParserTokensArray tokens)
        {
            // Set the tokens.
            Tokens = tokens.Tokens;

            // Then, initialize the base parser.
            InitializeBaseParser(tokens);

        }

        internal void InitializeBaseParser(ABParserTokensArray tokens) => _baseParser = NativeMethods.CreateBaseParser(tokens.SingleCharTokensPointer, tokens.MultiCharTokensPointer, tokens.SingleCharTokensLength, tokens.MultiCharTokensLength);

        internal void SetBaseParserText(ABParserText text)
        {
            TextLength = text.GetLength();
            NativeMethods.SetText(_baseParser, text.AsString(), TextLength);

            // Now that we know the size of text, we can generate the maximum size of the data.
            Data = new ushort[text.GetLength() * 2 + 11];
        }

        internal int TwoShortsToInteger(ushort[] arr, int index) => (arr[index] >> 16) + arr[index + 1];

        internal int ShortsToString(ushort[] arr, int index, out ABParserText text)
        {
            // Get the length of the string.
            int length = TwoShortsToInteger(arr, index);
            var result = new char[length];

            // Go through and convert all of them to characters.
            index += 2;
            for (int i = 0; i < length; i++)
                result[i] = (char)arr[index++];

            // Turn that into an "ABParserText", and send that out - also sending out the ending point.
            text = new ABParserText(result);
            return index;
        }

        internal void ParseResult(ContinueExecutionResult result)
        {
            switch (result)
            {
                case ContinueExecutionResult.Stop:
                case ContinueExecutionResult.BeforeTokenProcessed:

                    // If this is a "Stop" command, we'll generate some "OnTokenProcessedEventArgs", otherwise, we'll generate a "BeforeTokenProcessedEventArgs".
                    var eventArgs = result == ContinueExecutionResult.Stop ? (TokenProcessedEventArgs)new OnTokenProcessedEventArgs(this) : new BeforeTokenProcessedEventArgs(this);

                    // Copy across all of the data from what the Core ABParser gave us.
                    eventArgs.TokenIndex = Data[0];
                    eventArgs.TokenStart = TwoShortsToInteger(Data, 1);
                    eventArgs.HasPreviousToken = result == ContinueExecutionResult.Stop ? !FirstOnTokenProcessed : !FirstBeforeTokenProcessed;
                    eventArgs.PreviousTokenIndex = FirstBeforeTokenProcessed ? (ushort)0 : Data[3];
                    eventArgs.PreviousTokenStart = FirstBeforeTokenProcessed ? TwoShortsToInteger(Data, 4) : 0;

                    // Set the leading.
                    var leadingEnd = ShortsToString(Data, 6, out var leading);
                    eventArgs.Leading = leading;

                    // Set the trailing - but only if this was a "stop" command.
                    if (result == ContinueExecutionResult.Stop)
                    {
                        // Turn it into an "OnTokenProcessedEventArgs", since that's what a "stop" command needs.
                        OnTokenProcessedArgs = (OnTokenProcessedEventArgs)eventArgs;

                        // Give it the trailing.
                        ShortsToString(Data, leadingEnd, out var trailing);
                        OnTokenProcessedArgs.Trailing = trailing;
                    }

                    // Otherwise, if we're creating a "BeforeTokenProcessed" event, then set the "BeforeTokenProcessedArgs".
                    else BeforeTokenProcessedArgs = (BeforeTokenProcessedEventArgs)eventArgs;

                    break;
                case ContinueExecutionResult.OnAndBeforeTokenProcessed:

                    // When reading this code, just remember that the BeforeTokenProcessed is for the token AFTER the OnTokenProcessed.

                    // Create the two new eventArgs - the "OnTokenProcessed" will, of course, have a next token in this case.
                    var beforeTokenProcessedArgs = new BeforeTokenProcessedEventArgs(this)
                    {
                        HasPreviousToken = true
                    };
                    var onTokenProcessedArgs = new OnTokenProcessedEventArgs(this)
                    {
                        HasPreviousToken = !FirstOnTokenProcessed,
                        HasNextToken = true
                    };

                    // First, we'll set the OnTokenProcessed token - for "BeforeTokenProcessed", this is actually the previous token.
                    beforeTokenProcessedArgs.PreviousTokenIndex = onTokenProcessedArgs.TokenIndex = Data[0];
                    beforeTokenProcessedArgs.PreviousTokenStart = onTokenProcessedArgs.TokenStart = TwoShortsToInteger(Data, 1);

                    // Next, we'll set the previous token for the "OnTokenProcessed" - this applies to only the "OnTokenProcessed".
                    onTokenProcessedArgs.PreviousTokenIndex = Data[3];
                    onTokenProcessedArgs.PreviousTokenStart = TwoShortsToInteger(Data, 4);

                    // Then, we'll set the BeforeTokenProcessed token - which, for the "OnTokenProcessed" is actually the next token.
                    onTokenProcessedArgs.NextTokenIndex = beforeTokenProcessedArgs.TokenIndex = Data[6];
                    onTokenProcessedArgs.NextTokenStart = beforeTokenProcessedArgs.TokenStart = TwoShortsToInteger(Data, 7);

                    // After that, we'll set the leading for the "OnTokenProcessed", which means nothing to the "BeforeTokenProcessed".
                    var onTokenProcessedLeadingEnd = ShortsToString(Data, 9, out var onTokenProcessedLeading);
                    onTokenProcessedArgs.Leading = onTokenProcessedLeading;

                    // And, after that, we'll set the trailing for the "OnTokenProcessed", which is the leading for the "BeforeTokenProcessed".
                    ShortsToString(Data, onTokenProcessedLeadingEnd, out var onTokenProcessedTrailing);
                    beforeTokenProcessedArgs.Leading = onTokenProcessedArgs.Trailing = onTokenProcessedTrailing;

                    // Finally, put those two into the main fields.
                    BeforeTokenProcessedArgs = beforeTokenProcessedArgs;
                    OnTokenProcessedArgs = onTokenProcessedArgs;

                    break;
            }
        }

        #endregion

        #region Main Execution

        internal void Execute()
        {
            // Trigger the "OnStart".
            OnStart();

            // This is how execution works on this side.
            // Quite simply, we will run "ContinueExecution", and that will do all of the work in C++.
            // Then, whenever the C++ code wants us to do something - like calling "OnTokenProcessed", it will return a result, and we will act on that.
            // After we've done that, we'll then just get it to continue execution.
            var result = ContinueExecutionResult.None;

            // Just keep on executing until we hit the "Stop" result.
            while (result != ContinueExecutionResult.Stop)
            {
                ParseResult(result = NativeMethods.ContinueExecution(_baseParser, Data));

                // Do whatever the result said to do.
                switch (result)
                {
                    case ContinueExecutionResult.Stop:

                        OnTokenProcessed(OnTokenProcessedArgs);
                        break;

                    case ContinueExecutionResult.BeforeTokenProcessed:

                        // If this was the first "BeforeTokenProcessed", then it won't be anymore in the future.
                        if (FirstBeforeTokenProcessed)
                            FirstBeforeTokenProcessed = false;

                        // Trigger the events.
                        BeforeTokenProcessed(BeforeTokenProcessedArgs);

                        break;
                    case ContinueExecutionResult.OnAndBeforeTokenProcessed:

                        // If this was the first "OnTokenProcessed", then it won't be anymore in the future.
                        if (FirstOnTokenProcessed)
                            FirstOnTokenProcessed = false;

                        // Trigger the events.
                        BeforeTokenProcessed(BeforeTokenProcessedArgs);
                        OnTokenProcessed(OnTokenProcessedArgs);

                        break;
                }
            }

            // Then, run "OnEnd" - for the leading, if we've never hit an "OnTokenProcessed", then this will just be the entire text, otherwise, we'll just pull the trailing from the last OnTokenProcessed to get this leading.
            OnEnd(FirstOnTokenProcessed ? Text : OnTokenProcessedArgs.Trailing);

        }

        #endregion

        #region Events

        protected virtual void OnStart() { }

        protected virtual void OnEnd(ABParserText leading) { }

        /// <summary>
        /// Called before a token's trailing has been generated - mainly used to generate TokenLimits!
        /// </summary>
        protected virtual void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args) { }

        /// <summary>
        /// Called when a token has been fully processed, and its leading and trailing have been generated.
        /// </summary>
        protected virtual void OnTokenProcessed(OnTokenProcessedEventArgs args) { }

        #endregion

        #region Public Methods

        public void Start(ABParserText text)
        {
            SetBaseParserText(Text = text);
            Execute();
        }

        #endregion

        #region Constructor / Dispose

        protected ABParser(ABParserTokensArray tokens) => InitializeABParser(tokens);
        public void Dispose() => NativeMethods.DeleteBaseParser(_baseParser);

        #endregion
    }
}
