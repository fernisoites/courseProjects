<?php
	session_start();
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
		<link rel="stylesheet" href="assets/css/main.css" />
		<!--[if lte IE 8]><link rel="stylesheet" href="assets/css/ie8.css" /><![endif]-->
					<!--<style>
                    #example1{
                        background: url(images/background.jpg);
                        background-position: left top;
                        background-repeat: no-repeat;
                    }
                    </style>
                -->
	</head>
	<div id="page-wrapper">
	<body class="landing">
	

            
			<!-- Header -->
				<header id="header">

					<img src="images/bird_ticket.png", style="width:80px">

					<h1><font size="4"><a href="index.php"><ul><ul><ul><ul><ul>UTickets</font></h1>
					<nav id="nav">
						<ul>

								<!--<a href="#" class="icon fa-angle-down">Layouts</a>
								<ul>
									<li><a href="generic.html">Generic</a></li>
									<li><a href="contact.html">Contact</a></li>
									<li><a href="elements.html">Elements</a></li>
									<li>
										<a href="#">Submenu</a>
										<ul>
											<li><a href="#">Option One</a></li>
											<li><a href="#">Option Two</a></li>
											<li><a href="#">Option Three</a></li>
											<li><a href="#">Option Four</a></li>
										</ul>
									</li>
								</ul>-->
							</li>
							<?php 
								if(isset($_SESSION['username'])) 
								{
							?>
								<li><a href="memberHome.php">Welcome, <?php echo $_SESSION['username'];?></a></li>
								<li><a href="about.php">About Us</a></li>
								<li>
								<li><a href="logout.php" class="button">Log Out</a></li>
							<?php  
								} else {
							?>
								<li><a href="about.php">About Us</a></li>
								<li>
								<li><a href="signup.html" class="button">Sign Up</a></li>
								<li><a href="login.php" class="button">Sign In</a></li>
							<?php  
								}
							?>
						</ul>

					</nav>

				</header>
            
			<!-- Banner -->
				<section id="banner">
					

					<h2>U T i c k e t s</h2>
					<p>Your best ticket master!</p>
					<?php 
								if(isset($_SESSION['username'])) 
								{
							?>
					<ul class="actions">
						<li><a href="memberHome.php" class="button special">Your Account</a></li>
						<li><a href="about.php" class="button">Learn More</a></li>
					</ul>
					<?php  
								} else {
							?>
					<ul class="actions">
						<li><a href="signup.html" class="button special">Sign Up</a></li>
						<li><a href="about.php" class="button">Learn More</a></li>
					</ul>
					<?php  
								}
							?>

				</section>

			<!-- Main -->
				<section id="main" class="container">

					<section class="box special">
						<header class="major">
							<h2>What is UTickets?</h2>
							<p> • UTickets, where smart fans buy and sell tickets. •
								<br>
								• If you want to sell your ticket, just post it here! •
								<br>
								• If you want to buy tickets, we have the best price! •
								<br>
								• Our Machine Learning Algorithm will provide the recommended price! •
							</p>
						</header>
						<!--<span class="image featured"><img src="images/universities.jpeg" alt="" /></span>-->
					</section>

					<!--<section class="box special features">
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

					</section>-->

					<div class="row">
						<div class="6u 12u(narrower)">

							<section class="box special">

								<h3>Hot Games</h3>
		
								<table style="width:100%">
								  <tr>
								  	<td>Football</td>
								    <td>THUNDERBIRDS vs UTES</td> 
								    <td>09/01/16</td>
								  </tr>
								  <tr>
								  	<td>Football</td>
								    <td>WILDCATS vs CARDINAL</td> 
								    <td>09/01/16</td>
								  </tr>
								  <tr>
								  	<td>BasketBall</td>
								    <td>UVA vs VT</td> 
								    <td>09/01/16</td>
								  </tr>
								</table>
		
							</section>

						</div>
						<div class="6u 12u(narrower)">

							<section class="box special">

								<h3>Hot Tickets</h3>
		
								<table style="width:100%">
								  <tr>
								  	<td>Football</td>
								    <td>THUNDERBIRDS vs UTES</td> 
								    <td>From $58</td>
								  </tr>
								  <tr>
								  	<td>Football</td>
								    <td>WILDCATS vs CARDINAL</td> 
								    <td>From $48</td>
								  </tr>
								  <tr>
								  	<td>BasketBall</td>
								    <td>UVA vs VT</td> 
								    <td>From $68</td>
								  </tr>
								</table>								
							</section>

						</div>
					</div>

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