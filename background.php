<?

$original_width = 1280;
$original_height = 1024;

$default_width = 1280;
$default_height = 1024;

$max_width = $default_width * 2;
$max_height = $default_height * 2;

$width = ( intval( $_GET['w'] ) > 0 ) && ( intval( $_GET['w'] ) <= $max_width ) ? intval( $_GET['w'] ) : $default_width;
$height = ( intval( $_GET['h'] ) > 0 ) && ( intval( $_GET['h'] ) <= $max_height ) ? intval( $_GET['h'] ) : $default_height;

$scale = max( $width / $original_width, $height / $original_height );

$width = round( $original_width * $scale );
$height = round( $original_height * $scale );

$images_directory = 'images/pictures/';

$original_image_file = 'hs-2007-16-f-1280_wallpaper.jpg';
$resized_image_file = "hs-2007-16-f-1280_wallpaper_{$width}x{$height}.jpg";

if( !file_exists( $images_directory.$resized_image_file ) ) {
	resizeImage();
} else {
	$original_image_ctime = filectime( $images_directory.$original_image_file );
	$resized_image_ctime = filectime( $images_directory.$resized_image_file );
	if( $original_image_ctime >= $resized_image_ctime ) {
		resizeImage();
	}
}

header( 'location:'.$images_directory.rawurlencode( $resized_image_file ) );

function resizeImage() {
	global $images_directory, $original_image_file, $width, $height, $resized_image_file;
	$original_image = imagecreatefromjpeg( $images_directory.$original_image_file );
	$resized_image = imagecreatetruecolor( $width, $height );
	imagecopyresampled( $resized_image, $original_image, 0, 0, 0, 0, $width, $height, 1024, 768 );
	imagejpeg( $resized_image, $images_directory.$resized_image_file );
}

?>
