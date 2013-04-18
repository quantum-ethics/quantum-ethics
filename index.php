<?

if ($_SERVER['SERVER_NAME'] !== 'quantum-ethics.org') {
	header('Location: http://quantum-ethics.org/', true, 301);
	exit;
}

?><!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="content-type" content="text/html;charset=UTF-8">
		<meta name="language" content="english">
		<meta name="description" content="Quantum Ethics - A Spinozist Interpretation of Quantum Field Theory, by Sébastien Fauvel">
		<title>Quantum Ethics - A Spinozist Interpretation of Quantum Field Theory</title>
		<link rel="stylesheet" type="text/css;charset=UTF-8" href="style.css">
		<script src="http://code.jquery.com/jquery-1.9.1.min.js"></script>
	</head>
	<body>
		<script type="text/javascript">
			/*<![CDATA[*/
			$(function() {
				$('body').css('background-image', 'url(background.php?w=' + window.screen.width + '&h=' + window.screen.height + ')');
				$('a').click(function(event) {
					$.get('click.php', {url: $(event.target).closest('a').attr('href')});
					return true;
				});
			});
			/*]]>*/
		</script>
		<div id="title">
			<h1>Quantum Ethics</h1>
			<h2>&mdash;<br>A Spinozist Interpretation<br>of Quantum Field Theory</h2>
			<h3><br><a href="http://ens.academia.edu/S%C3%A9bastienFauvel">Sébastien Fauvel</a></h3>
		</div>
		<div id="abstract">
			<div id="download">
				<a href="http://www.amazon.com/Quantum-Ethics-Spinozist-Interpretation-Theory/dp/1481811703/">
					<img src="images/pictures/One_Loop_QED_Vacuum_Energy_Diagram.jpg" width="250" height="325" border="0" alt="" title="One-Loop QED Vacuum Energy Diagram">
				</a>
				<p><a href="http://quantum-ethics.org/Quantum%20Ethics.php">» View as PDF</a></p>
				<p><a href="http://www.amazon.com/Quantum-Ethics-Spinozist-Interpretation-Theory/dp/1481811703/">» Order on Amazon</a></p>
				<p><a href="https://github.com/fauvel/Quantum-Ethics">» Contribute on GitHub</a></p><br>
			</div>
			<h4>Abstract</h4>
			<p>This groundbreaking work is bringing together quantum physicists and philosophers in an unprecedented way... Making a tabula rasa from all its historical and heuristical developments, it presents Quantum Field Theory in its modern form in a exquisitely simple mathematical apparatus, nonetheless completely well-defined and fully functional... Focusing on the underlying ontology of Quantum Field Theory, which turns out to be very similar to the classical metaphysics of Spinoza's Ethics, it provides a solid fundament for any philosophical discussion of its interpretation.</p>
			<p>This book ought to become compulsory reading not only for all philosophers interested by the metaphysical and ethical implications of Quantum Physics, but also for all physicists working on the foundations of Physics, on Quantum Measurement and on Decoherence Theory.</p>
			<h4>About the Author</h4>
			<p>Sébastien Fauvel, born 1983, graduated from the Ecole Normale Supérieure of Paris in Physics and Comparative Literature. He has been working as a Consultant, Software and Web Developer in Lyon, Freiburg and Basel.</p>
			<h4>Preface</h4>
			<p>This book has been written with the main concern of providing the scientific community with a mean of building a bridge between physicists and philosophers in the field of Quantum Physics. It defines a common language to describe the realm of our experience of the world and I truly hope that this new language will find a large audience in both communities. For physicists, it stresses the importance of developing a well-defined mathematical formalism for Quantum Field Theory, since this is the necessary condition for philosophers to identify the underlying ontology, which builds the base of every philosophical discourse on the implications of Quantum Physics. The development of a coherent, convincing discourse by philosophers builds in turn the ground on which every conceptually and technically correct vulgarization effort can foot, and contributes thereby to broaden the popularity and acceptance of Quantum Physics through the whole society and especially among prospective students. Philosophers, on the other hand, gain a mean of confronting their ideas with the latest insights into fundamental physics, of expressing these ideas in terms of naturalistically grounded metaphysics and of articulating speculative thoughts in the uncertainty zones remaining within the physical theory itself or inherent to it.</p>
			<p>The careful reader already familiar with Quantum Physics will find here the very first mathematically well-defined formulation of Quantum Field Theory coming along with an intellectually satisfactory interpretation, perfectly capable of explaining all known quantum phenomena, and I am very pleased to present it to your curiosity today! However, although you will find, in the first edition of this book, all the main ideas I wanted to develop here, there still are a few areas which I couldn't find the time to develop to the extend they would have deserved yet. For instance, I do consider the formulation of Quantum Field Theory presented in this book, which is essentially a lattice regularization, as an acceptable fundamental theory, although it obviously lacks to respect the heuristical principles of Gauge and Poincaré invariance on which any textbook introduction to Quantum Field Theory relies. I included a few calculation examples in order to demonstrate that, at usual energy and distance scales, the resulting physics is the same as expected, but it would have been useful, to convince the skeptical reader, to add a classical renormalization example, a treatment of the weak and strong interactions as well as of the Higgs mechanism and of some gravitational model. The philosophical aspects of this book might have deserved a more extensive treatment, too. I have given a few basic examples to show how the physical theory can be used as a reference language in order to express philosophical questions, but I would have to add more various examples to give an adequate idea of the potential of this method.</p>
			<p>I am preparing a second, augmented edition of this work so that the ideas presented here become more explicit and accessible to a broader public. For the time being, I wish you a fruitful and enjoyable reading as well as long hours of delightful meditation!</p>
			<p><? $mtime = filemtime('Quantum Field Theory.pdf');
				print date('F, Y', $mtime ? $mtime : time()); ?><br>Sébastien Fauvel</p>
		</div>
	</body>
</html><? include_once '../../visitors.php';
