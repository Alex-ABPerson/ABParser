<!DOCTYPE html>
<html>

<head>
    <title>Installation</title>
    <?php include('../../base/pageHeader.html') ?>
</head>

<body class="lightContainer">
    <?php include('../../base/pageBodyStart.html') ?>

    <h1 id="title">Installation</h1>
    <hr>
    <p><b>NOTE:</b> This guide assumes you are using Visual Studio.</p>
    <p>The first step is, to install ABParser, here are some ways of doing this:</p>

    <h2 id="nuget">NuGet</h2>
    <hr>
    <p>
        <b>Coming soon</b><br><br>
        You can install ABParser from directly within Visual Studio simply by using NuGet, so, here's how to do it:
    </p>

    <ol>
        <li>Right click on your project in the Solution Explorer.</li>
        <li>Choose "Manage NuGet packages".</li>
        <li>Go to the "Browse" tab.</li>
        <li>Search "ABParser" and select "ABSoftware.ABParser".</li>
        <li>Click on "Install".</li>
        <li>Accept the license agreement.</li>
    </ol>

    <p>And that's it! Simple.</p>

    <h2 id="manually">Manually</h2>
    <hr>
    <b>Coming soon</b><br><br>
    <p>However, if for whatever reason you need to install it manually, then you can do that, but it does take a bit of effort because the core part is written in C++, so it needs to have those libraries accessible alongside the EXE file.</p>

    <ol>
        <li>Download the ZIP file here (TODO: Add URL)</li>
        <li>Navigate to your project (the one you want to install ABParser into) in the file explorer - where the csproj is, not the sln file</li>
        <li>Then, extract the "ABSoftware.ABParser.dll" file out.</li>
        <li>In Visual Studio, right click on "References" under your project in the Solution Explorer</li>
        <li>Choose "Add Reference"</li>
        <li>Choose "Browse" and navigate to the DLL file we just extracted</li>
        <li>Then make sure it's ticked and choose "OK".</li>
        <li>Now, navigate to your <b>main binary</b> directory - the one where the EXE file is, NOT the DLL if it's a class library you're installing to (typically "bin\Debug").</li>
        <li>Copy all of the files in the "core" folder into there - not the folder itself, just the files in that folder.</li>
    </ol>
    <p>That will have installed ABParser!</p>

    <div class="msgBox warningBox">
		<h4 class="noAnchor">Warning</h4>
		<p> If your program is split into multiple projects, make sure you put the core files inside the binary directory <b>with the actual executable file.</b></p>
	</div>

    <?php include('../../base/pageBodyEnd.html') ?>
</body>
</html>