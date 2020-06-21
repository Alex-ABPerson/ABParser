using ABSoftware.ABParser.Events;
using ABSoftware.ABParser.Exceptions;
using ABSoftware.ABParser.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
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

        #region Main Data

        string _textAsString;
        public char[] Text;
        public int TextLength;
        public ABParserToken[] Tokens;

        public Stack<string> CurrentEventTokenLimits = new Stack<string>();
        public Stack<string> CurrentTriviaLimits = new Stack<string>();

        #endregion

        #region Internal Data

        /// <summary>
        /// A pointer to the actual parser in C++, this pointer can be used to get or set information.
        /// </summary>
        IntPtr _baseParser;

        bool EncounteredToken = true;
        bool EncounteredSecondToken = true;

        BeforeTokenProcessedEventArgs BeforeTokenProcessedArgs;
        OnTokenProcessedEventArgs OnTokenProcessedArgs;
        OnEndEventArgs OnEndArgs;

        TokenInformation OnTokenProcessedPreviousTokenInfo;
        TokenInformation OnTokenProcessedTokenInfo;
        TokenInformation CurrentEventTokenInfo;

        bool _disposedForNextParse = false;
        bool _disposeAtDestruction = false;
        bool _disposeAsyncronously = true;
        bool _currentlyDisposing = false;

        #endregion

        #region Internal Management

        internal void InitializeABParser(ABParserConfiguration config)
        {
            // Set the tokens.
            Tokens = config.Tokens;

            // Then, initialize the base parser.
            InitializeBaseParser(config);

            BeforeTokenProcessedArgs = new BeforeTokenProcessedEventArgs(this);
            OnTokenProcessedArgs = new OnTokenProcessedEventArgs(this);
            OnEndArgs = new OnEndEventArgs();

            OnTokenProcessedPreviousTokenInfo = new TokenInformation();
            OnTokenProcessedTokenInfo = new TokenInformation();
            CurrentEventTokenInfo = new TokenInformation();
        }

        internal void InitializeBaseParser(ABParserConfiguration tokens) => _baseParser = NativeMethods.CreateBaseParser(tokens.TokensStorage);

        internal void InitString(string text)
        {
            _textAsString = text;
            Text = text.ToCharArray();
            TextLength = text.Length;
            NativeMethods.InitString(_baseParser, text, TextLength);
        }

        internal void ResetInfo()
        {
            EncounteredToken = false;
            EncounteredSecondToken = false;
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

        internal unsafe int TwoShortsToInteger(ushort* data, int index) => (data[index] >> 16) + data[index + 1];

        internal unsafe void ShortsToString(ushort* data, int index, out char[] text)
        {
            // Get the length of the string.
            int length = TwoShortsToInteger(data, index);
            text = new char[length];

            // Go through and convert all of them to characters.
            index += 2;
            for (int i = 0; i < length; i++)
                text[i] = (char)data[index++];
        }

        unsafe void HandleResult(ContinueExecutionResult result, ushort* data)
        {
            if (result == ContinueExecutionResult.None) return;

            ResetTriviaStringCaches();
            MoveTokenInfos();

            // Get the trivia, which is always transmitted for everything.
            ShortsToString(data, 5, out var trivia);

            // Handle BeforeTokenProcessedArgs
            switch (result)
            {
                case ContinueExecutionResult.FirstBeforeTokenProcessed:

                    BeforeTokenProcessedArgs.PreviousToken = null;
                    goto case ContinueExecutionResult.OnThenBeforeTokenProcessed;

                case ContinueExecutionResult.OnThenBeforeTokenProcessed:

                    UpdateCurrentEventTokenInfo(data);
                    BeforeTokenProcessedArgs.CurrentToken = CurrentEventTokenInfo;
                    BeforeTokenProcessedArgs.Leading = trivia;

                    break;
            }

            OnTokenProcessedArgs.Leading = OnTokenProcessedArgs.Trailing;
            OnTokenProcessedArgs.Trailing = trivia;

            // Handle OnTokenProcessedArgs
            switch (result)
            {
                case ContinueExecutionResult.FirstBeforeTokenProcessed: return;
                case ContinueExecutionResult.StopAndFinalOnTokenProcessed:

                    // If this is a stop and we haven't had a single "BeforeTokenProcessed" yet, then we encountered no tokens and we shouldn't do anything.
                    if (!EncounteredToken) return;

                    OnTokenProcessedArgs.NextToken = null;
                    goto case ContinueExecutionResult.OnThenBeforeTokenProcessed;

                case ContinueExecutionResult.OnThenBeforeTokenProcessed:

                    OnTokenProcessedArgs.PreviousToken = EncounteredSecondToken ? OnTokenProcessedPreviousTokenInfo : null;
                    OnTokenProcessedArgs.CurrentToken = OnTokenProcessedTokenInfo;

                    break;
            }

            if (result == ContinueExecutionResult.OnThenBeforeTokenProcessed)
            {
                BeforeTokenProcessedArgs.PreviousToken = OnTokenProcessedTokenInfo;
                OnTokenProcessedArgs.NextToken = CurrentEventTokenInfo;
            }
        }

        unsafe void MoveTokenInfos()
        {
            TokenInformation swap = OnTokenProcessedPreviousTokenInfo;

            OnTokenProcessedPreviousTokenInfo = OnTokenProcessedTokenInfo;
            OnTokenProcessedTokenInfo = CurrentEventTokenInfo;
            CurrentEventTokenInfo = swap;
        }

        unsafe void ResetTriviaStringCaches()
        {
            BeforeTokenProcessedArgs.LeadingAsString = null;
            OnTokenProcessedArgs.LeadingAsString = null;
            OnTokenProcessedArgs.TrailingAsString = null;
        }

        unsafe void UpdateCurrentEventTokenInfo(ushort* data)
        {
            CurrentEventTokenInfo.Token = Tokens[data[0]];
            CurrentEventTokenInfo.Start = TwoShortsToInteger(data, 1);
            CurrentEventTokenInfo.End = TwoShortsToInteger(data, 3);
        }

        #endregion

        #region Main Execution

        internal async Task Execute()
        {
            // Don't do anything if there isn't any text to parse.
            if (TextLength == 0)
            {
                OnStart();
                OnEndArgs.Leading = new char[0];
                OnEnd(OnEndArgs);
            }

            // If we're currently disposing, then wait until we're done before moving on.
            while (_currentlyDisposing)
                await Task.Delay(1);

            DoExecute();
        }

        unsafe void DoExecute()
        {
            ushort* data = stackalloc ushort[TextLength + 8];

            // Trigger the "OnStart".
            OnStart();

            // This is how execution works on this side.
            // Quite simply, we will run "ContinueExecution", and that will do all of the work in C++.
            // Then, whenever the C++ code wants us to do something - like calling "OnTokenProcessed", it will return a result, and we will act on that.
            // After we've done that, we'll then just get it to continue execution.
            var result = ContinueExecutionResult.None;

            // Just keep on executing until we hit the "Stop" result.
            while (result != ContinueExecutionResult.StopAndFinalOnTokenProcessed)
            {
                HandleResult(result = NativeMethods.ContinueExecution(_baseParser, data), data);

                // Do whatever the result said to do.
                switch (result)
                {
                    case ContinueExecutionResult.StopAndFinalOnTokenProcessed:

                        if (!EncounteredToken)
                            break;

                        OnTokenProcessed(OnTokenProcessedArgs);
                        break;

                    case ContinueExecutionResult.FirstBeforeTokenProcessed:

                        EncounteredToken = true;
                        BeforeTokenProcessed(BeforeTokenProcessedArgs);

                        break;
                    case ContinueExecutionResult.OnThenBeforeTokenProcessed:

                        EncounteredSecondToken = true;

                        OnTokenProcessed(OnTokenProcessedArgs);
                        BeforeTokenProcessed(BeforeTokenProcessedArgs);

                        break;
                }
            }

            OnEndArgs.Leading = EncounteredToken ? OnTokenProcessedArgs.Trailing : Text;

            OnEnd(OnEndArgs);
            CurrentEventTokenLimits.Clear();
            CurrentTriviaLimits.Clear();

            // Finally, dispose all of the data - ready for the next parse.
            _disposedForNextParse = false;
            if (!_disposeAtDestruction)
                DisposeDataForNextParse();
        }

        #endregion

        #region Events

        protected virtual void OnStart() { }

        protected virtual void OnEnd(OnEndEventArgs args) { }

        /// <summary>
        /// Called before a token's trailing has been generated - mainly used to set limits.
        /// </summary>
        protected virtual void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args) { }

        /// <summary>
        /// Called when a token has been fully processed, and its leading and trailing have been generated.
        /// </summary>
        protected virtual void OnTokenProcessed(OnTokenProcessedEventArgs args) { }

        #endregion

        #region Public Methods

        public void SetText(string text) => SetTextAsync(text).Wait();

        public Task SetTextAsync(string text)
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

        public string GetTextAsString() => _textAsString = new string(Text);

        #endregion

        #region Constructor / Dispose

        protected ABParser(ABParserConfiguration config) => InitializeABParser(config);
        public async void Dispose()
        {
            while (_currentlyDisposing)
                await Task.Delay(1);
            if (_disposeAtDestruction && !_disposedForNextParse)
                DisposeDataForNextParse();
            NativeMethods.DeleteBaseParser(_baseParser);
        }

        #endregion

        #region Limits

        public void EnterTokenLimit(string limitName)
        {
            if (limitName.Length > 255) throw new ABParserNameTooLong();
            if (!NativeMethods.EnterTokenLimit(_baseParser, limitName, (byte)limitName.Length))
                throw new ABParserInvalidLimitName();

            CurrentEventTokenLimits.Push(limitName);
        }

        public void ExitTokenLimit(int amount = 1)
        {
            if (amount == 0) return;
            for (int i = 0; i < amount; i++)
            {
                if (CurrentEventTokenLimits.Count == 0) throw new ABParserExitNotInLimit();
                CurrentEventTokenLimits.Pop();
            }

            NativeMethods.ExitTokenLimit(_baseParser, amount);
        }

        public void EnterTriviaLimit(string limitName)
        {
            if (limitName.Length > 255) throw new ABParserNameTooLong();
            if (!NativeMethods.EnterTriviaLimit(_baseParser, limitName, (byte)limitName.Length))
                throw new ABParserInvalidLimitName();

            CurrentTriviaLimits.Push(limitName);
        }

        public void ExitTriviaLimit(int amount = 1)
        {
            if (amount == 0) return;
            for (int i = 0; i < amount; i++)
            {
                if (CurrentTriviaLimits.Count == 0) throw new ABParserExitNotInLimit();
                CurrentTriviaLimits.Pop();
            }

            NativeMethods.ExitTriviaLimit(_baseParser, amount);
        }

        #endregion
    }
}