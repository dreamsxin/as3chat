<?php
/**
 * 图像处理支持 - GD
 *
 * @author	Zhuzx
 * @email dreamsxin@qq.com
 * @date	2010-12-30
 */
class GD extends Image {

	// GD库bundled版本标识，
	protected static $_checked;

	/**
	 * 检查GD库是否开启，以及是版本
	 */
	public static function check()
	{
		if ( ! function_exists('gd_info')) {
			throw new Exception('ImageGD requires GD version 2.0.1 or greater. Please see http://php.net/gd_info for more information.');
		}

		if (defined('GD_VERSION')) {
			$version = GD_VERSION;
		} else {
			$info = gd_info();
			preg_match('/\d+\.\d+(?:\.\d+)?/', $info['GD Version'], $matches);
			$version = $matches[0];
		}

		if ( ! version_compare($version, '2.0.1', '>=')) {
			throw new Exception('ImageGD requires GD version 2.0.1 or greater, you have '.$version);
		}
		self::$_checked = TRUE;
	}

	protected $_image;
	protected $_create_function;

	/**
	 * 检查GD库信息，加载图片资源
	 */
	public function __construct($file)
	{
		if ( ! self::$_checked) {
			self::check();
		}

		parent::__construct($file);

		switch ($this->type)
		{
			case IMAGETYPE_JPEG:
				$create = 'imagecreatefromjpeg';
			break;
			case IMAGETYPE_GIF:
				$create = 'imagecreatefromgif';
			break;
			case IMAGETYPE_PNG:
				$create = 'imagecreatefrompng';
			break;
		}

		if ( ! isset($create) || ! function_exists($create)) {
			throw new Exception('Installed GD does not support '.image_type_to_extension($this->type, FALSE).' images');
		}

		$this->_create_function = $create;
		$this->_image = $this->file;
	}

	/**
	 * 释放资源
	 */
	public function __destruct()
	{
		if (is_resource($this->_image)) {
			imagedestroy($this->_image);
		}
	}

	/**
	 * 从给定的文件取得的图像
	 */
	protected function _load_image()
	{
		if ( ! is_resource($this->_image)) {
			$create = $this->_create_function;
			$this->_image = $create($this->file);

			imagesavealpha($this->_image, TRUE);
		}
	}

	protected function _do_resize($width, $height)
	{
		$pre_width = $this->width;
		$pre_height = $this->height;

		$this->_load_image();

		if ($width > ($this->width / 2) && $height > ($this->height / 2)) {
			$reduction_width  = round($width  * 1.1);
			$reduction_height = round($height * 1.1);

			while ($pre_width / 2 > $reduction_width  &&  $pre_height / 2 > $reduction_height) {
				$pre_width /= 2;
				$pre_height /= 2;
			}

			$image = $this->_create($pre_width, $pre_height);

			if (imagecopyresized($image, $this->_image, 0, 0, 0, 0, $pre_width, $pre_height, $this->width, $this->height)) {
				imagedestroy($this->_image);
				$this->_image = $image;
			}
		}

		$image = $this->_create($width, $height);

		if (imagecopyresampled($image, $this->_image, 0, 0, 0, 0, $width, $height, $pre_width, $pre_height)) {
			imagedestroy($this->_image);
			$this->_image = $image;

			$this->width  = imagesx($image);
			$this->height = imagesy($image);
		}
	}

	protected function _do_crop($width, $height, $offset_x, $offset_y)
	{
		$image = $this->_create($width, $height);

		$this->_load_image();

		if (imagecopyresampled($image, $this->_image, 0, 0, $offset_x, $offset_y, $width, $height, $width, $height)) {
			imagedestroy($this->_image);
			$this->_image = $image;

			$this->width  = imagesx($image);
			$this->height = imagesy($image);
		}
	}

	protected function _do_rotate($degrees)
	{
		$this->_load_image();

		$transparent = imagecolorallocatealpha($this->_image, 0, 0, 0, 127);

		$image = imagerotate($this->_image, 360 - $degrees, $transparent, 1);

		imagesavealpha($image, TRUE);

		$width  = imagesx($image);
		$height = imagesy($image);

		if (imagecopymerge($this->_image, $image, 0, 0, 0, 0, $width, $height, 100)) {
			imagedestroy($this->_image);
			$this->_image = $image;

			$this->width  = $width;
			$this->height = $height;
		}
	}

	protected function _do_flip($direction)
	{
		$flipped = $this->_create($this->width, $this->height);

		$this->_load_image();

		if ($direction === parent::HORIZONTAL) {
			for ($x = 0; $x < $this->width; $x++) {
				imagecopy($flipped, $this->_image, $x, 0, $this->width - $x - 1, 0, 1, $this->height);
			}
		} else {
			for ($y = 0; $y < $this->height; $y++)
			{
				imagecopy($flipped, $this->_image, 0, $y, 0, $this->height - $y - 1, $this->width, 1);
			}
		}

		imagedestroy($this->_image);
		$this->_image = $flipped;

		$this->width  = imagesx($flipped);
		$this->height = imagesy($flipped);
	}

