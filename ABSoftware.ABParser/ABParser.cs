using ABSoftware.ABParser.Events;
using ABSoftware.ABParser.Internal;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
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

        bool EndHasOnTokenProcessed;
        OnTokenProcessedEventArgs OnTokenProcessedArgs;

        bool _disposedForNextParse = false;
        bool _disposeAtDestruction = false;
        bool _disposeAsyncronously = true;
        bool _currentlyDisposing = false;

        #endregion

        #region Internal Management

        internal void InitializeABParser(ABParserTokensContainer tokens)
        {
            // Set the tokens.
            Tokens = tokens.Tokens;

            // Then, initialize the base parser.
            InitializeBaseParser(tokens);

        }

        internal void InitializeBaseParser(ABParserTokensContainer tokens) => _baseParser = NativeMethods.CreateBaseParser(tokens.TokensStorage);

        internal void InitString(ABParserText text)
        {
            Text = text;
            TextLength = text.GetLength();
            NativeMethods.InitString(_baseParser, text.AsString(), TextLength);

            // Now that we know the size of text, we can generate the maximum size of the data we will recieve for events.
            Data = new ushort[TextLength * 2 + 11];
        }

        internal void ResetInfo()
        {
            BeforeTokenProcessedArgs = null;
            OnTokenProcessedArgs = null;
            EndHasOnTokenProcessed = false;
            FirstBeforeTokenProcessed = true;
            FirstOnTokenProcessed = true;
        }

        internal async void DisposeDataForNextParse()
        {
            if (_disposedForNextParse)
                return;
            _disposedForNextParse = true;
            if (_disposeAsyncronously)
            {
                if (_currentlyDisposing) 
                    return;
                _currentlyDisposing = true;

                await Task.Run(() =>
                {
                    NativeMethods.DisposeDataForNextParse(_baseParser);
                    _currentlyDisposing = false;
                });
            } else NativeMethods.DisposeDataForNextParse(_baseParser);
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

                    // If this is a stop and we haven't had a single "BeforeTokenProcessed" yet, then we encountered no tokens and we shouldn't do anything.
                    if (result == ContinueExecutionResult.Stop && FirstBeforeTokenProcessed)
                    {
                        EndHasOnTokenProcessed = false;
                        return;
                    }

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
                        EndHasOnTokenProcessed = true;
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

        internal async Task Execute()
        {
            // Don't do anything if there isn't any text to parse.
            if (TextLength == 0)
            {
                OnStart();
                OnEnd(new ABParserText(""));
            }

            // If we're currently disposing, then wait until we're done before moving on.
            while (_currentlyDisposing)
                await Task.Delay(1);

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

                        if (!EndHasOnTokenProcessed)
                            break;

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

            // Then, run "OnEnd", simply using the trailing from the past "OnTokenProcessed".
            OnEnd(OnTokenProcessedArgs == null ? Text : OnTokenProcessedArgs.Trailing);

            // Finally, dispose all of the data - ready for the next parse.
            _disposedForNextParse = false;
            if (!_disposeAtDestruction)
                DisposeDataForNextParse();

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

        public void SetText(ABParserText text) => SetTextAsync(text).Wait();

        public Task SetTextAsync(ABParserText text)
        {
            return Task.Run(async () =>
            {
                while (_currentlyDisposing)
                    await Task.Delay(1);
                InitString(text);
            });
        }

        public void Start() => StartAsync().Wait();

        public async Task StartAsync()
        {
            if (Text == null)
                throw new Exception("The text hasn't been initialized yet!");
            ResetInfo();
            await Execute();
        }

        public void ChangeDisposeConfiguration(bool disposeAtDestruction, bool disposeAsyncronously)
        {
            _disposeAtDestruction = disposeAtDestruction;
            _disposeAsyncronously = disposeAsyncronously;
        }

        #endregion

        #region Constructor / Dispose

        protected ABParser(ABParserTokensContainer tokens) => InitializeABParser(tokens);
        public async void Dispose()
        {
            while (_currentlyDisposing)
                await Task.Delay(1);
            if (_disposeAtDestruction && !_disposedForNextParse)
                DisposeDataForNextParse();
            NativeMethods.DeleteBaseParser(_baseParser);
        }

        #endregion
    }
}
