<!DOCTYPE html>
<html lang="en">
<head>
    <title> ABParser Docs </title>
    <?php include('../base/docsHead.html') ?>

    <script>
        StartPath = "pages/start.php";

        Sections = [
            //new Section("Firsttt", "pages/1.php", "https://abworld.ml/img/artworkShowcase/jtsshieh/PaintBrush.png", [
            new Section("Usage", "pages/usage.php", "../img/Usage.png", [
                new Section("The Basics", "pages/basics.php", "pageImages/TheBasics.png", []),
                new Section("Managed", "pages/usage-managed.php", "", [
                    new Section("Installation", "pages/usage-managed0.php", "", []),
                    new Section("Creating an ABParser", "pages/usage-managed1.php", "", []),
                    new Section("Limits + BeforeTokenProcessed", "pages/usage-managed2.php", "", []),
                    new Section("Double Comma Parser", "pages/usage-managed3.php", "", []),
                    new Section("Precise Tokens + Extra Features", "pages/usage-managed4.php", "", []),
                ])
            ]),
            new Section("Development", "pages/development.php", "../img/Development.png", [

            ])
        ];
    </script>
</head>
<body class="lightTheme">

    <?php include('../base/docsBody.html') ?>

</body>
</html>