	protected function _do_watermark(proImage $watermark, $offset_x, $offset_y, $opacity)
	{
		$this->_load_image();

		$overlay = imagecreatefromstring($watermark->render());

		$width  = imagesx($overlay);
		$height = imagesy($overlay);

		if ($opacity < 100) {
			$opacity = round(abs(($opacity * 127 / 100) - 127));
			$color = imagecolorallocatealpha($overlay, 255, 255, 255, $opacity);
			imagelayereffect($overlay, IMG_EFFECT_OVERLAY);
			imagefilledrectangle($overlay, 0, 0, $width, $height, $color);
		}

		imagealphablending($this->_image, TRUE);

		if (imagecopy($this->_image, $overlay, $offset_x, $offset_y, 0, 0, $width, $height)) {
			imagedestroy($overlay);
		}
	}

	protected function _do_background($r, $g, $b, $opacity)
	{
		$this->_load_image();

		$opacity = round(abs(($opacity * 127 / 100) - 127));
		$background = $this->_create($this->width, $this->height);
		$color = imagecolorallocatealpha($background, $r, $g, $b, $opacity);

		imagefilledrectangle($background, 0, 0, $this->width, $this->height, $color);
		imagealphablending($background, TRUE);

		if (imagecopy($background, $this->_image, 0, 0, 0, 0, $this->width, $this->height)) {
			imagedestroy($this->_image);
			$this->_image = $background;
		}
	}

	protected function _do_mask(proImage $image)
	{
		$this->_load_image();

		$mask = imagecreatefromstring($image->render());

		$width  = imagesx($mask);
		$height = imagesy($mask);

		$newimage = $this->_create($this->width, $this->height);

		imagesavealpha($newimage, true);
    		imagefill($newimage, 0, 0, imagecolorallocatealpha( $newimage, 0, 0, 0, 127));
   
		if($this->width != $width || $this->height != $height) {
			$tempPic = imagecreatetruecolor($this->width, $this->height);
			imagecopyresampled( $tempPic, $mask, 0, 0, 0, 0, $this->width, $this->height, $width, $height);
			imagedestroy( $mask );
			$mask = $tempPic;
		}

		for( $x = 0; $x < $this->width; $x++ ) {
			for( $y = 0; $y < $this->height; $y++ ) {
			    $alpha = imagecolorsforindex($mask, imagecolorat($mask, $x, $y));
			    $alpha = 127 - floor($alpha['red'] / 2);
			    $color = imagecolorsforindex($this->_image, imagecolorat($this->_image, $x, $y ));
			    imagesetpixel( $newimage, $x, $y, imagecolorallocatealpha( $newimage, $color['red'], $color['green'], $color['blue'], $alpha));
			}
		}

		imagedestroy($this->_image);
		$this->_image = $newimage;
		imagedestroy($mask);
	}

	protected function _do_save($file, $quality)
	{
		$this->_load_image();

		$extension = pathinfo($file, PATHINFO_EXTENSION);

		list($save, $type) = $this->_save_function($extension, $quality);

		$status = isset($quality) ? $save($this->_image, $file, $quality) : $save($this->_image, $file);

		if ($status === TRUE  &&  $type !== $this->type) {
			$this->type = $type;
			$this->mime = image_type_to_mime_type($type);
		}

		return TRUE;
	}

	protected function _do_render($type, $quality)
	{
		$this->_load_image();

		list($save, $type) = $this->_save_function($type, $quality);

		ob_start();

		$status = isset($quality) ? $save($this->_image, NULL, $quality) : $save($this->_image, NULL);

		if ($status === TRUE  &&  $type !== $this->type) {
			$this->type = $type;
			$this->mime = image_type_to_mime_type($type);
		}

		return ob_get_clean();
	}

	/**
	 * 获取GD保存函数以及图像类型
	 *
	 * @param   string   image type: png, jpg, etc
	 * @param   integer  image quality
	 * @return  array    save function, IMAGETYPE_* constant
	 */
	protected function _save_function($extension, & $quality)
	{
		switch (strtolower($extension))
		{
			case 'jpg':
			case 'jpeg':
				$save = 'imagejpeg';
				$type = IMAGETYPE_JPEG;
			break;
			case 'gif':
				$save = 'imagegif';
				$type = IMAGETYPE_GIF;

				$quality = NULL;
			break;
			case 'png':
				$save = 'imagepng';
				$type = IMAGETYPE_PNG;

				$quality = 9;
			break;
			default:
				throw new Exception('Installed GD does not support '.$extension.' images');
			break;
		}

		return array($save, $type);
	}

	/**
	 * 建立空白的图片资源
	 *
	 * @param   integer   宽
	 * @param   integer   高
	 * @return  resource
	 */
	protected function _create($width, $height)
	{
		$image = imagecreatetruecolor($width, $height);
		imagealphablending($image, FALSE);
		imagesavealpha($image, TRUE);
		return $image;
	}

} // End ImageGD
