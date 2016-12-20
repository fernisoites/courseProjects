<?php
	session_start();
	if(!isset($_SESSION['username'])){
		$home_url = 'login.php';
    	header('Location: '.$home_url);
    }
?>
<!DOCTYPE HTML>
<!--
	Alpha by HTML5 UP
	html5up.net | @n33co
	Free for personal and commercial use under the CCA 3.0 license (html5up.net/license)
-->
<html>
	<head>
		<title>UTickets</title>
		<meta charset="utf-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1" />
		<!--[if lte IE 8]><script src="assets/js/ie/html5shiv.js"></script><![endif]-->
		<link rel="stylesheet" href="assets/css/member.css" />
		<!--[if lte IE 8]><link rel="stylesheet" href="assets/css/ie8.css" /><![endif]-->
					<!--<style>
                    #example1{
                        background: url(images/background.jpg);
                        background-position: left top;
                        background-repeat: no-repeat;
                    }
                    </style>
                -->

<script>
function showUser(str) {
    if (str == "") {
        document.getElementById("txtHint").innerHTML = "";
        return;
    } else { 
        if (window.XMLHttpRequest) {
            // code for IE7+, Firefox, Chrome, Opera, Safari
            xmlhttp = new XMLHttpRequest();
        } else {
            // code for IE6, IE5
            xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        xmlhttp.onreadystatechange = function() {
            if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
                document.getElementById("txtHint").innerHTML = xmlhttp.responseText;
            }
        };
        xmlhttp.open("GET","getuser.php?q="+str,true);
        xmlhttp.send();
    }
}
</script>



	</head>
	<div id="page-wrapper">
	<body class="landing">
	

            
			<!-- Header -->
				<header id="header">
					<img src="images/bird_ticket.png", style="width:80px">

					<h1><font size="4"><a href="index.html"><ul><ul><ul><ul><ul>UTickets</font></h1>
					<nav id="nav">
						<ul>
							<li><a href="">Welcome, <?php echo $_SESSION['username'];?></a></li>
							<li><a href="index.php">Home</a></li>
							<li>
							<li><a href="about.php">About Us</a></li>
							<li>
							<li><a href="logout.php" class="button">Log Out</a></li>
						</ul>
					</nav>

				</header>
            
			<!-- Banner -->
				<section id="banner">
					
					<?php
	echo "<h2>Hello, " .$_SESSION['username']. " !</h2>";
?>

					<p>We have some recommendations for you.  </p>
					<ul class="actions">
						<li><a href="#" class="button special">I want to buy</a></li>
						<li><a href="#" class="button">I want to sell</a></li>
					</ul>





