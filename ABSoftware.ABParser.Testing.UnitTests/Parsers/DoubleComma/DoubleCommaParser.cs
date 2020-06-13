using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers.DoubleComma
{
    enum DoubleCommaTokens
    {
        SingleComma,
        DoubleComma,
        SingleQuote,
        DoubleQuote
    }

    public class DoubleCommaParser : ABParser
    {
        static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken(new ABParserText(nameof(DoubleCommaTokens.SingleComma)), new ABParserText(",")),
            new ABParserToken(new ABParserText(nameof(DoubleCommaTokens.DoubleComma)), new ABParserText(",,")),
            new ABParserToken(new ABParserText(nameof(DoubleCommaTokens.SingleQuote)), new ABParserText("\"")).SetLimits("DoubleQuoteString"),
            new ABParserToken(new ABParserText(nameof(DoubleCommaTokens.DoubleQuote)), new ABParserText("'")).SetLimits("SingleQuoteString")
        });

        public DoubleCommaParser() : base(ParserConfig) { }

        public DoubleCommaResult Result;

        DoubleCommaString CurrentStringItem;
        bool CurrentItemIsString;
        bool InString;

        protected override void OnStart()
        {
            Result = new DoubleCommaResult();

            // Add one array at the beginning if there is actual text within this double comma document.
            if (!string.IsNullOrWhiteSpace(Text.AsString()))
                AddArray();
        }

        protected override void OnTokenProcessed(OnTokenProcessedEventArgs args)
        {
            switch (args.Token.TokenName.AsString())
            {
                case nameof(DoubleCommaTokens.SingleComma):
                    HandleItem(args.Leading.AsString());
                    break;
                case nameof(DoubleCommaTokens.DoubleComma):
                    if (!string.IsNullOrWhiteSpace(args.Leading.AsString()))
                        HandleItem(args.Leading.AsString());

                    AddArray();
                    break;
                case nameof(DoubleCommaTokens.SingleQuote):
                case nameof(DoubleCommaTokens.DoubleQuote):

                    if (InString)
                    {
                        CurrentStringItem = new DoubleCommaString(args.Leading.AsString());
                        CurrentItemIsString = true;
                    }

                    InString = !InString;
                    break;
            }
        }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            if (InString) ExitTokenLimit();
            else
                switch (args.Token.TokenName.AsString())
                {
                    case nameof(DoubleCommaTokens.SingleQuote):
                        EnterTokenLimit("SingleQuoteString");
                        break;
                    case nameof(DoubleCommaTokens.DoubleQuote):
                        EnterTokenLimit("DoubleQuoteString");
                        break;
                }
        }

        void AddArray()
        {
            Result.Arrays.Add(new DoubleCommaArray());
        }

        void HandleItem(string leading)
        {
            DoubleCommaItem itemToAdd;

            if (CurrentItemIsString)
            {
                itemToAdd = CurrentStringItem;
                CurrentItemIsString = false;
            }
            else
            {
                var num = int.Parse(leading);
                itemToAdd = new DoubleCommaInt(num);
            }

            Result.Arrays.Last().Items.Add(itemToAdd);
        }
    }

}
