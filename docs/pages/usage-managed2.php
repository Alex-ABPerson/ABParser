<!DOCTYPE html>
<html>

<head>
    <title>Token Limits</title>
    <?php include('../../base/pageHeader.html') ?>
</head>

<body class="lightContainer">
    <?php include('../../base/pageBodyStart.html') ?>

    <h1 id="title">Limits + BeforeTokenProcessed</h1>
    <hr>
    <p>Let's look at how we can apply <i>limits</i> in our code.</p>

    <h2 id="btp">BeforeTokenProcessed</h2>
    <hr>
    <p>The limits must be set inside something called the <i>BeforeTokenProcessed</i>, otherwise you may get strange results you're not expecting.</p>
    <p>As you know, <code>OnTokenProcessed</code> gives us the leading, trailing and other token information (such as the next token). But, in order to generate the trailing (and get the next token), what ABParser technically does is parses ahead in the background up towards the next token before <b>then</b> triggering <code>OnTokenProcessed</code>.</p>
    <p>Whereas, <code>BeforeTokenProcessed</code> gets called <b>instantly</b> as soon as we hit the token, before it scans ahead to get the next token and trailing.</p>
    <p>We must set the limits in here, because if we don't, the parser will have already determined what the next token and trailing will be, and it will be too late to limit what it does next.</p>
    <p>To use the <code>BeforeTokenProcessed</code>, simply override <code>BeforeTokenProcessed</code>, and you'll find you have almost all the same things as the <code>OnTokenProcessed</code>, but not the trailing or next token.</p>

    <pre><code class="language-csharp">
protected override void BeforeTokenProcessed(BeforeTokenProcessedEventArgs args) { }
    </code></pre>

    <div class="msgBox infoBox">
		<h4 class="noAnchor">Information</h4>
		<p>Limits can also be set in <code>OnStart</code> too if necessary.</p>
	</div>

    <h2 id="tokenLimit-title">TokenLimit</h2>
    <hr>
    <p>Let's start by looking at how to use a TokenLimit in the code, to remind you, here's how its stack-based system works:</p>

    <div class="img-box">
        <img src="../pageImages/usageBasics/limitStack.png">
    </div>

    <h3 id="tokenLimit-addingLimits">Adding Limits</h3>
    <hr>
    <p>They're just like with the regular tokens, they require a bit of processing, so, we like to generate them once at application start, and leave them there.</p>
    <p>In order to remove clutter, we configure them alongside the tokens.</p>
    <p>To do this, right after the <code>new ABParserToken(params)</code>, we can do <code>.SetLimits(limitNames)</code>.</p>
    <p>Then, within the brackets we simply give it a name, e.g. <code>new ABParserToken(params).SetLimits("Limit")</code>. You can add it to multiple limits by simply providing multiple strings to the method, like <code>AddToLimit("Limit1", "Limit2")</code>.</p>
    <p>Below is an example, which will create three different <code>TokenLimit</code>s. <code>QuoteLimit</code>, with the quote token in it, <code>QuestionLimit</code>, with the question token in it, and <code>BothLimit</code>, which contains both:</p>

    <pre><code class="language-csharp">
static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
{
    new ABParserToken("QUOTE"), "'").SetLimits("QuoteLimit", "BothLimit"),
    new ABParserToken("QUESTION"), "?").SetLimits("QuestionLimit", "BothLimit")
});
    </code></pre>

    <h3 id="tokenLimit-using">Using TokenLimits</h3>
    <hr>

    <p>We call <code>EnterTokenLimit(limitName)</code> to <i>enter</i> into a TokenLimit, and <code>ExitTokenLimit</code> to <i>exit</i> one.
    <p>If we do the following:</p>

    <pre><code class="language-csharp">
EnterTokenLimit("Limit1");
EnterTokenLimit("Limit2");
    </code></pre>

    <p>
        We just pushed <code>Limit1</code> onto the TokenLimit's stack, then pushed <code>Limit2</code> onto the TokenLimit's stack, so <code>Limit2</code> is at the top.</p>
        That means that we're inside the limit <code>Limit2</code>, meaning whatever tokens are in the <code>Limit2</code> are the ones we'll be listening out for.</p>
        However, if we then do:
    </p>

    <pre><code class="language-csharp">
