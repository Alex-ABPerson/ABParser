<!DOCTYPE html>
<html>

<head>
    <title>Usage</title>
    <?php include('../../base/pageHeader.html') ?>
</head>

<body class="lightContainer">
    <?php include('../../base/pageBodyStart.html') ?>

    <h1 id="title">Usage</h1>
    <hr>
    <p>So, here's how we'll guide you on ABParser:</p>

    <ol>
        <li>You'll go through the basics of ABParser, regardless of whether you'll use the unmanaged or managed version.</li>
        <li>You'll then (or now if you already know the basics) decide on whether you want to know about the unmanaged or managed version, and we'll guide you from there.</li>
    </ol>

    <p>
        So, now is a good time to decide which side of ABParser you need to actually learn about. <br><br>
        ABParser is split into two parts:
    </p>
    <ul>
        <li>The managed side (C#)</li>
        <li>The unmanaged side (C++)</li>
    </ul>

    <p>In this document we'll break down what each of these are, so that way you can determine either which one you are using or which one you wish to use.</p>

    <h2 id="managed">Managed</h2>
    <hr>
    <p>
        The managed side is a wrapper of the unmanaged side, written in C#, meaning in the background it actually uses the unmanaged side.<br><br>
        This can be used in only C#. So, if you want to use ABParser from C#, then the managed is the one to use.<br><br>
    </p>

    <h2 id="unmanaged">Unmanaged</h2>
    <hr>
    <p>
        The unmanaged part is the part of ABParser that is written in C++.<br><br>
    </p>

    <h2 id="next">Next</h2>
    <hr>
    <p>Now, decide where you need to go - if you want to learn the basics of ABParser first, do that here:</p>
    
    <div class="navBoxContainer">
        <div data-navigates="child" data-navigateTo="The Basics" class="navBox navBoxLight navBox-half">
            <div class="navBoxImgContainer">
                <img class="navBoxImg" src="../pageImages/TheBasics.png">
            </div>
            <h1 class="noAnchor">ABParser Basics</h1>
            <p>This will cover all of the basics on how to use ABParser.</p>
        </div>
    </div>

    <p>Or, if you already know those and want to jump straight into the documentation for a specific side, do that below:</p>
    <div class="navBoxContainer">
        <div data-navigates="child" data-navigateTo="Managed" class="navBox navBoxLight navBox-half">
            <h1 class="noAnchor">Managed</h1>
            <p>The managed side of ABParser - for C#.</p>
        </div>
        <div data-navigates="child" data-navigateTo="Unmanaged" class="navBox navBoxLight navBox-half">
            <h1 class="noAnchor">Unmanaged</h1>
            <p>The unmanaged side of ABParser - for C++.</p>
        </div>
    </div>
    <?php include('../../base/pageBodyEnd.html') ?>
</body>

</html>