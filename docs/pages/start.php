<!DOCTYPE html>
<html>

<head>
    <title>Start</title>
    <?php include('../../base/pageHeader.html') ?>
</head>

<body class="lightContainer">
    <?php include('../../base/pageBodyStart.html') ?>

    <h1 id="title">ABParser Docs</h1>
    <hr>
    <p>
        Welcome to the ABParser documentation! There is documentation about how to use ABParser (both managed and unmanaged versions), as well as how they work for contributing to ABParser.<br><br>
        ABParser is a tool that you can use to parse (read) through text easily. It can be used for anything that needs to read through some text and process it, here are some examples:
    </p>

    <ul>
        <li>Deserializer - Something such as a JSON deserializer can use ABParser to read through the JSON.</li>
        <li>Minifier - A minifier could use ABParser to read through the text to minify.</li>
        <li>Compiler - You could use ABParser to make a compiler! ABParser would be in charge of reading through the code to compile.</li>
    </ul>

    <h2 id="whyUseABParser">Why use ABParser?</h2>
    <hr>
    <p>Why use ABParser? There are two major reasons why.</p>

    <h3 id="whyUse-Effort">Effort</h3>
    <hr>
    <p>Implementing this sort of system yourself may appear to be little work. However, there is much more to it than there first seems to be.<br><br></p>
    <ul>
        <li>If you're going through character-by-character, dealing with multi-character constructs can be very difficult.</li>
        <li>How will you handle overlapping things? If you want to detect the text "the" AND the text "they", you would have to look ahead when you reach the text "the", in order to determine whether it's really "the" or "they".</li>
        <li>If you decide you now want to escape characters, you have implement that as well.</li>
    </ul>
    <p>
        And there are many more, and putting all these alongside the real logic for your parser will get messy.<br><br>
        ABParser handles <b>all</b> of these in the background so that you can focus on the actual data processing.<br><br>
        And, in addition to that, it's very easy to use, it only takes 15 minutes to read the documentation and understand how to use it!
    </p>

    <h3 id="whyUse-Performance">Performance</h3>
    <hr>
    <p>
        ABParser is designed to be very fast and as efficient as possible. It always reads through the text from start the end, and it makes as few heap allocations as possible as it runs.<br><br>
        But in addition to that, if you're using it in C#, the core of ABParser is written in C++! And C++ runs much faster than C# code would.
    </p>

    <h2>Getting Started</h2>
    <p>The first step is to choose below what type of documentation you want, you can alternatively use the menu to quickly navigate to a specific page.</p>
    
    <div class="navBoxContainer">
        <div data-navigates="child" data-navigateTo="Usage" class="navBox navBoxLight navBox-half">
            <div class="navBoxImgContainer">
                <img class="navBoxImg" src="../pageImages/Usage.png">
            </div>
            <h1 class="noAnchor">Usage</h1>
            <p>This is how to use ABParser and all its features.</p>
        </div>
        <div data-navigates="child" data-navigateTo="Development" class="navBox navBoxLight navBox-half">
            <div class="navBoxImgContainer">
                <img class="navBoxImg" src="../pageImages/Development.png">
            </div>
            <h1 class="noAnchor">Development</h1>
            <p>This will tell you how the source code of ABParser works.</p>
        </div>
    </div>

    <?php include('../../base/pageBodyEnd.html') ?>
</body>

</html>