ExitTokenLimit();
    </code></pre>

    <p>
        We've now taken the <code>Limit2</code> off the stack, so now we're limited to whatever is in <code>Limit1</code>. And, if we call <code>ExitTokenLimit</code> yet again, then we'll have removed everything off the stack and ABParser will go back to normal without any limits on what tokens it detects.</p>
        Remember that <i>all</i> these things need to be called from the <code>BeforeTokenProcessed</code>.
    </p>

    <h2 id="triviaLimit-using">TriviaLimit</h2>
    <hr>
    <p>
        The <code>TriviaLimit</code> can be used to exclude certain characters from the trivia.</p>
        We <i>use</i> the <code>TriviaLimit</code>s exactly the same as using the <code>TokenLimit</code>, with the <code>EnterTriviaLimit</code> and <code>ExitTriviaLimit</code>.</p>
        The biggest differences is how we add <code>TriviaLimit</code>s.
    </p>

    <h3 id="triviaLimit-addingLimits">Adding Limits</h3>
    <hr>
    <p>
        Adding <code>TriviaLimit</code>s is also done on the <code>ABParserConfiguration</code>, to save it being re-processed repeatedly as the parser is constructed.</p>
        Before we add any limits, the first thing we need to do is tell ABParser how many TriviaLimits we're going to add. You can find this as the last parameter on the <code>ABParserConfiguration</code> constructor.</p>
        Then, to actually add the TriviaLimits, we simply chain <code>AddTriviaLimit</code>s off the end of the <code>new ABParserConfiguration</code>.</p>
        The very first parameter <code>AddTriviaLimit</code> takes in is a boolean, and if this is false, then this limit will be blacklisting (meaning the trivia <b>won't</b> include the given characters), and if it's true, it will be whitelisting (meaning the trivia will <i>only</i> contain the given characters).</p>
        Then, you follow that by the name you want to give this limit, and all future parameters after that are <i>which</i> characters we want to put within this limit.</p>
        Here's an example:
    </p>

    <pre><code class="language-csharp">
static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
{
    // Tokens
}, 2).AddTriviaLimit(false, "NoWhitespace", ' ', '\r', '\n', '\t').AddTriviaLimit(true, "OnlyCs", 'C', 'c');
    </code></pre>

    <p>
        This adds two trivia limits, one called <code>NoWhitespace</code> that contains 4 whitespace characters, and excludes them from the trivia, and the other is called <code>OnlyCs</code>, that will cause the trivia to ONLY contain capital C and lowercase c.</p>
        This now means that we can enter into one of these trivia limits, and those characters will <i>not</i> be included in the trivia.
    </p>

    <h4 id="triviaLimit-OnFirstUnlimitedCharacter">OnFirstUnlimitedCharacterProcessed</h4>
    <hr>
    <p>
        This is the <b>final</b> event in ABParser before we know everything! This gets triggered as soon as we reach the very <i>first</i> character in the text that isn't in the current TriviaLimit.</p>
        It doesn't matter whether the character is part of a token or just a character that will go into trivia, as long as it's a character that isn't being excluded by the current TriviaLimit in some way, this will get triggered.</p>
        Why is this useful? Typically, you initialize all of your variables in <code>OnStart</code>, however, sometimes you <i>only</i> want to do your initialization if there is actually anything in the text you're parsing.</p>
        Depending on what you're parsing, sometimes you can <i>easily</i> detect whether there's anything in the text based on whether there's a token in there, but sometimes you may have a format where it's perfectly possible to have no tokens but still have something inside the text.</p>
        So, an example of what you can do with this, is you can set a <code>TriviaLimit</code> in <code>OnStart</code>, set to ignore whitespace (for example), and then put all of your initialization code within <code>OnFirstUnlimitedCharacterProcessed</code>.</p>
        Now, when you have text that's just whitespace or empty, you won't initialize anything, everything will only get initialized when we've confirmed that there's actual text there.</p>
        In the next part, we'll look at making a parser that needs exactly this!</p>
        Here's how it's overriden:
    </p>

    <pre><code class="language-csharp">
protected override void OnFirstUnlimitedCharacterProcessed(int pos) { }
    </code></pre>

    <h2 id="triviaLimit-using">DetectionLimit</h2>
    <hr>
    <p>
        These limits tell ABParser to ignore certain characters when checking for certain tokens.</p>
        These limits are applied to <i>each</i> token, and can't be enabled or disabled.</p>
        Similar to the <code>TokenLimit</code>s, we can apply these just by calling the <code>SetDetectionLimits</code> method on the tokens as we declare them.</p>
        Like here:
    </p>

    <pre><code class="language-csharp">
static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
{
    // ...
    new ABParserToken("the").SetDetectionLimits(' ', '\t', '\r', '\n'),
    // ...
});
    </code></pre>

    <p>
        Now, whenever we check for this token, if there are any of those characters provided in-between some of the characters on the token, they will be ignored and the token will still be detected.
    </p>
    
    <?php include('../../base/pageBodyEnd.html') ?>
</body>
</html>