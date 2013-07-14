<?

if ($_SERVER['SERVER_NAME'] !== 'quantum-ethics.org') {
	header('Location: http://quantum-ethics.org/', true, 301);
	exit;
}

function latex2html($latex) {
	$html = $latex;
	foreach(array(
		'&' => '&amp;',
		'<' => '&lt;',
		'>' => '&gt;',
		'``' => '“',
		'\'\'' => '”',
		'\'' => '’',
		'`' => '‘',
		'--' => '–',
	) as $search => $replace) {
		$html = str_replace($search, $replace, $html);
	}
	foreach(array(
		'#\\\\chapter\\*{([^}]*+)}#' => "\t\t\t<h4>$1</h4>",
		'#\\\\(?:addcontentsline|renewcommand|epigraph)[^\\n]*+#' => '',
		'#\\n\\n([^\\n]++)#' => "\t\t\t<p>$1</p>\n",
		'#\\\\textit{([^}]*+)}#' => '<i>$1</i>',
		'#\\n\\n++#' => "\n",
	) as $search => $replace) {
		$html = preg_replace($search, $replace, $html);
	}
	return $html;
}

/**
 * Send a GET requst using cURL
 * @param string $url to request
 * @param array $get values to send
 * @param array $options for cURL
 * @return string
 */
function curl_get($url, array $get = array(), array $options = array()) {
	$urlFull = $url;
	if(count($get)) {
		if(strpos($urlFull, '?') === false) {
			$urlFull .= '?';
		}
		$urlFull .= http_build_query($get);
	}
	$defaults = array(
		CURLOPT_URL => $urlFull,
		CURLOPT_HEADER => 0,
		CURLOPT_RETURNTRANSFER => true,
		CURLOPT_TIMEOUT => 4
	);
	$ch = curl_init();
	curl_setopt_array($ch, ($options + $defaults));
	if(!$result = curl_exec($ch)) {
		trigger_error(curl_error($ch));
	}
	curl_close($ch);
	return $result;
} 

?><!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="content-type" content="text/html;charset=UTF-8">
		<meta name="language" content="english">
		<meta name="description" content="Quantum Ethics: A Spinozist Interpretation of Constructive Quantum Field Theory on the Lattice, by Sébastien Fauvel">
		<title>Quantum Ethics: A Spinozist Interpretation of Constructive Quantum Field Theory on the Lattice</title>
		<link href="style.css" rel="stylesheet" type="text/css">
		<script src="http://code.jquery.com/jquery-1.9.1.min.js" type="text/javascript"></script>
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
			<h2>&mdash;<br>A Spinozist Interpretation<br>of Constructive Quantum Field Theory<br>on the Lattice</h2>
			<h3><br><a href="http://ens.academia.edu/S%C3%A9bastienFauvel">Sébastien Fauvel</a></h3>
		</div>
		<div id="abstract">
			<div id="download">
				<a href="http://www.amazon.com/Quantum-Ethics-Spinozist-Interpretation-Theory/dp/1481811703/">
					<img src="images/pictures/One_Loop_QED_Vacuum_Energy_Diagram.jpg" width="250" height="325" border="0" alt="" title="One-Loop QED Vacuum Energy Diagram">
				</a>
				<p><a href="http://quantum-ethics.org/Quantum%20Ethics.php">» View as PDF</a></p>
				<p><a href="http://www.amazon.com/Quantum-Ethics-Spinozist-Interpretation-Theory/dp/1481811703/">» Order on Amazon</a></p>
				<p><a href="https://github.com/sebastien-fauvel/quantum-ethics">» Contribute on GitHub</a></p><br>
			</div>
<? print latex2html(curl_get('https://raw.github.com/sebastien-fauvel/quantum-ethics/master/chapter-history-of-this-book.tex')); ?>
			<p><a href="http://quantum-ethics.org/Quantum%20Ethics.php">» Read full PDF</a></p>
			<h4>About the Author</h4>
			<p>Sébastien Fauvel, born 1983, graduated from the Ecole Normale Supérieure of Paris in Physics and Comparative Literature. He has been working as a Consultant, Software and Web Developer in Lyon, Freiburg and Basel.</p>
		</div>
	</body>
</html><? include_once '../../visitors.php';
