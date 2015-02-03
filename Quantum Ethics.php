<?

if ($_SERVER['SERVER_NAME'] !== 'quantum-ethics.org') {
	header('Location: http://quantum-ethics.org/Quantum%20Ethics.php', true, 301);
	exit;
}

$file = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'quantum-ethics.pdf';

$headers = array(
	'Accept-Ranges'       => 'bytes',
	'Connection'          => 'Keep-Alive',
	'Content-Length'      => filesize($file),
	'Content-Type'        => 'application/pdf',
	'Date'                => date('r'),
	'Keep-Alive'          => 'timeout=15, max=100',
	'Last-Modified'       => date('r', filemtime($file)),
	'Content-Disposition' => 'attachment; filename="Quantum Ethics.pdf"',
);

foreach ($headers as $key => $value) {
	header("{$key}: {$value}");
}

readfile($file);
