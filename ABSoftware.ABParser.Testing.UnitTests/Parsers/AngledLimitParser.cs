using ABSoftware.ABParser.Events;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ABSoftware.ABParser.Testing.UnitTests.Parsers
{
    // < - Enters into the angled information.
    // << - Enters into the second level of the angles.
    // <<< - Enters into the third level of the angles.
    // ! - Completely exits all of the angles. (Only valid when one level deep)
    // > or >> - Goes back up an angle.
    // ? - Only valid in the second level and deeper.
    public class AngledLimitParser : TrackingParser
    {
        int CurrentLevel = 0;

        static readonly ABParserConfiguration ParserConfiguration = new ABParserConfiguration(new ABParserToken[]
        {
            new ABParserToken(new ABParserText("<")).SetLimits("Outside"),
            new ABParserToken(new ABParserText("<<")).SetLimits("FirstLevel"),
            new ABParserToken(new ABParserText("<<<")).SetLimits("SecondLevel"),
            new ABParserToken(new ABParserText("?")).SetLimits("SecondLevel", "ThirdLevel"),
            new ABParserToken(new ABParserText("!")).SetLimits("FirstLevel", "SecondLevel", "ThirdLevel"),
            new ABParserToken(new ABParserText(">")).SetLimits("FirstLevel", "SecondLevel", "ThirdLevel"),
            new ABParserToken(new ABParserText(">>")).SetLimits("FirstLevel", "SecondLevel", "ThirdLevel")
        });

        public AngledLimitParser() : base(ParserConfiguration) { }

        protected override void OnStart()
        {
            base.OnStart();
            EnterTokenLimit("Outside");
        }

        protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args)
        {
            base.BeforeTokenProcessed(args);
            switch (args.Token.TokenName.AsString())
            {
                case "<":
                    CurrentLevel = 1;
                    EnterTokenLimit("FirstLevel");
                    break;
                case "<<":
                    CurrentLevel = 2;
                    EnterTokenLimit("SecondLevel");
                    break;
                case "<<<":
                    CurrentLevel = 3;
                    EnterTokenLimit("ThirdLevel");
                    break;
                case "!":
                    ExitTokenLimit(CurrentLevel);
                    CurrentLevel = 0;
                    break;
                case ">":
                case ">>":
                    ExitTokenLimit();
                    CurrentLevel--;
                    break;
            }
        }
    }
}
