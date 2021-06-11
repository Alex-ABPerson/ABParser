<!DOCTYPE html>
<html>

<head>
    <title>Precise Tokens + Extra Features</title>
    <?php include('../../base/pageHeader.html') ?>
</head>

<body class="lightContainer">
    <?php include('../../base/pageBodyStart.html') ?>

    <h1 id="title">Precise Tokens + Extra Features</h1>
    <hr>
    <p>
        We'll cover extra features ABParser has, but first, it's important that we talk about providing precise tokens.
    </p>

    <h2 id="preciseTokens">Precise Tokens</h2>
    <hr>
    <p>
        ABParser is an incredible library, and it's able to work out the correct tokens in most cases, but it isn't magic, and there are some very, very specific cases where it is logically impossible for it to detect the correct tokens.<br><br>
        Let's imagine we have the tokens "the", "they" and "theyare", and we arrange them like this in a string.
    </p>

    <pre><code>
AtheBtheyCtheyarDtheyareE
    </code></pre>

    <p>
        Even though the tokens overlap, ABParser is very smart and can work out what tokens there are (the, they, they, theyare), this is one of the reasons this library was made, handling this is very difficult annoying without it.<br><br>
        However, there are some very specific places, where it's impossible to know which token it's supposed to be, for example, if we have the tokens "they" and "heya", and we have:
    </p>

    <pre><code>
theya
    </code></pre>

    <p>
        Generally, ABParser will just give you the first token it sees (they) in this case, but that's not always what you'll be looking for.<br><br>
        So, it's good to put in tokens <i>as specific</i> as you can get. More specific tokens means less room for error, and means ABParser has to do less work, which can help improve performance as well.<br><br>
    </p>

    <h2 id="escaping-title">Escaping</h2>
    <hr>
    <p>The final major feature ABParser has is the ability to escape tokens, which is common in most formats (where placing a character before a token will cause it to not count).</p>

    <h3 id="escaping-adding">Adding</h3>
    <p>
        Adding different escape characters can be done just like <code>TriviaLimits</code>. You write <code>AddEscapeCharSet</code> after the configuration, giving a name and a list of the characters you want to have count as escape characters.<br><br>
        However, just like with the <code>TriviaLimits</code>, you must say <i>how many</i> escape characters there are. This is the <b>third</b> parameter on the constructor. So, you'll need to provide "0" for the <code>TriviaLimits</code> if you don't have any.
    </p>

    <pre><code class="language-csharp">
static readonly ABParserConfiguration ParserConfig = new ABParserConfiguration(new ABParserToken[]
{
    ...
}, 0, 1).AddEscapeCharSet("Name", '\\', 'a');
    </code></pre>
    <p>Adding different escape characters can be done just like <code>TriviaLimits</code>. You write <code>AddEscapeCharSet</code> after the configuration, giving a name and a list of the characters you want to have count as escape characters.</p>
</body>
</html>