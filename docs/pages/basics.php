<!DOCTYPE html>
<html>

<head>
    <title>Basics</title>
    <?php include('../../base/pageHeader.html') ?>
</head>

<body class="lightContainer">
    <?php include('../../base/pageBodyStart.html') ?>

    <h1 id="title">The Basics</h1>
    <hr>
    <p>Let's cover all the basic concepts that ABParser is built upon.</p>

    <h2 id="tokens">Tokens</h2>
    <hr>
    <p>
        The most fundamental things when it comes to using ABParser are the tokens.<br><br>
        Essentially, a token is a specific piece of text you want ABParser to look out for, it can be multiple characters long or just a single character long. For example, in JSON it would be the quotes, braces, commas and colons.<br><br>
        So, what happens is when you create an ABParser, you choose all of the tokens that ABParser will look out for. And, when ABParser hits that token, it will notify you, and you can handle it.<br><br>
    </p>

    <h2 id="trivia">Trivia</h2>
    <hr>
    <p>
        But then what happens to everything that <i>isn't</i> a token? Well, it becomes trivia, or more specifically the leading and trailing.<br><br>
        So, when ABParser encounters a token, you can then look at the trivia around it - that's how it works.<br><br>
        Trivia is split down into two parts - the leading and trailing. When ABParser encounters a token and notifies you, you'll have access to both of these.
    </p>

    <h3 id="leading">Leading</h3>
    <hr>
    <p>
        The leading is the text that came before the current token, starting at the last token.<br><br>
        If this is the first token in the string, then it will be starting from the beginning of the text.<br><br>
        In the diagram below, in red are the tokens, and in green is the leading.<br><br>
    </p>
    <div class="img-box">
        <img src="../pageImages/usageBasics/LeadingDiagram.png">
    </div>

    <h3 id="trailing">Trailing</h3>
    <hr>
    <p>
        The trailing is the opposite - it is the text AFTER the token going up towards the next token.<br><br>
        And this right here is a sign of how powerful ABParser is. You can look ahead of where you are with no effort!<br><br>
        If this is the last token in the string, then it will end at the end of the text.<br><br>
        Once again, the diagram below shows what the trailing is - using red as the tokens and the green at the trailing.<br><br>
    </p>
    <div class="img-box">
        <img src="../pageImages/usageBasics/TrailingDiagram.png">
    </div>

    <h2 id="limits">Limits</h2>
    <hr>
    <p>
        And, already we know almost all of ABParser. If you want to build a full parser with ABParser, you need to know about the limits that ABParser provides.<br><br>
        Just like the tokens, we define these limits once when we create the parser. In the case of the <b>TokenLimit</b> and <b>TriviaLimit</b>, we give them a name, and we can then <i>apply</i> them by name as the parser runs.
        There are three different limits in ABParser, and they limit ABParser to only pay attention to certain tokens or characters.<br><br>
        We can turn these limits on and off as the parser runs, sometimes through a stack-based system, as you'll see below.
    </p>

    <table class="docs-table">
		<tr>
			<th>Name</th>
			<th>Function</th>
		</tr>
		<tr>
			<td>TokenLimit</td>
			<td>This will tell ABParser to <i>only</i> listen out for a specific sub-set of the tokens, used very commonly.</td>
		</tr>
        <tr>
			<td>TriviaLimit</td>
			<td>This will tell ABParser to either ignore or only allow certain characters into the trivia.</td>
		</tr>
        <tr>
			<td>DetectionLimit</td>
			<td>This is applied per token, and will tell ABParser to <i>ignore</i> certain characters when scanning for tokens.</td>
		</tr>
	</table>

    <h3 id="limits-stackBased">Stack-Based Limits</h3>
    <hr>
    <p>
        The <b>TokenLimit</b> and the <b>TriviaLimit</b> are stack-based, to provide more flexibility for more complex formats.<br><br>
        This means which TokenLimits/TriviaLimits are currently applied are stored as a stack. And whatever is at the top of the stack is what limit takes effect.<br><br>
        We can <i>enter</i> into a limit, which will <i>push</i> that limit onto the stack. And, we can <i>exit</i> a limit, which will <i>pop</i> that limit off the top of the stack.<br><br>
        And, when there is <i>nothing</i> on the stack, there is <i>no</i> limit, so ABParser will just allow everything.
    </p>
    <div class="img-box">
        <img src="../pageImages/usageBasics/limitStack.png">
    </div>

    <h3 id="tokenLimit-title">TokenLimit</h3>
    <hr>
    <p>
        When a <code>TokenLimit</code> is applied, only the tokens in that limit will be detected.<br><br>
        Any tokens that aren't in the limits are completely ignored, and will be put into the trivia as if they weren't tokens.<br><br>
        An example of how this is useful, is if you had a format that has a string literal. When you've hit the first quote of that string literal, the <i>only</i> token you want to be notified of is the ending quote, so you could enter into a limit that only listens out for the ending string quote, and exit that limit when you do hit the ending string quote.
    </p>

    <h3 id="tokenLimit-title">TriviaLimit</h3>
    <hr>
    <p>
        This can useful at times, with this we can tell ABParser to whitelist or blacklist certain characters in the trivia.<br><br>
        This means the <code>Leading</code> and <code>Trailing</code> won't contain given characters or will be limited to contain <b>only</b> given characters.<br><br>
        This could be useful to exclude something such as <i>whitespace</i> from the trivia, as that could commonly get in the way of understanding the trivia.
    </p>

    <h3 id="detectionLimit-title">DetectionLimit</h3>
    <hr>
    <p>
        This is applied on <i>each</i> token, and tells ABParser to ignore certain characters in the middle of tokens, and still detect them.<br><br>
        For example, if you had the token "the", if you set a DetectionLimit on that token that contains a " ", then entering: "the", "t he", "th e", "t h e" etc. will all still be detected as just the token "the".<br><br>
        There are not <i>many</i> places you would want to use this. However, one example is in pre-processor definitions. Because pre-processor definitions can be written as:
    </p>

    <pre><code class="language-cpp">
#define ABC
#    define ABC
    #define ABC
    </code></pre>

    <p>
        All of these are valid. The "#" has to always be at the start of the line, but it can be indented forwards.<br><br>
        So, to achieve this, we could make the "#" token become "\n#", that way it will only work at the start of a line, then tell ABParser to ignore whitespace in the middle of the token (excluding the '\n'), allowing indenting it forward with whitespace to still work.<br><br>
        And everything is handled for you!
    </p>

    <h2 id="next">Next</h2>
    <hr>
    <p>That's it! That's all there is to it: the tokens, trivia and limits! Now, we're ready to start writing some code, so, to do that, simply choose what part of ABParser you want to use, and we'll guide you from there!</p>

    <div class="navBoxContainer">
        <div data-navigates="alongside" data-navigateTo="Managed" class="navBox navBoxLight navBox-half">
            <h1 class="noAnchor">Managed</h1>
            <p>The managed side of ABParser - for C#.</p>
        </div>
        <div data-navigates="alongside" data-navigateTo="Unmanaged" class="navBox navBoxLight navBox-half">
            <h1 class="noAnchor">Unmanaged</h1>
            <p>The unmanaged side of ABParser - for C++.</p>
        </div>
    </div>

    <?php include('../../base/pageBodyEnd.html') ?>
</body>
</html>