<script>
function showUser(str) {
    if (str == "") {
        document.getElementById("txtHint").innerHTML = "";
        return;
    } else { 
        if (window.XMLHttpRequest) {
            // code for IE7+, Firefox, Chrome, Opera, Safari
            xmlhttp = new XMLHttpRequest();
        } else {
            // code for IE6, IE5
            xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        xmlhttp.onreadystatechange = function() {
            if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
                document.getElementById("txtHint").innerHTML = xmlhttp.responseText;
            } 
        };
        xmlhttp.open("GET","getuser.php?q="+str,true);
        xmlhttp.send();
    }
}
showUser('adf@fad.com');
</script>


				</section>

			<!-- Main -->
				<section id="main" class="container">


					<div class="row">
						<div class="6u 12u(narrower)">

							<section class="box special">
									<ul class="actions fit">
										<li><a href="#" class="button special big">Recommended Football Games</a></li>
									</ul>
								<table style="width:100%">
								  <tr>
								    <td>THUNDERBIRDS vs UTES</td> 
								    <td>09/01/16</td>
								    <td><font size="3" color="red">From $19</font></td>
								  </tr>
								  <tr>
								    <td>WILDCATS vs CARDINAL</td> 
								    <td>09/01/16</td>
								    <td><font size="3" color="red">From $19</font></td>
								  </tr>
								  <tr>
								    <td>UVA vs VT</td> 
								    <td>09/01/16</td>
								    <td><font size="3" color="red">From $199</font></td>
								  </tr>
								</table>
		
							</section>

						</div>
						<div class="6u 12u(narrower)">

							<section class="box special">

								<h3>Hot Comments</h3>

								<p align="left"><font size="3" color="blue">Alfred</font>: The UVA - VT game is a must go. I have researched arround, there is no place where you can find ticket at such a low price, even not anywhere close to this price. </p>
		
								<p align="left"><font size="3" color="blue">Weaver</font>: Absolutely agree with Alfred. The UVA - VT game is a must go. The loweset price I could find from other places is at least double the price. </p>					
							</section>

						</div>
					</div>

					<div class="row">
						<div class="6u 12u(narrower)">

							<section class="box special">
									<ul class="actions fit">
										<li><a href="#" class="button special big">Recommended Basketball Games</a></li>
									</ul>
								<table style="width:100%">
								  <tr>
								    <td>THUNDERBIRDS vs UTES</td> 
								    <td>09/01/16</td>
								    <td><font size="3" color="red">From $19</font></td>
								  </tr>
								  <tr>
								    <td>WILDCATS vs CARDINAL</td> 
								    <td>09/01/16</td>
								    <td><font size="3" color="red">From $19</font></td>
								  </tr>
								  <tr>
								    <td>UVA vs VT</td> 
								    <td>09/01/16</td>
								    <td><font size="3" color="red">From $199</font></td>
								  </tr>
								</table>
		
							</section>

						</div>
						<div class="6u 12u(narrower)">

							<section class="box special">

								<h3>Hot Comments</h3>

								<p align="left"><font size="3" color="blue">TA1</font>: The UVA - VT game is a must go. I have researched arround, there is no place where you can find ticket at such a low price, even not anywhere close to this price. </p>
		
								<p align="left"><font size="3" color="blue">TA2</font>: Absolutely agree with Alfred. The UVA - VT game is a must go. The loweset price I could find from other places is at least double the price. </p>					
							</section>

						</div>
					</div>
<!--
					<section class="box special features">
						<div class="features-row">
							<section>
								
								<img src="images/football.jpg", style="width:380px;height:270px" >
								<h4>Wanna watch Football games? Click here!</h4>

							</section>
							<section>
								<img src="images/basketball.jpg", style="width:380px;height:270px">
								<h4>Wanna watch Basketball games? Click here!</h4>

							</section>
						</div>

					</section>
-->
				</section>

			<!-- CTA -->
				<!--<section id="cta">

					</ul>
					<!--<h2>Sign up for UTickets</h2>
					<p>Can't wait to sell or buy tickets? Sign up now!.</p>

					<form>
						<div class="row uniform 50%">
							<div class="8u 12u(mobilep)">
								<input type="email" name="email" id="email" placeholder="Email Address" />
							</div>
							<div class="4u 12u(mobilep)">
								<input type="submit" value="Sign Up" class="fit" />
							</div>
						</div>
					</form>-->

				</section>

			<!-- Footer -->
				<footer id="footer">
					<ul class="icons">
						<li><a href="#" class="icon fa-twitter"><span class="label">Twitter</span></a></li>
						<li><a href="#" class="icon fa-facebook"><span class="label">Facebook</span></a></li>
						<li><a href="#" class="icon fa-instagram"><span class="label">Instagram</span></a></li>
						<li><a href="#" class="icon fa-github"><span class="label">Github</span></a></li>
						<li><a href="#" class="icon fa-dribbble"><span class="label">Dribbble</span></a></li>
						<li><a href="#" class="icon fa-google-plus"><span class="label">Google+</span></a></li>
					</ul>
					<!--<ul class="copyright">
						<li>&copy; Untitled. All rights reserved.</li><li>Design: <a href="http://html5up.net">HTML5 UP</a></li>
					</ul>-->
				</footer>

		</div>

		<!-- Scripts -->
			<script src="assets/js/jquery.min.js"></script>
			<script src="assets/js/jquery.dropotron.min.js"></script>
			<script src="assets/js/jquery.scrollgress.min.js"></script>
			<script src="assets/js/skel.min.js"></script>
			<script src="assets/js/util.js"></script>
			<!--[if lte IE 8]><script src="assets/js/ie/respond.min.js"></script><![endif]-->
			<script src="assets/js/main.js"></script>

	</body>
</html>							