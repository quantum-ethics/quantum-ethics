<?

$file = dirname( __FILE__ ).DIRECTORY_SEPARATOR.'Quantum Field Theory.pdf';

$headers = array(
'Accept-Ranges' => 'bytes',
'Connection' => 'Keep-Alive',
'Content-Length' => filesize( $file ),
'Content-Type' => 'application/pdf',
'Date' => date( 'r' ),
'Keep-Alive' => 'timeout=15, max=100',
'Last-Modified' => date( 'r', filemtime( $file ) ),
'Content-Disposition' => 'attachment; filename="Quantum Ethics.pdf"',
);

foreach( $headers as $key => $value ) {
	header( "{$key}: {$value}" );
}

readfile( $file );

include_once '../../visitors.php';

?>